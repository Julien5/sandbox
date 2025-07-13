use typst_as_lib::*;
use typst_as_lib::{typst_kit_options::TypstKitFontOptions, TypstEngine};
use typst_pdf::*;

static TEMPLATE_FILE: &str = include_str!("/tmp/test.typ");
static OUTPUT: &str = "/tmp/test.pdf";

pub fn test() {
    // Read in fonts and the main source file.
    // We can use this template more than once, if needed (Possibly
    // with different input each time).

    let template = TypstEngine::builder()
        .main_file(TEMPLATE_FILE)
        .search_fonts_with(TypstKitFontOptions::default().include_system_fonts(false))
        .build();

    // Run it
    let doc = template
        .compile()
        .output
        .expect("typst::compile() returned an error!");

    let options = Default::default();

    // Create pdf
    let pdf = typst_pdf::pdf(&doc, &options).expect("Could not generate pdf.");
    std::fs::write(OUTPUT, pdf).expect("Could not write pdf.");
}
