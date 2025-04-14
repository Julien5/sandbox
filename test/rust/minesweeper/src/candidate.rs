#![allow(non_snake_case)]

use std::collections::BTreeMap;

use crate::bomb::*;
use crate::tile::*;
use crate::utils::*;

use rayon::prelude::*;

fn make_tile(bomb_chunk:BombChunk) -> Tile {
	Tile::with_chunk(bomb_chunk)
}

struct TileAccumulator {
	tiles : BTreeMap<usize,Tile>,
}

impl TileAccumulator {
	fn init() -> TileAccumulator {
		let ret=TileAccumulator {
			tiles: BTreeMap::new(),
		};
		ret
	}
	
	fn aggregate(&mut self, mut tile: Tile) {
		let index=tile.tile_index();
		if index>0 {
			match self.tiles.get_mut(&(index-1)) {
				Some(prev) => merge(prev,&mut tile),
				_ => {}
			}
		}
		match self.tiles.get_mut(&(index+1)) {
			Some(next) => merge(&mut tile,next),
			_ => {}
		}
		self.tiles.insert(tile.tile_index(),tile);
	}
	
	fn print_bombs(&mut self,printer:&mut Printer) {
		let K=self.tiles.keys();
		for key in K {
			let current = self.tiles.get(key).unwrap();
			current.print_bombs(printer);
		}
	}
	fn print_counts(&mut self,printer:&mut Printer) {
		let K=self.tiles.keys();
		for key in K {
			let current = self.tiles.get(key).unwrap();
			current.print_counts(printer);
		}
	}
}

fn _main_for(X:usize,Y:usize,Bchunk:usize,K:usize,quiet:bool) {
	let mut tiles = Vec::new();
	for index in 0..K {
		let chunk=BombChunk::with_bomb_count(X,Y,index,Bchunk);
		tiles.push(make_tile(chunk));
	}
	let mut acc=TileAccumulator::init();
	for tile in tiles {
		acc.aggregate(tile);
	}
	let mut printer=make_printer(quiet);
	acc.print_bombs(&mut printer);
	acc.print_counts(&mut printer);
}

fn _main_iter(X:usize,Y:usize,Bchunk:usize,K:usize,quiet:bool) {
	let tiles:Vec<Tile>=(0..K).into_iter()
		.map(|index| {
			let chunk=BombChunk::with_bomb_count(X,Y,index,Bchunk);
			make_tile(chunk)
		}).collect();
	let mut acc=TileAccumulator::init();
	for tile in tiles {
		acc.aggregate(tile);
	}
	let mut printer=make_printer(quiet);
	acc.print_bombs(&mut printer);
	acc.print_counts(&mut printer);
}

fn _main_parallel(X:usize,Y:usize,Bchunk:usize,K:usize,quiet:bool) {
	let acc=std::sync::Arc::new(std::sync::Mutex::new(TileAccumulator::init()));
	let _:Vec<()>=(0..K).into_par_iter()
		.map(|index| {
			let chunk=BombChunk::with_bomb_count(X,Y,index,Bchunk);
			let tile=make_tile(chunk);
			acc.lock().unwrap().aggregate(tile);
		}).collect();
	let mut printer=make_printer(quiet);
	log::info!("print bombs");
	acc.lock().unwrap().print_bombs(&mut printer);
	log::info!("print counts");
	acc.lock().unwrap().print_counts(&mut printer);
}

fn main_parallel(X:usize,Y:usize,Bchunk:usize,K:usize,quiet:bool) {
	let tiles:Vec<Tile>=(0..K).into_par_iter()
		.map(|index| {
			let chunk=BombChunk::with_bomb_count(X,Y,index,Bchunk);
			make_tile(chunk)
		}).collect();
	let mut acc=TileAccumulator::init();
	for tile in tiles {
		acc.aggregate(tile);
	}
	let mut printer=make_printer(quiet);
	log::info!("print bombs");
	acc.print_bombs(&mut printer);
	log::info!("print counts");
	acc.print_counts(&mut printer);
}


pub fn main(X:usize,B:usize,quiet:bool,K:usize) {
	/*
	let K=match B {
		0..4 => 1,
		4..16 => 2,
		_ => 1 // [X/2,B/2,32].into_iter().min().unwrap()
};
	 */

	log::info!("running with {} chunks",K);

	// https://stackoverflow.com/questions/57741820/how-to-get-the-floored-quotient-of-two-integers
	let Y = X/K;
	assert_eq!(Y*K,X);
	let Bchunk = B/K;
	
	log::info!("make bombs and count");
	//_main_for(X,Y,Bchunk,K,quiet);
	//_main_iter_2(X,Y,Bchunk,K,quiet);
	main_parallel(X,Y,Bchunk,K,quiet);
}

