use std::collections::HashMap;

use crate::{
    backend::Segment,
    inputpoint::{InputPoint, InputType, TrackProjection},
    parameters::Parameters,
    track::Track,
};

fn placement_order_profile(point: &InputPoint) -> usize {
    let delta = point.distance_to_track();
    let kind = point.kind();
    let population = match point.population() {
        Some(p) => p,
        None => 0,
    };
    let mut ret = 1;
    if kind == InputType::GPX && delta < 1000f64 {
        return ret;
    }
    ret += 1;
    if kind == InputType::City && delta < 1000f64 {
        return ret;
    }
    ret += 1;
    if kind == InputType::Village && delta < 1000f64 && population > 1000 {
        return ret;
    }
    ret += 1;
    if (kind == InputType::MountainPass || kind == InputType::Peak) && delta < 500f64 {
        return ret;
    }
    ret += 1;
    if kind == InputType::Village && delta < 200f64 {
        return ret;
    }
    ret += 10;
    ret
}

fn important(p: &InputPoint) -> bool {
    let pop = match p.population() {
        Some(n) => n,
        None => {
            if p.kind() == InputType::City {
                1000
            } else {
                0
            }
        }
    };
    let dist = p.distance_to_track();
    if pop > 100000 && dist < 5000f64 {
        return true;
    }
    if pop > 10000 && dist < 1000f64 {
        return true;
    }
    if pop >= 500 && dist < 500f64 {
        return true;
    }
    /*if dist < 2000f64 {
        log::trace!(
            "too far for the profile:{:?} {:?} {:?} d={:.1}",
            p.kind(),
            p.population(),
            p.name(),
            dist
        );
    }*/
    false
}

type Interval = std::ops::Range<usize>;
type Points = Vec<InputPoint>;

fn contains(interval: &Interval, point: &InputPoint) -> bool {
    let index = point.track_projection.as_ref().unwrap().track_index;
    interval.start <= index && index < interval.end
}

fn tight(interval: &mut Interval, track: &Track) {
    let dstart = track.distance(interval.start);
    let dend = track.distance(interval.end - 1);
    let margin = (dend - dstart) / 8f64;
    interval.start = track.index_after(dstart + margin);
    interval.end = track.index_before(dend - margin);
}

fn largest_interval(segment: &Segment, points: &Points) -> Interval {
    let mut indices: Vec<_> = points
        .iter()
        .map(|p| p.track_projection.as_ref().unwrap().track_index)
        .collect();
    indices.sort();
    let mut prev = 0usize;
    let mut intervals = Vec::new();
    for i in indices {
        intervals.push(Interval {
            start: prev,
            end: i,
        });
        prev = i;
    }
    intervals.push(Interval {
        start: prev,
        end: segment.range.end,
    });
    intervals.sort_by_key(|i| i.len());
    intervals.last().unwrap().clone()
}

pub fn profile_points(segment: &Segment, parameters: &Parameters) -> Vec<InputPoint> {
    let mut ret = Vec::new();
    let mut candidates: HashMap<usize, InputPoint> = segment
        .points
        .iter()
        .enumerate()
        .map(|(i, p)| (i, p.clone()))
        .collect();
    while ret.len() != parameters.profile_options.npoints as usize {
        let mut interval = largest_interval(segment, &ret);
        tight(&mut interval, &segment.track);
        let d0 = segment.track.distance(interval.start);
        let d1 = segment.track.distance(interval.end - 1);
        log::trace!("{:.1} {:.1}", d0 / 1000f64, d1 / 1000f64,);
        // keep points in the interval
        assert!(!candidates.is_empty());
        let mut inner = candidates.clone();
        inner.retain(|index, p| contains(&interval, p));
        let mut selected = if inner.is_empty() {
            let dmid = 0.5 * (d0 + d1);
            let indx = segment.track.index_after(dmid);
            let wgs = &segment.track.wgs84[indx];
            let euc = &segment.track.euclidian[indx];
            let mut p = InputPoint::from_wgs84(&wgs, &euc);
            p.track_projection = Some(TrackProjection {
                track_floating_index: indx as f64,
                track_index: indx,
                track_distance: 0f64,
                elevation: wgs.z(),
                euclidean: euc.clone(),
            });
            p.tags
                .insert("name".to_string(), format!("Point-{}", ret.len()));
            p
        } else {
            let mut innerv: Vec<_> = inner.iter().map(|(index, p)| (index, p)).collect();
            innerv.sort_by_key(|(index, p)| placement_order_profile(p));
            let first = innerv.first().unwrap();
            let index = first.0;
            let point = first.1;
            candidates.remove(index);
            point.clone()
        };
        selected.label_placement_order = placement_order_profile(&selected);
        log::trace!("=> selected {:?}", selected.name());
        ret.push(selected.clone());
    }
    ret
}
