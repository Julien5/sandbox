#![allow(non_snake_case)]

use crate::gpsdata::ProfileBoundingBox;
use svg::node::element::path::Command;
use svg::node::element::path::Position;
type Data = svg::node::element::path::Data;
type Group = svg::node::element::Group;
type Rect = svg::node::element::Path;
type Path = svg::node::element::Path;
type Text = svg::node::element::Text;
use crate::gpsdata;

fn line(p1: (i32, i32), p2: (i32, i32)) -> Data {
    Data::new().move_to(p1).line_to(p2)
}

fn bbox(TL: (i32, i32), BR: (i32, i32)) -> Data {
    Data::new()
        .move_to((TL.0, TL.1))
        .line_to((TL.0, BR.1))
        .line_to((BR.0, BR.1))
        .line_to((BR.0, TL.1))
        .line_to((TL.0, TL.1))
}

fn _testpath() -> Data {
    Data::new().move_to((0, 0)).line_to((20, 20))
}

fn rect(id: &str, color: &str, data: Data) -> Rect {
    Rect::new().set("id", id).set("fill", color).set("d", data)
}

fn bbrect(id: &str, color: &str, TL: (i32, i32), BR: (i32, i32)) -> Rect {
    rect(id, color, bbox(TL, BR))
}

fn transformSL(_W: i32, H: i32, Mleft: i32, Mbottom: i32) -> String {
    format!("translate({} {}) scale(-1 -1)", Mleft, H - Mbottom)
}

fn transformSB(_W: i32, H: i32, Mleft: i32, Mbottom: i32) -> String {
    format!("translate({} {})", Mleft, H - Mbottom)
}

fn transformSD(_W: i32, H: i32, Mleft: i32, Mbottom: i32, _WD: i32) -> String {
    let alpha = 1; //WD as f64 / 100f64;
    format!(
        "translate({} {}) scale(1 -1) scale({} 1)",
        Mleft,
        H - Mbottom,
        alpha
    )
}

fn dashed(from: (i32, i32), to: (i32, i32)) -> Path {
    let p = Path::new()
        .set("stroke", "black")
        .set("stroke-dasharray", "1.0,2.5,5.0,5.0,10.0,5.0")
        .set("d", line(from, to));
    p
}

fn stroke(width: &str, from: (i32, i32), to: (i32, i32)) -> Path {
    let p = Path::new()
        .set("stroke-width", width)
        .set("stroke", "black")
        .set("d", line(from, to));
    p
}

fn textx(label: &str, pos: (i32, i32)) -> Text {
    let ret = Text::new(label)
        .set("font-family", "sans")
        .set("text-anchor", "middle")
        .set("transform", format!("translate({} {})", pos.0, pos.1));
    ret
}

fn texty(label: &str, pos: (i32, i32)) -> Text {
    let ret = Text::new(label)
        .set("text-anchor", "end")
        .set("font-family", "sans")
        .set(
            "transform",
            format!("translate({} {}) scale(-1 -1)", pos.0, pos.1),
        );
    ret
}

fn to_view(x: f64, y: f64) -> (f64, f64) {
    ((x / 100f64), 250f64 - (y / 5f64))
}

fn track(d: Data) -> Path {
    let p = Path::new()
        .set("stroke", "black")
        .set("stroke-width", 2)
        .set("shape-rendering", "geometricPrecision")
        .set("fill", "transparent")
        .set("d", d);
    p
}

fn toSD((x, y): (f64, f64), WD: i32, HD: i32, bbox: &gpsdata::ProfileBoundingBox) -> (i32, i32) {
    assert!(bbox.xmin <= bbox.xmax);
    assert!(bbox.ymin <= bbox.ymax);
    let f = |x: f64| -> f64 {
        let a = WD as f64 / (bbox.xmax - bbox.xmin);
        let b = -bbox.xmin * a;
        a * x + b
    };
    let g = |y: f64| -> f64 {
        let a = HD as f64 / (bbox.ymax - bbox.ymin);
        let b = -bbox.ymin * a;
        a * y + b
    };
    println!("{}", g(y));
    assert!(g(y) >= 0f64);
    (f(x).floor() as i32, g(y).floor() as i32)
}

fn data(
    geodata: &gpsdata::Track,
    range: &std::ops::Range<usize>,
    (WD, HD): (i32, i32),
    bbox: &ProfileBoundingBox,
) -> Data {
    let mut data = Data::new();
    for k in range.start..range.end {
        let (x, y) = (geodata.distance(k), geodata.elevation(k));
        let (xg, yg) = toSD((x, y), WD, HD, bbox);
        if data.is_empty() {
            data.append(Command::Move(Position::Absolute, (xg, yg).into()));
        }
        data.append(Command::Line(Position::Absolute, (xg, yg).into()));
    }
    data
}

pub fn canvas(
    geodata: &gpsdata::Track,
    range: &std::ops::Range<usize>,
    bbox: &gpsdata::ProfileBoundingBox,
) -> svg::Document {
    let W = 1400;
    let H = 400;
    let Mleft = 50;
    let Mbottom = 50;
    let WD = W - Mleft;
    let HD = H - Mbottom;

    let BG = Group::new().set("id", "frame");

    let mut SL = Group::new()
        .set("id", "SL")
        .set("transform", transformSL(W, H, Mleft, Mbottom));

    let mut SB = Group::new()
        .set("id", "SB")
        .set("transform", transformSB(W, H, Mleft, Mbottom));

    let mut SD = Group::new()
        .set("id", "SD")
        .set("transform", transformSD(W, H, Mleft, Mbottom, WD))
        .add(bbrect("bg", "lightgray", (0, 0), (WD, HD)))
        .add(stroke("3", (0, 0), (WD, 0)))
        .add(stroke("3", (0, 0), (0, HD)))
        .add(stroke("3", (0, HD), (WD, HD)))
        .add(stroke("3", (WD, 0), (WD, HD)));

    for dy in 1..8 {
        let ys = (dy - 1) * 50;
        let yd = 25 + (dy - 1) * 50;
        SD = SD.add(dashed((0, yd), (WD, yd)));
        SD = SD.add(stroke("1", (0, ys), (WD, ys)));
        SD = SD.add(stroke("1", (0, ys), (WD, ys)));
    }

    let deltax = 200;
    for dx in 1..9 {
        let xd = (dx - 1) * deltax;
        let xdd = xd - deltax / 2;
        SD = SD.add(dashed((xdd, 0), (xdd, HD)));

        if xd > WD {
            break;
        }
        SD = SD.add(stroke("1", (xd, 0), (xd, HD)));
        let tx = (dx - 2) * 20;
        if tx >= 0 {
            SB = SB.add(textx(format!("{}", (dx - 2) * 20).as_str(), (xd, 25)));
        }
    }

    SD = SD.add(track(data(geodata, range, (WD, HD), bbox)));

    for d in 1..8 {
        let ys = (d - 1) * 50;
        SL = SL.add(texty(format!("{}", (d - 1) * 200).as_str(), (10, ys - 5)));
    }

    let world = Group::new()
        .set("id", "world")
        .set("shape-rendering", "crispEdges")
        .set("transform", "translate(5 5)")
        .add(BG)
        .add(SL)
        .add(SB)
        .add(SD);

    let document = svg::Document::new()
        .set("width", 1700)
        .set("height", 500)
        .set("viewBox", (0, 0, 1700, 500))
        .add(world);

    document
}
