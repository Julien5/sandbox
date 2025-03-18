extern crate chrono;
extern crate glob;

pub mod speed;
pub mod worker;

use std::io::{BufReader, Read};
use std::fs::File;

use rayon::prelude::*;

fn main() {
	let _args: Vec<String> = std::env::args().collect();
	let mut files=Vec::new();
	for entry in glob::glob("/home/julien/projects/tracks/*/*/*.gpx").unwrap() {
		let f1=entry.unwrap();
		let fe=f1.display().to_string();
		if fe.contains("-moving.gpx") {
			files.push(fe);
		}
    }

	files.sort();
	//files.truncate(100);

	let mut contents=Vec::new();
	for filename in &files {
		let file = File::open(filename.clone()).unwrap();
		let mut reader_file = BufReader::new(file);
		let mut content: Vec<u8> = Vec::new();
		let _ = reader_file.read_to_end(&mut content);
		contents.push(content);
	}

	assert_eq!(contents.len(),files.len());
	assert!(contents.len() > 0usize);
	//let s : usize = contents.iter().map(|content| worker(content)).sum();
	let s : usize = contents.par_iter().map(|content| worker::worker(content)).sum();
	println!("analyzed {s} points");

	let n=contents.len();
	println!("analyzed {n} files");
}
