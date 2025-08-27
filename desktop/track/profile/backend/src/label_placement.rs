use std::collections::HashMap;
pub type Attributes = HashMap<String, svg::node::Value>;

pub fn set_attr(attr: &mut Attributes, k: &str, v: &str) {
    attr.insert(String::from_str(k).unwrap(), svg::node::Value::from(v));
}

fn distance((x1, y1): (f64, f64), (x2, y2): (f64, f64)) -> f64 {
    let dx = x2 - x1;
    let dy = y2 - y1;
    (dx * dx + dy * dy).sqrt()
}

const FONTSIZE: f64 = 16f64;

fn check(ret: &mut f64, s: &str, c: &char, w: &f64) -> bool {
    if s.find(*c).is_some() {
        *ret += w;
        return true;
    }
    false
}

// from https://stackoverflow.com/questions/16007743/roughly-approximate-the-width-of-a-string-of-text-in-python
fn char_width(s: &char) -> f64 {
    let mut size = 0f64;
    if check(&mut size, "lij|\' '", s, &37f64) {
    } else if check(&mut size, "![]fI.,:;/\\t", s, &50f64) {
    } else if check(&mut size, "`-(){}r\"", s, &60f64) {
    } else if check(&mut size, "*^zcsJkvxy", s, &85f64) {
    } else if check(&mut size, "aebdhnopqug#$L+<>=?_~FZT", s, &95f64) {
    } else if check(&mut size, "0123456789", s, &95f64) {
    } else if check(&mut size, "BSPEAKVXY&UwNRCHD", s, &112f64) {
    } else if check(&mut size, "QGOMm%W@", s, &135f64) {
    }
    let ret = size * 6f64 / 1000.0;
    (ret / 0.57f64) * (FONTSIZE / 16f64) * 9f64
}

fn text_width(s: &str) -> f64 {
    let mut ret = 0f64;
    for c in s.chars() {
        ret += char_width(&c);
    }
    return ret;
}

#[derive(Clone)]
pub struct Circle {
    pub id: String,
    pub cx: f64,
    pub cy: f64,
    pub r: f64,
    pub fill: Option<String>,
}

impl Circle {
    pub fn new() -> Circle {
        Circle {
            id: String::new(),
            cx: 0f64,
            cy: 0f64,
            r: 4f64,
            fill: None,
        }
    }
}

#[derive(Clone)]
pub struct Label {
    pub id: String,
    pub bbox: LabelBoundingBox,
    pub text: String,
}

impl Label {
    pub fn new() -> Label {
        Label {
            id: String::new(),
            bbox: LabelBoundingBox::zero(),
            text: String::new(),
        }
    }

    pub fn set_text(&mut self, s: &str) {
        self.text = String::from_str(s).unwrap();
        let width = text_width(s);
        self.bbox = LabelBoundingBox::new_blwh((0f64, 0f64), width, FONTSIZE);
    }

    pub fn bounding_box(&self) -> LabelBoundingBox {
        self.bbox.clone()
    }
}

#[derive(Clone)]
pub struct LabelBoundingBox {
    top_left: (f64, f64),
    bottom_right: (f64, f64),
}

impl LabelBoundingBox {
    fn zero() -> Self {
        LabelBoundingBox {
            top_left: (0f64, 0f64),
            bottom_right: (0f64, 0f64),
        }
    }

    fn new_tlbr(top_left: (f64, f64), bottom_right: (f64, f64)) -> Self {
        LabelBoundingBox {
            top_left,
            bottom_right,
        }
    }

    fn new_blwh(bottom_left: (f64, f64), width: f64, height: f64) -> Self {
        let top_left = (bottom_left.0, bottom_left.1 - height);
        let bottom_right = (bottom_left.0 + width, bottom_left.1);
        LabelBoundingBox {
            top_left,
            bottom_right,
        }
    }

    fn new_brwh(bottom_right: (f64, f64), width: f64, height: f64) -> Self {
        let top_left = (bottom_right.0 - width, bottom_right.1 - height);
        LabelBoundingBox {
            top_left,
            bottom_right,
        }
    }

    fn new_tlwh(top_left: (f64, f64), width: f64, height: f64) -> Self {
        let bottom_right = (top_left.0 + width, top_left.1 + height);
        LabelBoundingBox {
            top_left,
            bottom_right,
        }
    }

    fn new_trwh(top_right: (f64, f64), width: f64, height: f64) -> Self {
        let top_left = (top_right.0 - width, top_right.1);
        let bottom_right = (top_right.0, top_right.1 + height);
        LabelBoundingBox {
            top_left,
            bottom_right,
        }
    }

    pub fn x_min(&self) -> f64 {
        self.top_left.0
    }

    pub fn y_min(&self) -> f64 {
        self.top_left.1
    }

