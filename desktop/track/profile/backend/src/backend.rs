#![allow(non_snake_case)]

use std::str::FromStr;

use crate::gpsdata;
use crate::gpsdata::ProfileBoundingBox;
use crate::gpsdata::WaypointOrigin;
use crate::pdf;
use crate::project;
use crate::render;

pub struct Backend {
    track: gpsdata::Track,
    waypoints: Vec<gpsdata::Waypoint>,
    _epsilon: f32,
    _shift: f64,
}

#[derive(Clone)]
pub struct Segment {
    pub id: usize,
    pub range: std::ops::Range<usize>,
}

pub struct SegmentStatistics {
    pub length: f64,
    pub elevation_gain: f64,
    pub distance_start: f64,
    pub distance_end: f64,
}

impl Segment {
    pub fn new(id: usize, range: std::ops::Range<usize>) -> Segment {
        Segment { id, range }
    }
}

impl Backend {
    fn enrichWaypoints(&mut self) {
        // not fast.
        let mut waypoints = Vec::new();
        for w in &self.waypoints {
            match w.origin {
                WaypointOrigin::GPX => waypoints.push(w.clone()),
                _ => {}
            }
        }
        let indexes = self.track.interesting_indexes(self._epsilon);
        for idx in indexes {
            let wgs = self.track.wgs84[idx].clone();
            let utm = self.track.utm[idx].clone();
            waypoints.push(gpsdata::Waypoint::from_track(wgs, utm, idx));
        }
        let indexes = project::nearest_neighboor(&self.track.utm, &self.waypoints);
        debug_assert_eq!(self.waypoints.len(), indexes.len());
        for k in 0..indexes.len() {
            self.waypoints[k].track_index = indexes[k];
        }
        // sort
        self.waypoints
            .sort_by(|w1, w2| w1.track_index.cmp(&w2.track_index));
        for k in 1..self.waypoints.len() {
            let k1 = self.waypoints[k].track_index;
            let k0 = self.waypoints[k - 1].track_index;
            debug_assert!(k1 >= k0);
        }
        self.waypoints = waypoints;
    }

    pub fn new(filename: &str) -> Backend {
        let filename = String::from_str(filename).unwrap();
        let mut gpx = gpsdata::read_gpx(filename.as_str());
        let segment = gpsdata::read_segment(&mut gpx);
        let track = gpsdata::Track::from_segment(&segment);
        let km = 1000f64;
        let mut ret = Backend {
            track,
            waypoints: gpsdata::read_waypoints(&gpx),
            _epsilon: 70.0f32,
            _shift: 100f64 * km,
        };
        ret.enrichWaypoints();
        ret
    }

    pub fn changeParameter(&mut self, delta: f32) {
        self._epsilon += delta;
    }

    pub fn epsilon(&self) -> f32 {
        self._epsilon
    }

    pub fn render_track(&mut self) -> String {
        self.enrichWaypoints();
        let range = 0..self.track.len();
        let viewBox = ProfileBoundingBox::from_track(&self.track, &range);
        render::track_profile(&self.track, &range, &viewBox)
    }
    pub fn render_waypoints(&mut self) -> String {
        self.enrichWaypoints();
        let range = 0..self.track.len();
        let viewBox = ProfileBoundingBox::from_track(&self.track, &range);
        render::waypoints_profile(&self.track, &self.waypoints, &range, &viewBox)
    }

    pub fn segments(&self) -> Vec<Segment> {
        let mut ret = Vec::new();

        let mut start = 0f64;
        let mut k = 0usize;
        loop {
            let end = start + self._shift;
            let range = self.track.segment(start, end);
            if range.is_empty() {
                break;
            }
            ret.push(Segment::new(k, range));
            start = start + self._shift;
            k = k + 1;
        }
        ret
    }
    pub fn render_segment_track(&mut self, segment: &Segment) -> String {
        println!("render_segment_track:{}", segment.id);
        let range = &segment.range;
        self.enrichWaypoints();
        let bbox = ProfileBoundingBox::from_track(&self.track, &range);
        render::track_profile(&self.track, &range, &bbox)
    }
    pub fn render_segment_waypoints(&mut self, segment: &Segment) -> String {
        println!("render_segment_waypoints:{}", segment.id);
        let range = &segment.range;
        self.enrichWaypoints();
        let bbox = ProfileBoundingBox::from_track(&self.track, &range);
        render::waypoints_profile(&self.track, &self.waypoints, &range, &bbox)
    }
    pub fn segment_statistics(&self, segment: &Segment) -> SegmentStatistics {
        let range = &segment.range;
        SegmentStatistics {
            length: self.track.distance(range.end - 1) - self.track.distance(range.start),
            elevation_gain: self.track.elevation_gain(&range),
            distance_start: self.track.distance(range.start),
            distance_end: self.track.distance(range.end - 1),
        }
    }
}

pub fn worker(filename: &str) {
    println!("read gpx");
    let mut gpx = gpsdata::read_gpx(filename);
    let segment = gpsdata::read_segment(&mut gpx);
    println!("make track");
    let track = gpsdata::Track::from_segment(&segment);
    println!("make waypoints");
    let mut waypoints = gpsdata::read_waypoints(&gpx);
    println!("add automatic waypoints");
    let indexes = track.interesting_indexes(70.0f32);
    for idx in indexes {
        let wgs = track.wgs84[idx].clone();
        let utm = track.utm[idx].clone();
        waypoints.push(gpsdata::Waypoint::from_track(wgs, utm, idx));
    }
    let indexes = project::nearest_neighboor(&track.utm, &waypoints);
    debug_assert_eq!(waypoints.len(), indexes.len());
    for k in 0..indexes.len() {
        waypoints[k].track_index = indexes[k];
    }
    // sort
    waypoints.sort_by(|w1, w2| w1.track_index.cmp(&w2.track_index));
    for k in 1..waypoints.len() {
        let k1 = waypoints[k].track_index;
        let k0 = waypoints[k - 1].track_index;
        debug_assert!(k1 >= k0);
    }
    println!("render");
    let typfile = render::compile(&track, &waypoints);
    println!("make pdf");
    let pdffile = typfile.replace(".typ", ".pdf");
    pdf::run(typfile.as_str(), pdffile.as_str());
}
