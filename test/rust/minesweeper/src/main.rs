#![allow(non_snake_case)]

mod utils;
mod bomb;
mod tile;
mod baseline;

use std::env;

use bomb::*;
use tile::*;

// use rayon::prelude::*;

// for stdout efficiency
use std::{
    fs::File,
    io::{BufWriter, Write},
    os::unix::io::FromRawFd,
};

struct Printer {
	writer : Option<BufWriter<File>>,
}

impl Printer {
	fn new_verbose() -> Printer {
		let stdout=unsafe { File::from_raw_fd(1) };
		let p=Printer {
			writer : Some(BufWriter::new(stdout))
		};
		p
	}
	fn new_quiet() -> Printer {
		let p=Printer {
			writer : None
		};
		p
	}
	fn print(&mut self,tile:&Tile) {
		match &mut self.writer {
			Some(writer) => {
				tile.print(writer);
				let _=writer.flush();
			}
			_ => {	}
		}
	}
}

fn make_tile(bomb_chunk:BombChunk, printer:&mut Printer) -> Tile {
	println!("worker on grid of size {}",bomb_chunk.n());
	let grid = Tile::with_chunk(bomb_chunk);
	printer.print(&grid);
	grid
}

struct TileAccumulator {
	tiles : Vec<Tile>,
}

impl TileAccumulator {
	fn init() -> TileAccumulator {
		let ret=TileAccumulator {
			tiles:vec![]
		};
		ret
	}
	fn aggregate(&mut self,mut tile:Tile,printer:&mut Printer) {
		let index=tile.index();
		println!("aggregating tile index:{} tiles:{}",index,self.tiles.len());
		tile.count_bombs();
		printer.print(&tile);
		self.tiles.push(tile);	
	}
}

fn main() {
	let args: Vec<String> = env::args().collect();
	
	let quiet : bool = args[1].contains("quiet");
	let n = args[2].parse::<usize>().unwrap();
	let b = args[3].parse::<usize>().unwrap();
	baseline::main(n,b);

	let mut printer = match quiet {
		true => {
			Printer::new_quiet()
		}
		false => {
			Printer::new_verbose()
		}
	};

	let mut bomb_chunks=BombChunks::new();
	let Nchunks=1;
	println!("generate chunks");
	for index in 0..Nchunks {
		let chunk=BombChunk::with_bomb_count(n,index,b);
		bomb_chunks.push(chunk);
	}
	println!("count and collect");
	let tiles : Vec<Tile> = bomb_chunks.into_iter()
		.map(|chunk| make_tile(chunk,&mut printer)).collect();
	let acc=std::sync::Arc::new(std::sync::Mutex::new(TileAccumulator::init()));
	let _results : Vec<()> = tiles.into_iter()
		.map(|tile| {
			acc.lock().unwrap().aggregate(tile,&mut printer);
		}).collect();
}

