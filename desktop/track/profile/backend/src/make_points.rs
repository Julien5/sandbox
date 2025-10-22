use crate::{
    backend::Segment,
    inputpoint::{InputPoint, InputType},
    parameters::Parameters,
};

fn placement_order_profile(point: &InputPoint) -> i32 {
    let delta = point.distance_to_track();
    let kind = point.kind();
    let mut ret = 1;
    if kind == InputType::City && delta < 1000f64 {
        return ret;
    }
    if (kind == InputType::MountainPass || kind == InputType::Peak) && delta < 500f64 {
        return ret;
    }
    ret += 1;
    if kind == InputType::Village && delta < 1000f64 {
        return ret;
    }
    ret += 1;
    if kind == InputType::City && delta < 10000f64 {
        return ret;
    }
    ret += 1;
    if kind == InputType::Village && delta < 200f64 {
        return ret;
    }
    ret += 1;
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
    intervals.first().unwrap().clone()
}

pub fn profile_points(segment: &Segment, parameters: &Parameters) -> Vec<InputPoint> {
    let mut ret = Vec::new();
    let mut candidates = segment.points.clone();
    while ret.len() != parameters.profile_options.npoints {
        let interval = largest_interval(segment, &ret);
        // keep points in the interval
        assert!(!candidates.is_empty());
        let mut inner: Vec<_> = candidates
            .iter()
            .filter(|p| contains(&interval, p))
            .collect();
        inner.sort_by_key(|p| placement_order_profile(p));
        if inner.is_empty() {
            break;
        }
        let mut selected = inner.first().unwrap().to_owned().clone();
        selected.label_placement_order = 1 + ret.len();
        // TODO: implement == for InputPoint
        let index = candidates
            .iter()
            .position(|p| p.wgs84 == selected.wgs84)
            .unwrap();
        candidates.remove(index);
        log::trace!("point:{:?}", selected);
        ret.push(selected.clone());
    }
    ret
}
