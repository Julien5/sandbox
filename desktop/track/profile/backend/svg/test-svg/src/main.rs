#![allow(non_snake_case)]

extern crate svg;

type Data = svg::node::element::path::Data;
type Group = svg::node::element::Group;
type Rect = svg::node::element::Path;

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

fn simplerect(id: &str, color: &str, W: i32, H: i32) -> Rect {
    Rect::new()
        .set("id", id)
        .set("fill", color)
        .set("d", simple(W, H))
}

fn simpleblackrect(id: &str, W: i32, H: i32) -> Rect {
    simplerect(id, "black", W, H)
}

fn bbrect(id: &str, color: &str, TL: (i32, i32), BR: (i32, i32)) -> Rect {
    Rect::new()
        .set("id", id)
        .set("fill", color)
        .set("d", bbox(TL, BR))
}

fn testpath() -> Rect {
    Rect::new()
        .set("id", "test")
        .set("fill", "yellow")
        .set("stroke", "yellow")
        .set("stroke-width", "4")
        .set("d", _testpath())
}

fn transformSL(W: i32, H: i32, Mleft: i32, Mbottom: i32) -> String {
    format!("translate({} {}) scale(-1 -1)", Mleft, H - Mbottom)
}

fn transformSB(W: i32, H: i32, Mleft: i32, Mbottom: i32) -> String {
    format!("translate({} {})", Mleft, H - Mbottom)
}

fn transformSD(W: i32, H: i32, Mleft: i32, Mbottom: i32) -> String {
    format!("translate({} {}) scale(1 -1)", Mleft, H - Mbottom)
}

fn main() {
    use svg::Document;

    let W = 600;
    let H = 400;
    let Mleft = 100;
    let Mbottom = 100;

    let BG = Group::new()
        .set("id", "frame")
        .add(bbrect("bg", "red", (0, 0), (W, H)));

    let SL = Group::new()
        .set("id", "SL")
        .set("transform", transformSL(W, H, Mleft, Mbottom))
        .add(bbrect("bg", "gray", (0, -Mbottom), (Mleft, H - Mbottom)))
        .add(testpath());

    let SB = Group::new()
        .set("id", "SB")
        .set("transform", transformSB(W, H, Mleft, Mbottom))
        .add(bbrect("bg", "lightgray", (0, 0), (W - Mleft, Mbottom)))
        .add(testpath());

    let SD = Group::new()
        .set("id", "SB")
        .set("transform", transformSD(W, H, Mleft, Mbottom))
        .add(bbrect("bg", "lightblue", (0, 0), (W - Mleft, H - Mbottom)))
        .add(testpath());

    let world = Group::new()
        .set("id", "world")
        .set("transform", "translate(50 50)")
        .add(simpleblackrect("world", W, H))
        .add(BG)
        .add(SL)
        .add(SB)
        .add(SD);

    let Wm = W + 400;
    let Hm = H + 700;

    let document = Document::new()
        .set("width", 700)
        .set("height", 700)
        .set("viewBox", (0, 0, 700, 700))
        .add(simpleblackrect("world", 1000, 1000))
        .add(world);

    svg::save("image.svg", &document).unwrap();
}
