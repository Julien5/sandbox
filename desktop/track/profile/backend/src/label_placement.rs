enum Anchor {
    Start,
    End,
}

impl Anchor {
    pub fn as_str(&self) -> String {
        match self {
            Anchor::Start => "start".to_string(),
            Anchor::End => "end".to_string(),
        }
    }
    pub fn from_string(s: &str) -> Anchor {
        match s {
            "end" => Anchor::End,
            _ => Anchor::Start,
        }
    }
}

pub trait SvgElement {
    fn from_attributes(a: &Attributes) -> Self
    where
        Self: Sized;
    fn to_attributes(&self) -> Attributes;
}

use std::collections::HashMap;
pub type Attributes = HashMap<String, svg::node::Value>;

pub fn set_attr(attr: &mut Attributes, k: &str, v: &str) {
    attr.insert(String::from_str(k).unwrap(), svg::node::Value::from(v));
}

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

pub struct Label {
    pub id: String,
    x: f64,
    y: f64,
    pub text: String,
    text_anchor: Anchor,
}

impl Label {
    pub fn new() -> Label {
        Label {
            id: String::new(),
            x: 0f64,
            y: 0f64,
            text: String::new(),
            text_anchor: Anchor::Start,
        }
    }

    pub fn bounding_box(&self) -> LabelBoundingBox {
        let width = self.text.len() as f64 * 10.0; // 10 pixels per character
        let height = 16.0; // Assuming a fixed height of 16 pixels for the font size

        let (top_left, bottom_right) = match self.text_anchor {
            Anchor::End => (
                (self.x - width, self.y - height), // Adjust for right alignment
                (self.x, self.y),
            ),
            _ => ((self.x, self.y - height), (self.x + width, self.y)),
        };

        let eps = match self.text_anchor {
            Anchor::End => (2f64, 2f64),
            _ => (-2f64, 2f64),
        };

        LabelBoundingBox::new(offset(&top_left, eps), offset(&bottom_right, eps))
    }
}

pub struct LabelBoundingBox {
    top_left: (f64, f64),
    bottom_right: (f64, f64),
}

fn offset(p: &(f64, f64), d: (f64, f64)) -> (f64, f64) {
    (p.0 + d.0, p.1 + d.1)
}

impl LabelBoundingBox {
    fn new(top_left: (f64, f64), bottom_right: (f64, f64)) -> Self {
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

    pub fn width(&self) -> f64 {
        self.x_max() - self.x_min()
    }

    pub fn height(&self) -> f64 {
        self.y_max() - self.y_min()
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

pub struct Point {
    pub id: String,
    pub circle: Circle,
    pub label: Label,
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

impl SvgElement for Circle {
    fn from_attributes(a: &Attributes) -> Circle {
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

    fn to_attributes(&self) -> Attributes {
        let mut ret = Attributes::new();
        set_attr(&mut ret, "id", self.id.as_str());
        set_attr(&mut ret, "cx", format!("{}", self.cx).as_str());
        set_attr(&mut ret, "cy", format!("{}", self.cy).as_str());
        set_attr(&mut ret, "r", format!("{}", self.r).as_str());
        ret
    }
}

impl SvgElement for Label {
    fn from_attributes(a: &Attributes) -> Label {
        let anchor = match a.get("text-anchor") {
            Some(string) => Anchor::from_string(string),
            _ => Anchor::Start,
        };
        Label {
            id: a.get("id").unwrap().to_string(),
            x: a.get("x").unwrap().to_string().parse::<f64>().unwrap(),
            y: a.get("y").unwrap().to_string().parse::<f64>().unwrap(),
            text: String::new(),
            text_anchor: anchor,
        }
    }

    fn to_attributes(&self) -> Attributes {
        let mut ret = Attributes::new();
        set_attr(&mut ret, "id", self.id.as_str());
        set_attr(&mut ret, "text-anchor", self.text_anchor.as_str().as_str());
        set_attr(&mut ret, "font-size", "16");
        set_attr(&mut ret, "x", format!("{}", self.x).as_str());
        set_attr(&mut ret, "y", format!("{}", self.y).as_str());
        ret
    }
}

impl SvgElement for Polyline {
    fn from_attributes(a: &Attributes) -> Polyline {
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

    fn to_attributes(&self) -> Attributes {
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

fn polyline_hits_label(polyline: &Polyline, label: &Label) -> bool {
    let bbox = label.bounding_box();

    for &(x, y) in &polyline.points {
        if x >= bbox.x_min() && x <= bbox.x_max() && y >= bbox.y_min() && y <= bbox.y_max() {
            return true;
        }
    }

    false
}

fn offset_at(r: f64, angle: f64) -> (f64, f64) {
    let x = r * angle.cos();
    let y = r * angle.sin();
    (x, y)
}

fn candidates(
    distance: f64,
    angle_index: i32,
    (width, height): (f64, f64),
) -> Vec<(f64, f64, Anchor)> {
    let mut ret = Vec::new();
    let steps = 5;

    let height_step = height / (steps as f64);
    let width_step = width / (steps as f64);
    match angle_index {
        0 => {
            for i in 0..steps {
                let dy = i as f64 * height_step;
                ret.push((distance, dy, Anchor::Start));
            }
            return ret;
        }
        25 => {
            for i in 0..steps {
                let dx = i as f64 * width_step;
                ret.push((-dx, -distance, Anchor::Start));
            }
            return ret;
        }
        50 => {
            for i in 0..steps {
                let dy = i as f64 * height_step;
                ret.push((-distance, dy, Anchor::End));
            }
            return ret;
        }
        75 => {
            for i in 0..steps {
                let dx = i as f64 * width_step;
                ret.push((dx, distance, Anchor::End));
            }
            return ret;
        }
        _ => {}
    }

    let angle = (angle_index as f64) * 2f64 * std::f64::consts::PI;
    let (epsx, mut epsy) = offset_at(distance, angle);
    let anchor = if epsx < 0f64 {
        Anchor::End
    } else {
        Anchor::Start
    };
    if epsy > 0f64 {
        epsy += height;
    }
    ret.push((epsx, epsy, anchor));
    ret
}

struct Candidate {
    x: f64,
    y: f64,
    anchor: Anchor,
}

pub fn place_label(point: &mut Point, polyline: &Polyline) {
    let label = &mut point.label;
    let bb = label.bounding_box();
    let (width, height) = (bb.width(), bb.height());
    for n in 5..10 {
        for a in (0..100).step_by(25) {
            for c in candidates(n as f64, a, (width, height)) {
                let (dx, dy, anchor) = c;
                label.x = point.circle.cx + dx;
                label.y = point.circle.cy + dy;
                label.text_anchor = anchor;
                if !polyline_hits_label(polyline, label) {
                    println!(
                        "[{:4}][n={n}][a={a:2}] => [d=({dx:.1},{dy:.1})][{}]",
                        label.text,
                        label.text_anchor.as_str(),
                    );
                    return;
                }
            }
        }
    }
    println!("[{}] FAIL", label.text);
}

pub fn place_labels(points: &mut Vec<Point>, polyline: &Polyline) {
    for p in points {
        place_label(p, polyline);
    }
}
