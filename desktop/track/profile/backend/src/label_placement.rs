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
struct Candidate {
    bbox: LabelBoundingBox,
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
        let width = 10f64 * s.len() as f64;
        self.bbox = LabelBoundingBox::new_blwh((0f64, 0f64), width, 16f64);
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
    pub fn distance_to_point(&self, p: (f64, f64)) -> f64 {
        let mut candidates = Vec::new();

        candidates.push((self.x_min(), self.y_min()));
        candidates.push((self.x_min(), self.y_max()));
        candidates.push((self.x_max(), self.y_min()));
        candidates.push((self.x_max(), self.y_max()));

        candidates.push((self.x_mid(), self.y_max()));
        candidates.push((self.x_max(), self.y_mid()));

        candidates.push((self.x_mid(), self.y_min()));
        candidates.push((self.x_min(), self.y_mid()));

        let mut ret = f64::MAX;
        for c in candidates {
            let d = distance(c, p);
            if d < ret {
                ret = d;
            }
        }
        ret
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
pub struct Point {
    pub id: String,
    pub circle: Circle,
    pub label: Label,
}

impl PartialEq for Point {
    fn eq(&self, other: &Self) -> bool {
        self.id == other.id
    }
}

impl Eq for Point {}
use std::hash::Hash;
use std::hash::Hasher;
impl Hash for Point {
    fn hash<H: Hasher>(&self, state: &mut H) {
        self.id.hash(state);
    }
}

impl Point {
    pub fn new() -> Point {
        Point {
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
        let height = 16f64;
        let width = 10f64 * text.len() as f64;
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
        let mut x = self.bbox.top_left.0;
        if self.bounding_box().x_max() < cx {
            set_attr(&mut ret, "text-anchor", "end");
            x = self.bbox.bottom_right.0;
        } else {
            set_attr(&mut ret, "text-anchor", "start");
        }
        let y = self.bbox.bottom_right.1;
        set_attr(&mut ret, "id", self.id.as_str());
        set_attr(&mut ret, "font-size", "16");
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
        if x >= bbox.x_min() && x <= bbox.x_max() && y >= bbox.y_min() && y <= bbox.y_max() {
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

impl Candidate {
    fn _new_tlbr(top_left: (f64, f64), bottom_right: (f64, f64)) -> Self {
        Candidate {
            bbox: LabelBoundingBox::new_tlbr(top_left, bottom_right),
        }
    }
    fn new_blwh(bottom_left: (f64, f64), width: f64, height: f64) -> Self {
        Candidate {
            bbox: LabelBoundingBox::new_blwh(bottom_left, width, height),
        }
    }
    fn new_brwh(bottom_right: (f64, f64), width: f64, height: f64) -> Self {
        Candidate {
            bbox: LabelBoundingBox::new_brwh(bottom_right, width, height),
        }
    }
    fn new_tlwh(top_left: (f64, f64), width: f64, height: f64) -> Self {
        Candidate {
            bbox: LabelBoundingBox::new_tlwh(top_left, width, height),
        }
    }
    fn new_trwh(top_right: (f64, f64), width: f64, height: f64) -> Self {
        Candidate {
            bbox: LabelBoundingBox::new_trwh(top_right, width, height),
        }
    }
}

fn candidates_at(distance: f64, angle_index: i32, point: &Point) -> Vec<Candidate> {
    let mut ret = Vec::new();
    let steps = 5;
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
                let c = Candidate::new_blwh((cx + distance, cy + dy), width, height);
                ret.push(c);
            }
            return ret;
        }
        25 => {
            for i in 0..steps {
                let dx = i as f64 * width_step;
                let c = Candidate::new_blwh((cx - dx, cy - distance), width, height);
                ret.push(c);
            }
            return ret;
        }
        50 => {
            for i in 0..steps {
                let dy = i as f64 * height_step;
                let c = Candidate::new_brwh((cx - distance, cy + dy), width, height);
                ret.push(c);
            }
            return ret;
        }
        75 => {
            for i in 0..steps {
                let dx = i as f64 * width_step;
                let c = Candidate::new_trwh((cx + dx, cy + distance), width, height);
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
        (true, false) => Candidate::new_blwh(p, width, height),
        (false, false) => Candidate::new_brwh(p, width, height),
        (false, true) => Candidate::new_trwh(p, width, height),
        (true, true) => Candidate::new_tlwh(p, width, height),
    };
    ret.push(c);
    ret
}

fn generate_candidates(point: &Point) -> Vec<Candidate> {
    let mut ret = Vec::new();
    for n in 5..10 {
        for a in (0..100).step_by(25) {
            for c in candidates_at(n as f64, a, point) {
                ret.push(c);
            }
        }
    }
    ret
}

fn _candidates(points: &Vec<Point>) -> HashMap<Point, Vec<Candidate>> {
    let mut ret = HashMap::new();
    for p in points {
        ret.insert(p.clone(), generate_candidates(p));
    }
    ret
}

fn distance_to_others(candidate: &Candidate, points: &Vec<Point>, k: usize) -> f64 {
    let mut ret = f64::MAX;
    for l in 0..points.len() {
        if l == k {
            continue;
        }
        let other = &points[l];
        let d = candidate
            .bbox
            .distance_to_point((other.circle.cx, other.circle.cy));
        if d < ret {
            ret = d;
        }
    }
    ret
}

fn place_label(points: &mut Vec<Point>, polyline: &Polyline, k: usize) {
    // find one that is close to p and away from other points
    let target = &mut points[k];
    if target.label.text.is_empty() {
        return;
    }
    let candidates = generate_candidates(target);
    for k in 0..candidates.len() {
        let c = &candidates[k];
        if !polyline_hits_bbox(polyline, &c.bbox) {
            println!("[{:4}][{k:3}] => [d=({})]", target.label.text, c.bbox);
            target.label.bbox = c.bbox.clone();
            break;
        }
        /*
        let _dtarget = c
            .bbox
            .distance_to_point((target.circle.cx, target.circle.cy));
        let _dothers = distance_to_others(c, points, k);
        */
        // c close to target ?
        // c far rom others ?
    }
}

pub fn place_labels(points: &mut Vec<Point>, polyline: &Polyline) {
    for k in 0..points.len() {
        place_label(points, polyline, k);
    }
}
