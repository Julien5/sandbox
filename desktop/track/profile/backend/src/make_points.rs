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

pub fn profile_points(segment: &Segment, parameters: &Parameters) -> Vec<InputPoint> {
    let mut ret = segment.points.clone();
    ret.retain(|p| {
        let distance = p.distance_to_track();
        match p.kind() {
            InputType::MountainPass | InputType::Peak => {
                return distance < 250f64;
            }
            InputType::Hamlet => {
                return false;
            }
            InputType::GPX => {
                return distance < 250f64;
            }
            InputType::City | InputType::Village => {
                return important(p);
            }
        }
    });
    for w in &mut ret {
        w.label_placement_order = placement_order_profile(&w);
    }
    ret
}
