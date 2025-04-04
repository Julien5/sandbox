#![allow(non_snake_case)]

mod baseline;
mod candidate;
mod bomb;
mod utils;
mod tile;

use std::env;

fn main() {
	let args: Vec<String> = env::args().collect();

	let algorithm = args[1].clone();
	let quiet : bool = args[2].contains("quiet");
	let n = args[3].parse::<usize>().unwrap();
	let b = args[4].parse::<usize>().unwrap();
	
	match algorithm.as_str() {
		"baseline" => baseline::main(n,b,quiet),
		"candidate" => candidate::main(n,b,quiet),
		&_ => {
			println!("unknown {}",algorithm);
		}
	}
}

