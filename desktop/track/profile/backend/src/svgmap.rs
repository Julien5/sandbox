#![allow(non_snake_case)]

use std::collections::HashMap;
use std::str::FromStr;

use crate::utm::UTMPoint;
use crate::{gpsdata, waypoints_table};
use crate::{segment, waypoint};

use svg::node::element::path::{Command, Data, Position};
use svg::Document;

struct BoundingBox {
    min: (f64, f64),
    max: (f64, f64),
}

impl BoundingBox {
    fn new() -> BoundingBox {
        let min = (f64::MAX, f64::MAX);
        let max = (f64::MIN, f64::MIN);
        BoundingBox { min, max }
    }
    fn width(&self) -> f64 {
        return self.max.0 - self.min.0;
    }
    fn height(&self) -> f64 {
        return self.max.1 - self.min.1;
    }
    fn update(&mut self, p: &UTMPoint) {
        if p.x() > self.max.0 {
            self.max.0 = p.x();
        }
        if p.y() > self.max.1 {
            self.max.1 = p.y();
        }
        if p.x() < self.min.0 {
            self.min.0 = p.x();
        }
        if p.y() < self.min.1 {
            self.min.1 = p.y();
        }
    }
    // TODO: take WxH into account
    fn fix_aspect_ratio(&mut self, _W: i32, _H: i32) {
        let X = (self.min.0 + self.max.0) / 2f64;
        let Y = (self.min.1 + self.max.1) / 2f64;
        if self.height() > self.width() {
            let delta = 0.5f64 * (self.height());
            self.max.0 = X + delta;
            self.min.0 = X - delta;
        } else {
            let delta = 0.5f64 * self.width();
            self.max.1 = Y + delta;
            self.min.1 = Y - delta;
        }
        let margin = 2000f64;
        self.max.0 = self.max.0 + margin;
        self.max.1 = self.max.1 + margin;
        self.min.0 = self.min.0 - margin;
        self.min.1 = self.min.1 - margin;
    }
    fn to_graphics_coordinates(&self, p: &UTMPoint, W: i32, H: i32) -> (f64, f64) {
        let xmin = self.min.0;
        let xmax = self.max.0;
        let ymin = self.min.1;
        let ymax = self.max.1;

        let f = |x: f64| -> f64 {
            let a = W as f64 / (xmax - xmin);
            let b = -a * xmin;
            a * x + b
        };
        let g = |y: f64| -> f64 {
            let a = H as f64 / (ymin - ymax);
            let b = -a * ymax;
            a * y + b
        };
        (f(p.x()), g(p.y()))
    }
    fn contains(&self, p: &UTMPoint) -> bool {
        if p.x() < self.min.0 {
            return false;
        }
        if p.x() > self.max.0 {
            return false;
        }
        if p.y() < self.min.1 {
            return false;
        }
        if p.y() > self.max.1 {
            return false;
        }
        return true;
    }
}

type Path = svg::node::element::Path;
type Attributes = HashMap<String, svg::node::Value>;

struct SvgPoint {
    id: String,
    circle: Attributes,
    label: Attributes,
    text: String,
}

impl SvgPoint {
    fn new() -> SvgPoint {
        SvgPoint {
            id: String::new(),
            circle: Attributes::new(),
            label: Attributes::new(),
            text: String::new(),
        }
    }
}

pub struct SvgMap {
    polyline: Attributes,
    points: Vec<SvgPoint>,
    document: Attributes,
}

fn readid(id: &str) -> (&str, &str) {
    id.split_once("/").unwrap()
}

fn set_attr(attr: &mut Attributes, k: &str, v: &str) {
    attr.insert(String::from_str(k).unwrap(), svg::node::Value::from(v));
}

fn set_attr_data(attr: &mut Attributes, k: &str, v: &Data) {
    attr.insert(
        String::from_str(k).unwrap(),
        svg::node::Value::from(v.clone()),
    );
}