    pub fn bottom_left(&self) -> (f64, f64) {
        (self.x_min(), self.y_max())
    }

    pub fn top_right(&self) -> (f64, f64) {
        (self.x_max(), self.y_min())
    }

    pub fn x_max(&self) -> f64 {
        self.bottom_right.0
    }

    pub fn y_max(&self) -> f64 {
        self.bottom_right.1
    }
    pub fn y_mid(&self) -> f64 {
        0.5 * (self.y_min() + self.y_max())
    }
    pub fn x_mid(&self) -> f64 {
        0.5 * (self.x_min() + self.x_max())
    }

    pub fn width(&self) -> f64 {
        self.x_max() - self.x_min()
    }

    pub fn height(&self) -> f64 {
        self.y_max() - self.y_min()
    }
    pub fn project_on_border(&self, q: (f64, f64)) -> (f64, f64) {
        let (qx, qy) = q;

        // Calculate distances to each edge
        let left = self.x_min();
        let right = self.x_max();
        let top = self.y_min();
        let bottom = self.y_max();

        let dist_left = (qx - left).abs();
        let dist_right = (qx - right).abs();
        let dist_top = (qy - top).abs();
        let dist_bottom = (qy - bottom).abs();

        // Find the closest edge
        let min_dist = dist_left.min(dist_right).min(dist_top).min(dist_bottom);

        if min_dist == dist_left {
            (left, qy.clamp(top, bottom)) // Project onto the left edge
        } else if min_dist == dist_right {
            (right, qy.clamp(top, bottom)) // Project onto the right edge
        } else if min_dist == dist_top {
            (qx.clamp(left, right), top) // Project onto the top edge
        } else {
            (qx.clamp(left, right), bottom) // Project onto the bottom edge
        }
    }
    pub fn distance(&self, q: (f64, f64)) -> f64 {
        let p = self.project_on_border(q);
        let dx = p.0 - q.0;
        let dy = p.1 - q.1;
        (dx * dx + dy * dy).sqrt()
    }
    fn contains(&self, (x, y): (f64, f64)) -> bool {
        if x >= self.x_min() && x <= self.x_max() && y >= self.y_min() && y <= self.y_max() {
            return true;
        }
        false
    }
    fn intersect_self(&self, other: &Self) -> bool {
        for p in [
            self.top_left,
            self.bottom_right,
            self.bottom_left(),
            self.top_right(),
        ] {
            if other.contains(p) {
                return true;
            }
        }
        false
    }
    fn insersect(&self, other: &Self) -> bool {
        if other.intersect_self(self) || self.intersect_self(other) {
            return true;
        }
        false
    }
}

use std::fmt;
use std::str::FromStr;
impl fmt::Display for LabelBoundingBox {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(
            f,
            "LabelBoundingBox {{ top_left: ({:.0}, {:.0}), bottom_right: ({:.0}, {:.0}) }}",
            self.top_left.0, self.top_left.1, self.bottom_right.0, self.bottom_right.1
        )
    }
}

#[derive(Clone)]
pub struct PointFeature {
    pub id: String,
    pub circle: Circle,
    pub label: Label,
}

impl PartialEq for PointFeature {
    fn eq(&self, other: &Self) -> bool {
        self.id == other.id
    }
}

impl Eq for PointFeature {}
use std::hash::Hash;
use std::hash::Hasher;

use crate::backend;
use crate::parameters;
impl Hash for PointFeature {
    fn hash<H: Hasher>(&self, state: &mut H) {
        self.id.hash(state);
    }
}

impl PointFeature {
    pub fn new() -> PointFeature {
        PointFeature {
            id: String::new(),
            circle: Circle::new(),
            label: Label::new(),
        }
    }
}

pub struct Polyline {
    id: String,
    pub points: Vec<(f64, f64)>,
}

impl Polyline {
    pub fn new() -> Polyline {
        Polyline {
            id: "track".to_string(),
            points: Vec::<(f64, f64)>::new(),
        }
    }
}

impl Circle {
    pub fn from_attributes(a: &Attributes) -> Circle {
        let fill = match a.get("fill") {
            Some(value) => Some(value.to_string()),
            _ => None,
        };

        Circle {
            id: a.get("id").unwrap().to_string(),
            cx: a.get("cx").unwrap().to_string().parse::<f64>().unwrap(),
            cy: a.get("cy").unwrap().to_string().parse::<f64>().unwrap(),
            r: a.get("r").unwrap().to_string().parse::<f64>().unwrap(),
            fill,
        }
    }

    pub fn to_attributes(&self) -> Attributes {
        let mut ret = Attributes::new();
        set_attr(&mut ret, "id", self.id.as_str());
        set_attr(&mut ret, "cx", format!("{}", self.cx).as_str());
        set_attr(&mut ret, "cy", format!("{}", self.cy).as_str());
        set_attr(&mut ret, "r", format!("{}", self.r).as_str());
        ret
    }
}

