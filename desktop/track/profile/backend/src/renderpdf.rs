use typst_as_lib::*;
use typst_as_lib::{typst_kit_options::TypstKitFontOptions, TypstEngine};
use typst_pdf::*;

pub fn test() -> Vec<u8> {
    // Read in fonts and the main source file.
    // We can use this template more than once, if needed (Possibly
    // with different input each time).

    static TEMPLATE_FILE: &str = include_str!("../data/test.typ");
    static OUTPUT: &str = "/tmp/test.pdf";

    let template = TypstEngine::builder()
        .main_file(TEMPLATE_FILE)
        .search_fonts_with(TypstKitFontOptions::default().include_system_fonts(false))
        .build();

    let doc = template
        .compile()
        .output
        .expect("typst::compile() returned an error!");

    let options = Default::default();

    let pdf = typst_pdf::pdf(&doc, &options).expect("Could not generate pdf.");
    pdf
}
