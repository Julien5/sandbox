#![allow(non_snake_case)]

extern crate svg;

type Data = svg::node::element::path::Data;
type Group = svg::node::element::Group;
type Rect = svg::node::element::Path;
type Path = svg::node::element::Path;

fn line(p1: (i32, i32), p2: (i32, i32)) -> Data {
    Data::new().move_to(p1).line_to(p2)
}

fn simple(W: i32, H: i32) -> Data {
    Data::new()
        .move_to((0, 0))
        .line_to((W, 0))
        .line_to((W, H))
        .line_to((0, H))
        .line_to((0, 0))
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

fn simplerect(id: &str, color: &str, W: i32, H: i32) -> Rect {
    rect(id, color, simple(W, H))
}

fn simpleblackrect(id: &str, W: i32, H: i32) -> Rect {
    simplerect(id, "black", W, H)
}

fn bbrect(id: &str, color: &str, TL: (i32, i32), BR: (i32, i32)) -> Rect {
    rect(id, color, bbox(TL, BR))
}

fn testpath() -> Rect {
    Rect::new()
        .set("id", "test")
        .set("fill", "yellow")
        .set("stroke", "yellow")
        .set("stroke-width", "4")
        .set("d", _testpath())
}

fn transformSL(_W: i32, H: i32, Mleft: i32, Mbottom: i32) -> String {
    format!("translate({} {}) scale(-1 -1)", Mleft, H - Mbottom)
}

fn transformSB(_W: i32, H: i32, Mleft: i32, Mbottom: i32) -> String {
    format!("translate({} {})", Mleft, H - Mbottom)
}

fn transformSD(_W: i32, H: i32, Mleft: i32, Mbottom: i32) -> String {
    format!("translate({} {}) scale(1 -1)", Mleft, H - Mbottom)
}

fn dashed(from: (i32, i32), to: (i32, i32)) -> Group {
    let p = Path::new()
        .set("stroke", "black")
        .set("stroke-dasharray", "1.0,2.5,5.0,5.0,10.0,5.0")
        .set("d", line(from, to));

    Group::new()
        .set("fill", "none")
        .set("color", "black")
        .set("stroke-width", "1")
        .set("stroke-linecap", "butt")
        .set("stroke-linejoin", "miter")
        .add(p)
}

fn stroke(width: &str, from: (i32, i32), to: (i32, i32)) -> Group {
    let p = Path::new().set("stroke", "black").set("d", line(from, to));

    Group::new()
        .set("fill", "none")
        .set("color", "black")
        .set("stroke-width", width)
        .set("stroke-linecap", "butt")
        .set("stroke-linejoin", "miter")
        .add(p)
}

fn main() {
    use svg::Document;

    let W = 600;
    let H = 400;
    let Mleft = 50;
    let Mbottom = 50;
    let WD = W - Mleft;
    let HD = H - Mbottom;

    let BG = Group::new()
        .set("id", "frame")
        .add(bbrect("bg", "red", (0, 0), (W, H)));

    let SL = Group::new()
        .set("id", "SL")
        .set("transform", transformSL(W, H, Mleft, Mbottom))
        .add(bbrect("bg", "gray", (0, -Mbottom), (Mleft, HD)))
        .add(testpath());

    let SB = Group::new()
        .set("id", "SB")
        .set("transform", transformSB(W, H, Mleft, Mbottom))
        .add(bbrect("bg", "lightgray", (0, 0), (WD, Mbottom)))
        .add(testpath());

    let SD = Group::new()
        .set("id", "SB")
        .set("transform", transformSD(W, H, Mleft, Mbottom))
        .add(bbrect("bg", "lightblue", (0, 0), (WD, HD)))
        .add(testpath())
        .add(dashed((0, 300), (WD, 300)))
        .add(dashed((0, 200), (WD, 200)))
        .add(dashed((0, 100), (WD, 100)))
        .add(stroke("2", (0, 0), (WD, 0)))
        .add(stroke("2", (0, 0), (0, HD)));

    let world = Group::new()
        .set("id", "world")
        .set("transform", "translate(5 5)")
        .add(simpleblackrect("world", W, H))
        .add(BG)
        .add(SL)
        .add(SB)
        .add(SD);

    let document = Document::new()
        .set("width", 700)
        .set("height", 700)
        .set("viewBox", (0, 0, 700, 700))
        .add(simpleblackrect("table", 700, 700))
        .add(world);

    svg::save("image.svg", &document).unwrap();
}
