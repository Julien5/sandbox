use svg::Node;

use crate::label_candidates;
use crate::label_candidates::Candidate;
use crate::label_candidates::Candidates;
use crate::label_candidates::LabelBoundingBox;
use crate::label_graph::Graph;

use std::collections::HashMap;
pub type Attributes = HashMap<String, svg::node::Value>;

pub fn set_attr(attr: &mut Attributes, k: &str, v: &str) {
    attr.insert(String::from_str(k).unwrap(), svg::node::Value::from(v));
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
use std::str::FromStr;

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
    pub fn _from_attributes(a: &Attributes) -> Circle {
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
    pub fn _from_attributes(a: &Attributes, text: &str) -> Label {
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
    pub fn _from_attributes(a: &Attributes) -> Polyline {
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

fn bbox_at(distance: f64, angle_index: i32, point: &PointFeature) -> Vec<LabelBoundingBox> {
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

fn generate_bboxes(point: &PointFeature, dtarget_max: f64) -> Vec<LabelBoundingBox> {
    let mut ret = Vec::new();
    let dtarget_start = 2i32;
    let mut dtarget_min = f64::MAX;
    let angle_indices = [0, 12, 25, 38, 50, 62, 75, 90];
    for n in (5..100).step_by(10) {
        for a in angle_indices {
            let dtarget = (n as f64 / 100f64) * dtarget_max;
            if dtarget < dtarget_start as f64 {
                continue;
            }
            if dtarget_min > dtarget {
                dtarget_min = dtarget;
            }
            for c in bbox_at(dtarget, a, point) {
                ret.push(c);
            }
        }
    }
    let dtarget_min = dtarget_min.ceil() as i32;
    for dtarget in dtarget_start..dtarget_min {
        for a in angle_indices {
            for c in bbox_at(dtarget as f64, a, point) {
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

fn candidates_for_point(
    parameters: &parameters::ExperimentalParameters,
    points: &Vec<PointFeature>,
    polyline: &Polyline,
    k: usize,
) -> Candidates {
    if points[k].label.text.is_empty() {
        return Candidates::new();
    }
    let target = &points[k];
    let dtarget_max = match parameters.dtarget_max {
        Some(d) => d,
        _ => 200.0,
    };
    let all = generate_bboxes(target, dtarget_max);
    let mut ret = Candidates::new();
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
        ret.push(Candidate::new(c.clone(), dtarget, dothers));
    }
    return ret;
}

fn build_graph(
    backend: &backend::Backend,
    points: &Vec<PointFeature>,
    polyline: &Polyline,
) -> Graph {
    let mut ret = Graph::new();
    for k in 0..points.len() {
        let candidates = candidates_for_point(&backend.get_eparameters(), points, polyline, k);
        assert!(!ret.candidates.contains_key(&k));
        ret.add_node(k, candidates);
    }
    ret.build_map();
    ret
}

fn candidate_debug_rectangle(candidate: &Candidate) -> svg::node::element::Rectangle {
    let mut debug_bb = svg::node::element::Rectangle::new();
    let mut bb = &candidate.bbox;
    debug_bb = debug_bb.set("x", bb.x_min());
    debug_bb = debug_bb.set("y", bb.y_min());
    debug_bb = debug_bb.set("width", bb.width());
    debug_bb = debug_bb.set("height", bb.height());
    debug_bb = debug_bb.set("fill", "transparent");
    debug_bb = debug_bb.set("stroke-width", "1");
    debug_bb = debug_bb.set("stroke", "green");
    debug_bb
}

pub fn place_labels(
    backend: &backend::Backend,
    points: &mut Vec<PointFeature>,
    polyline: &Polyline,
) -> svg::node::element::Group {
    let G = build_graph(backend, points, polyline);
    debug_assert!(!G.candidates.is_empty());
    let mut debug = svg::node::element::Group::new();
    for k in 0..points.len() {
        let target = k;
        let target_text = &points[k].label.text;
        debug_assert!(G.candidates.contains_key(&target));
        // sort in descending order
        let candidates = G.candidates.get(&target).unwrap();
        let s = label_candidates::select_candidates(candidates);
        for k in s {
            debug.append(candidate_debug_rectangle(&candidates[k]));
        }
        let best_index = G.best(&k);
        match best_index {
            Some(index) => {
                let candidate = &candidates[index];
                let bbox = &candidate.bbox;
                let dothers = &candidate.dothers;
                let dtarget = &candidate.dtarget;
                println!(
                    "[{:12}] c({:.1},{:.1}) d_t={:.1} d_o = {:.1}]",
                    target_text,
                    bbox.x_min(),
                    bbox.y_max(),
                    dtarget,
                    dothers
                );
                points[k].label.bbox = bbox.clone();
            }
            _ => {
                println!("failed to find any candidate for [{}]", target_text);
            }
        }
    }
    debug
}
