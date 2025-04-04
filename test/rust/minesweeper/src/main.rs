#![allow(non_snake_case)]

mod baseline;
mod candidate;
mod bomb;
mod utils;
mod tile;
use std::io::Write;

use std::env;

fn main() {
	// env_logger::init();
	env_logger::Builder::new()
        .format(|buf, record| {
            writeln!(buf,
                "{} [{}] - {}",
                chrono::Local::now().format("%M:%S:%3f"),
                record.level(),
                record.args()
            )
        })
        .filter(None, log::LevelFilter::Info)
        .init();
	
	let args: Vec<String> = env::args().collect();

	let algorithm = args[1].clone();
	let quiet : bool = args[2].contains("quiet");
	let n = args[3].parse::<usize>().unwrap();
	let b = args[4].parse::<usize>().unwrap();
	
	match algorithm.as_str() {
		"baseline" => baseline::main(n,b,quiet),
		"candidate" => candidate::main(n,b,quiet),
		&_ => {
			log::error!("unknown {}",algorithm);
		}
	}
	log::info!("done");
}

