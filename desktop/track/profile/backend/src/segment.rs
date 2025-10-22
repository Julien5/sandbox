use geo::LineLocatePoint;

use crate::inputpoint::{InputPoint, InputPointMap, InputType, TrackProjection};
use crate::mercator::{EuclideanBoundingBox, MercatorPoint};
use crate::parameters::Parameters;
use crate::track::{self, Track};
use crate::{bboxes, gpsdata, locate, make_points, profile, svgmap};

#[derive(Clone)]
pub struct Segment {
    pub id: usize,
    pub range: std::ops::Range<usize>,
    pub profile_bbox: gpsdata::ProfileBoundingBox,
    pub map_bbox: EuclideanBoundingBox,
    pub track_tree: locate::IndexedPointsTree,
    pub track: std::sync::Arc<Track>,
    pub points: Vec<InputPoint>,
}

pub struct SegmentStatistics {
    pub length: f64,
    pub elevation_gain: f64,
    pub distance_start: f64,
    pub distance_end: f64,
}

impl Segment {
    pub fn new(
        id: usize,
        range: std::ops::Range<usize>,
        bbox: &gpsdata::ProfileBoundingBox,
        mbbox: &EuclideanBoundingBox,
        track_tree: locate::IndexedPointsTree,
        track: std::sync::Arc<Track>,
        inputpoints: &InputPointMap,
    ) -> Segment {
        let points = Self::copy_segment_points(inputpoints, mbbox, &track, &track_tree);
        Segment {
            id,
            range: range.clone(),
            profile_bbox: bbox.clone(),
            map_bbox: mbbox.clone(),
            track_tree: track_tree,
            track,
            points,
        }
    }

    pub fn render_profile(&self, (width, height): (i32, i32), parameters: &Parameters) -> String {
        log::info!("render profile:{}", self.id);
        let points = self.profile_points(parameters);
        log::trace!("points:{:?}", points.len());
        let ret = profile::profile(
            &self.track,
            &points,
            &self,
            &parameters.profile_options,
            width,
            height,
        );
        if parameters.debug {
            let filename = std::format!("/tmp/profile-{}.svg", self.id);
            std::fs::write(filename, &ret).expect("Unable to write file");
        }
        ret
    }

    fn middle_point(a: &(f64, f64, f64), b: &(f64, f64, f64), alpha: f64) -> (f64, f64, f64) {
        let ab = (b.0 - a.0, b.1 - a.1, b.2 - a.2);
        (a.0 + alpha * ab.0, a.1 + alpha * ab.1, a.2 + alpha * ab.2)
    }

    fn two_closest_index(track: &track::Track, index: &usize, p: &InputPoint) -> (usize, usize) {
        let tracklen = track.euclidian.len();
        if *index == 0 {
            return (0, 1);
        }
        if *index == tracklen - 1 {
            return (index - 1, *index);
        }
        let dbefore = p.euclidian.d2(&track.euclidian[index - 1]);
        let dafter = p.euclidian.d2(&track.euclidian[index - 1]);
        if dbefore < dafter {
            (index - 1, *index)
        } else {
            (*index, *index + 1)
        }
    }

