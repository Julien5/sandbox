#![allow(non_snake_case)]

use std::env;

pub mod utils;
pub mod bomb;
pub mod tile;

use bomb::*;
use tile::*;

// use rayon::prelude::*;

// for stdout efficiency
use std::{
    fs::File,
    io::{BufWriter, Write},
    os::unix::io::FromRawFd,
};

fn chunk_count(bomb_chunk:BombChunk) -> Tile {
	let n=12usize;
	println!("worker on grid of size {n}");
	let mut grid = Tile::with_chunk(bomb_chunk);
	grid.count_bombs();
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
	fn aggregate(&mut self,tile:Tile) {
		let index=tile.index();
		println!("aggregating tile index:{} tiles:{}",index,self.tiles.len());
		self.tiles.push(tile);	
	}
}


fn main() {
	let args: Vec<String> = env::args().collect();

	let stdout = unsafe { File::from_raw_fd(1) };
	let mut writer = BufWriter::new(stdout);
	
	let quiet : bool = args[1].contains("quiet");
	let n = args[2].parse::<usize>().unwrap();
	let b = args[3].parse::<usize>().unwrap();

	let bomb_chunk = BombChunk::with_b(n,0,b);
	let mut grid = Tile::with_chunk(bomb_chunk);
	if quiet == false {
		grid.print(&mut writer);
	}
	grid.count_bombs();
	if quiet == false {
		grid.print(&mut writer);
	}
	let _ = writer.flush();

	let mut bomb_chunks=BombChunks::new();
	let Nchunks=16;
	println!("generate chunks");
	for index in 0..Nchunks {
		let chunk=BombChunk::with_b(n,index,b);
		bomb_chunks.push(chunk);
	}
	println!("count and collect");
	let acc=std::sync::Arc::new(std::sync::Mutex::new(TileAccumulator::init()));
	let _:Vec<()>=bomb_chunks.into_iter()
		.map(|chunk| chunk_count(chunk))
		.map(|tile| {
			acc.lock().unwrap().aggregate(tile);
		}).collect();
	()
}

#[cfg(test)]
mod tests {
	use super::*;
    #[test]
    fn test_2d() {
		let c = _2d(0_usize,4);
		assert_eq!(c,(0,0));

		let c = _2d(1_usize,4);
		assert_eq!(c,(1,0));

		let c = _2d(4_usize,4);
		assert_eq!(c,(0,1));
		
		let c = _2d(15_usize,4);
		assert_eq!(c,(3,3));
    }
}