impl Label {
    pub fn from_attributes(a: &Attributes, text: &str) -> Label {
        let anchor = match a.get("text-anchor") {
            Some(data) => data,
            _ => "start",
        };
        let x = a.get("x").unwrap().to_string().parse::<f64>().unwrap();
        let y = a.get("y").unwrap().to_string().parse::<f64>().unwrap();
        let height = FONTSIZE;
        let width = text_width(text);
        let (top_left, bottom_right) = if anchor == "start" {
            ((x, y - height), (x + width, y))
        } else {
            ((x - width, y - height), (x, y))
        };
        let bbox = LabelBoundingBox::new_tlbr(top_left, bottom_right);
        Label {
            id: a.get("id").unwrap().to_string(),
            bbox,
            text: String::from_str(text).unwrap(),
        }
    }

    pub fn to_attributes(&self, cx: f64) -> Attributes {
        let mut ret = Attributes::new();
        let mut x = self.bbox.top_left.0 + 2f64;
        let anchor = if self.bounding_box().x_max() < cx {
            "end"
        } else {
            "start"
        };
        if anchor == "end" {
            x = self.bbox.bottom_right.0 - 2f64;
        }
        set_attr(&mut ret, "text-anchor", anchor);
        let y = self.bbox.bottom_right.1 - 2f64;
        set_attr(&mut ret, "id", self.id.as_str());
        set_attr(&mut ret, "font-size", format!("{:.1}", FONTSIZE).as_str());
        set_attr(&mut ret, "x", format!("{}", x).as_str());
        set_attr(&mut ret, "y", format!("{}", y).as_str());
        ret
    }
}

impl Polyline {
    pub fn from_attributes(a: &Attributes) -> Polyline {
        let data = a.get("d").unwrap();
        let mut points = Vec::new();
        for tok in data.split(" ") {
            let t: Vec<&str> = tok.split(",").collect();
            debug_assert!(t.len() == 2);
            let x = format!("{}", t[0].get(1..).unwrap())
                .parse::<f64>()
                .unwrap();
            let y = format!("{}", t[1]).parse::<f64>().unwrap();
            points.push((x, y));
        }
        Polyline {
            id: format!("{}", a.get("id").unwrap()),
            points,
        }
    }

    pub fn to_attributes(&self) -> Attributes {
        let mut ret = Attributes::new();
        let mut dv = Vec::new();
        for (x, y) in &self.points {
            if dv.is_empty() {
                dv.push(format!("M{x:.1},{y:.1}"));
            } else {
                dv.push(format!("L{x:.1},{y:.1}"));
            }
        }
        let d = dv.join(" ");
        set_attr(&mut ret, "id", self.id.as_str());
        set_attr(&mut ret, "fill", "transparent");
        set_attr(&mut ret, "stroke-width", "2");
        set_attr(&mut ret, "stroke", "black");
        set_attr(&mut ret, "d", d.as_str());
        ret
    }
}

fn polyline_hits_bbox(polyline: &Polyline, bbox: &LabelBoundingBox) -> bool {
    for &(x, y) in &polyline.points {
        if bbox.contains((x, y)) {
            return true;
        }
    }

    false
}

fn cartesian(r: f64, angle: f64) -> (f64, f64) {
    let x = r * angle.cos();
    let y = r * angle.sin();
    (x, y)
}

fn candidates_bbox_at(
    distance: f64,
    angle_index: i32,
    point: &PointFeature,
) -> Vec<LabelBoundingBox> {
    let mut ret = Vec::new();
    let steps = 10;
    let bbox = point.label.bounding_box();
    let width = bbox.width();
    let height = bbox.height();
    let height_step = height / (steps as f64);
    let width_step = width / (steps as f64);
    let cx = point.circle.cx;
    let cy = point.circle.cy;
    match angle_index {
        0 => {
            for i in 0..steps {
                let dy = i as f64 * height_step;
                let c = LabelBoundingBox::new_blwh((cx + distance, cy + dy), width, height);
                ret.push(c);
            }
            return ret;
        }
        25 => {
            for i in 0..steps {
                let dx = i as f64 * width_step;
                let c = LabelBoundingBox::new_blwh((cx - dx, cy - distance), width, height);
                ret.push(c);
            }
            return ret;
        }
        50 => {
            for i in 0..steps {
                let dy = i as f64 * height_step;
                let c = LabelBoundingBox::new_brwh((cx - distance, cy + dy), width, height);
                ret.push(c);
            }
            return ret;
        }
        75 => {
            for i in 0..steps {
                let dx = i as f64 * width_step;
                let c = LabelBoundingBox::new_trwh((cx + dx, cy + distance), width, height);
                ret.push(c);
            }
            return ret;
        }
        _ => {}
    }

    let angle = (angle_index as f64) * 2f64 * std::f64::consts::PI;
    let (epsx, epsy) = cartesian(distance, angle);
    let p = (cx + epsx, cy + epsy);
    let c = match (epsx > 0f64, epsy > 0f64) {
        (true, false) => LabelBoundingBox::new_blwh(p, width, height),
        (false, false) => LabelBoundingBox::new_brwh(p, width, height),
        (false, true) => LabelBoundingBox::new_trwh(p, width, height),
        (true, true) => LabelBoundingBox::new_tlwh(p, width, height),
    };
    ret.push(c);
    ret
}