    fn compute_track_projection(
        track: &track::Track,
        tracktree: &locate::IndexedPointsTree,
        point: &mut InputPoint,
    ) {
        let index = tracktree.nearest_neighbor(&point.euclidian).unwrap();
        let (index1, index2) = Self::two_closest_index(track, &index, point);
        let p1 = &track.euclidian[index1];
        let p2 = &track.euclidian[index2];
        let linestring: geo::LineString = vec![p1.xy(), p2.xy()].into();
        let index_floating_part = linestring
            .line_locate_point(&geo::point!(point.euclidian.xy()))
            .unwrap();
        assert!(0.0 <= index_floating_part && index_floating_part <= 1f64);
        let floating_index = index1 as f64 + index_floating_part;
        let t1 = &track.euclidian[index1];
        let t2 = &track.euclidian[index2];
        let a1 = (t1.0, t1.1, track.elevation(index1));
        let a2 = (t2.0, t2.1, track.elevation(index2));
        let m = Self::middle_point(&a1, &a2, index_floating_part);
        let euclidean = MercatorPoint::from_xy(&(m.0, m.1));
        let elevation = m.2;
        let track_distance = euclidean.d2(&point.euclidian).sqrt();

        // check
        let di = point.euclidian.d2(&track.euclidian[index]);
        let df = point.euclidian.d2(&euclidean);
        debug_assert!(df <= di);

        point.track_projection = Some(TrackProjection {
            track_floating_index: floating_index,
            track_index: index1,
            euclidean,
            elevation,
            track_distance,
        });
    }

    fn copy_segment_points(
        inputpoints: &InputPointMap,
        _bbox: &EuclideanBoundingBox,
        track: &track::Track,
        tracktree: &locate::IndexedPointsTree,
    ) -> Vec<InputPoint> {
        let mut ret = Vec::new();
        let mut bbox = _bbox.clone();
        // a bit too much enlarging, probably.
        bbox.enlarge(&5000f64);
        let bboxs = bboxes::split(&bbox, &bboxes::BBOXWIDTH);
        for (_index, bbox) in bboxs {
            let _points = inputpoints.get(&bbox);
            if _points.is_none() {
                continue;
            }
            let points = _points.unwrap();
            for p in points {
                let mut c = p.clone();
                Self::compute_track_projection(track, tracktree, &mut c);
                ret.push(c);
            }
        }
        ret
    }

    fn distance_to_track(&self, p: &InputPoint) -> f64 {
        match &p.track_projection {
            Some(proj) => proj.track_distance,
            None => {
                assert!(false);
                f64::MAX
            }
        }
    }

    pub fn profile_points(&self, parameters: &Parameters) -> Vec<InputPoint> {
        make_points::profile_points(&self, parameters)
    }

    pub fn render_map(&self, (width, height): (i32, i32), parameters: &Parameters) -> String {
        log::info!("render map:{}", self.id);
        let points = self.map_points(parameters);
        let ret = svgmap::map(&self.track, &points, &self, width, height, parameters.debug);
        if parameters.debug {
            let filename = std::format!("/tmp/map-{}.svg", self.id);
            std::fs::write(filename, &ret).expect("Unable to write file");
        }
        ret
    }

    fn placement_order_map(point: &InputPoint) -> usize {
        if point.name().is_none() {
            return usize::MAX - 10;
        }
        match point.kind() {
            InputType::Hamlet => {
                return 5;
            }
            InputType::Village => {
                return 4;
            }
            InputType::MountainPass | InputType::Peak => {
                return 3;
            }
            InputType::GPX => {
                return 2;
            }
            InputType::City => {
                return 1;
            }
        }
    }

    fn map_points(&self, parameters: &Parameters) -> Vec<InputPoint> {
        let profile = self.profile_points(parameters);
        // at most 15 points on the map.
        let nextra = if profile.len() >= 15 {
            0
        } else {
            (15 - profile.len()).max(0)
        };
        if nextra == 0 {
            return profile.clone();
        }
        let mut extra = self.points.clone();
        extra.retain(|p| {
            if profile.contains(&p) {
                return false;
            }
            if !self.map_bbox.contains(&p.euclidian.xy()) {
                return false;
            }
            true
        });
        extra.sort_by_key(|p| Self::placement_order_map(&p));
        extra.truncate(nextra);
        let mut ret = profile.clone();
        ret.extend_from_slice(&extra);
        for w in &mut ret {
            if profile.contains(&w) {
                assert!(w.label_placement_order < usize::MAX);
            } else {
                w.label_placement_order = Self::placement_order_map(&w) + profile.len();
            }
        }
        ret
    }
}