impl SvgMap {
    pub fn make(
        geodata: &gpsdata::Track,
        waypoints: &Vec<waypoint::Waypoint>,
        segment: &segment::Segment,
        W: i32,
        H: i32,
        _debug: bool,
    ) -> SvgMap {
        let mut data = Data::new();
        let path = &geodata.utm;
        let mut bbox = BoundingBox::new();
        let range = &segment.range;
        for k in range.start..range.end {
            bbox.update(&geodata.utm[k]);
        }
        bbox.fix_aspect_ratio(W, H);
        // todo: path in the bbox, which more than the path in the range.
        for k in range.start..range.end {
            let p = &path[k];
            let (xg, yg) = bbox.to_graphics_coordinates(p, W, H);
            if data.is_empty() {
                data.append(Command::Move(Position::Absolute, (xg, yg).into()));
            }
            data.append(Command::Line(Position::Absolute, (xg, yg).into()));
        }

        let mut polyline = Attributes::new();
        // TODO better .set for Attributes.
        set_attr(&mut polyline, "fill", "none");
        set_attr(&mut polyline, "stroke", "black");
        set_attr(&mut polyline, "stroke-width", "2");
        set_attr_data(&mut polyline, "d", &data);
        set_attr(&mut polyline, "id", "polyline");

        let mut document = Attributes::new();
        set_attr(
            &mut document,
            "viewBox",
            format!("(0, 0, {W}, {H})").as_str(),
        );
        set_attr(&mut document, "width", format!("{W})").as_str());
        set_attr(&mut document, "height", format!("{H})").as_str());

        let V = waypoints_table::show_waypoints_in_table(&waypoints, &segment.profile.bbox);
        let mut points = Vec::new();
        for k in 0..waypoints.len() {
            let w = &waypoints[k];
            if !bbox.contains(&w.utm) {
                continue;
            }
            let mut svgPoint = SvgPoint::new();
            let (x, y) = bbox.to_graphics_coordinates(&w.utm, W, H);
            set_attr(
                &mut svgPoint.circle,
                "id",
                format!("wp-{}/circle", k).as_str(),
            );
            set_attr(&mut svgPoint.circle, "cx", format!("{x}").as_str());
            set_attr(&mut svgPoint.circle, "cy", format!("{y}").as_str());
            set_attr(&mut svgPoint.circle, "r", "4");
            if V.contains(&k) {
                let label = w.info.as_ref().unwrap().profile_label();
                svgPoint.text = String::from_str(label.trim()).unwrap();
                set_attr(&mut svgPoint.label, "id", format!("wp-{}/text", k).as_str());
                set_attr(&mut svgPoint.label, "text-anchor", "left");
                set_attr(&mut svgPoint.label, "font-size", "16");
                set_attr(&mut svgPoint.label, "x", format!("{}", x + 6f64).as_str());
                set_attr(&mut svgPoint.label, "y", format!("{}", y + 4.5f64).as_str());
            } else {
                set_attr(&mut svgPoint.circle, "fill", "blue");
                set_attr(&mut svgPoint.circle, "stroke", "black");
                set_attr(&mut svgPoint.circle, "stroke-width", "2");
                set_attr(&mut svgPoint.circle, "r", "3");
            }
            points.push(svgPoint);
        }
        SvgMap {
            polyline,
            points,
            document,
        }
    }
    pub fn import(filename: std::path::PathBuf) -> SvgMap {
        use svg::node::element::*;
        use svg::parser::Event;
        let mut polyline = Attributes::new();
        let mut document = Attributes::new();
        let mut content = String::new();
        let mut points = Vec::new();
        let mut current_circle = SvgPoint::new();
        for event in svg::open(filename, &mut content).unwrap() {
            match event {
                Event::Tag(tag::Circle, _, attributes) => {
                    if attributes.contains_key("id") {
                        let id = attributes.get("id").unwrap().clone().to_string();
                        let (p_id, _p_attr) = readid(id.as_str());
                        current_circle.id = String::from_str(p_id).unwrap();
                        current_circle.circle = attributes.clone();
                        println!("{}: {:?}", id, attributes);
                    }
                }
                Event::Tag(tag::Text, _, attributes) => {
                    if attributes.contains_key("id") {
                        let id = attributes.get("id").unwrap();
                        current_circle.label = attributes.clone();
                        println!("{}: {:?}", id, attributes);
                    }
                }
                Event::Text(data) => {
                    println!("Event::Text {:?}", data);
                    current_circle.text = String::from_str(data).unwrap();
                    debug_assert!(!current_circle.id.is_empty());
                    points.push(current_circle);
                    current_circle = SvgPoint::new();
                }
                Event::Tag(tag::Path, _, attributes) => {
                    if attributes.contains_key("id") {
                        let id = attributes.get("id").unwrap();
                        println!("{}: {:?} attributes", id, attributes.len());
                    }
                    polyline = attributes.clone();
                    let data = attributes.get("d").unwrap();
                    let data = Data::parse(data).unwrap();
                    for command in data.iter() {
                        match command {
                            &Command::Move(..) => { /* … */ }
                            &Command::Line(..) => { /* … */ }
                            _ => {}
                        }
                    }
                }
                Event::Tag(tag::SVG, _, attributes) => {
                    if !attributes.is_empty() {
                        document = attributes.clone();
                    }
                }
                _ => {
                    println!("event {:?}", event);
                }
            }
        }

        SvgMap {
            polyline,
            points,
            document,
        }
    }

    pub fn export(self) -> String {
        let mut document = Document::new();
        for (k, v) in self.document {
            println!("export {} -> {}", k, v);
            document = document.set(k, v);
        }

        let mut svgpath = Path::new();
        for (k, v) in self.polyline {
            svgpath = svgpath.set(k, v);
        }
        document = document.add(svgpath);

        for point in self.points {
            let mut circle = svg::node::element::Circle::new();
            for (k, v) in point.circle {
                circle = circle.set(k, v);
            }
            document = document.add(circle);

            let mut label = svg::node::element::Text::new(point.text);
            for (k, v) in point.label {
                label = label.set(k, v);
            }
            document = document.add(label);
        }
        document.to_string()
    }
}

pub fn map(
    geodata: &gpsdata::Track,
    waypoints: &Vec<waypoint::Waypoint>,
    segment: &segment::Segment,
    W: i32,
    H: i32,
    debug: bool,
) -> String {
    let svgMap = SvgMap::make(geodata, waypoints, segment, W, H, debug);
    svgMap.export()
}