fn generate_bbox(point: &PointFeature, dtarget_max: f64) -> Vec<LabelBoundingBox> {
    let mut ret = Vec::new();
    let mut dtarget_min = f64::MAX;
    let angle_indices = [0, 12, 25, 38, 50, 62, 75, 90];
    for n in (5..100).rev().step_by(10) {
        for a in angle_indices {
            let dtarget = (n as f64 / 100f64) * dtarget_max;
            if dtarget_min > dtarget {
                dtarget_min = dtarget;
            }
            for c in candidates_bbox_at(dtarget, a, point) {
                ret.push(c);
            }
        }
    }
    let dtarget_min = dtarget_min.ceil() as i32;
    for dtarget in (2..dtarget_min).rev() {
        for a in angle_indices {
            for c in candidates_bbox_at(dtarget as f64, a, point) {
                // println!("{dtarget} {a} {})", point.id);
                ret.push(c);
            }
        }
    }
    ret
}

fn distance_to_others(
    bbox: &LabelBoundingBox,
    points: &Vec<PointFeature>,
    target_id: &String,
) -> (f64, usize) {
    let mut ret = (f64::MAX, 0);
    for l in 0..points.len() {
        let other = &points[l];
        if other.id == *target_id {
            continue;
        }
        let d = bbox.distance((other.circle.cx, other.circle.cy));
        if d < ret.0 {
            ret = (d, l);
        }
    }
    ret
}

#[derive(Clone)]
struct Candidate {
    bbox: LabelBoundingBox,
}

impl Candidate {
    fn new(bbox: LabelBoundingBox) -> Candidate {
        Candidate { bbox }
    }
    fn intersect(&self, other: &Self) -> bool {
        self.bbox.insersect(&other.bbox)
    }
}

type Candidates = Vec<Candidate>;
type CandidateMap = HashMap<PointFeature, Candidates>;

fn candidates_for_point(
    parameters: &parameters::ExperimentalParameters,
    points: &Vec<PointFeature>,
    polyline: &Polyline,
    k: usize,
) -> Candidates {
    if points[k].label.text.is_empty() {
        return Vec::new();
    }
    let target = &points[k];
    let dtarget_max = match parameters.dtarget_max {
        Some(d) => d,
        _ => 200.0,
    };
    let all = generate_bbox(target, dtarget_max);
    let mut ret = Vec::new();
    for index in 0..all.len() {
        let c = &all[index];
        if polyline_hits_bbox(polyline, &c) {
            continue;
        }
        let dtarget = c.distance((target.circle.cx, target.circle.cy));
        let (dothers, _) = distance_to_others(c, &points, &target.id);
        if dothers < dtarget {
            continue;
        }
        ret.push(Candidate::new(c.clone()));
    }
    return ret;
}

fn build_candidate_map(
    backend: &backend::Backend,
    points: &Vec<PointFeature>,
    polyline: &Polyline,
) -> CandidateMap {
    let mut ret = CandidateMap::new();
    for k in 0..points.len() {
        let target = &points[k];
        let candidates = candidates_for_point(&backend.get_eparameters(), points, polyline, k);
        assert!(!ret.contains_key(target));
        ret.insert(target.clone(), candidates);
    }
    ret
}

pub fn place_labels(
    backend: &backend::Backend,
    points: &mut Vec<PointFeature>,
    polyline: &Polyline,
) {
    let map = build_candidate_map(backend, points, polyline);
    debug_assert!(!map.is_empty());
    for k in 0..points.len() {
        let target = &points[k];
        debug_assert!(map.contains_key(target));
        let best = map.get(target).unwrap().last();
        match best {
            Some(candidate) => {
                let bbox = &candidate.bbox;
                let dothers = distance_to_others(bbox, points, &target.id);
                let dtarget = candidate
                    .bbox
                    .distance((target.circle.cx, target.circle.cy));
                println!(
                    "[{:12}] c({:.1},{:.1}) d_t={:.1} d_o = {:.1}]",
                    target.label.text,
                    bbox.x_min(),
                    bbox.y_max(),
                    dtarget,
                    dothers.0
                );
                points[k].label.bbox = bbox.clone();
            }
            _ => {
                println!("failed to find any candidate for [{}]", target.label.text);
            }
        }
    }
}
