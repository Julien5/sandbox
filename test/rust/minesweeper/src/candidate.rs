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
	tiles : BTreeMap<usize,Tile>
}

impl TileAccumulator {
	fn init() -> TileAccumulator {
		let ret=TileAccumulator {
			tiles: BTreeMap::new()
		};
		ret
	}
	fn aggregate(&mut self,tile:Tile) {
		let index=tile.tile_index();
		println!("collect tile index:{} tiles:{}",index,self.tiles.len());
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
		let mut prev:Option<&Tile>=None;
		let mut next:Option<&Tile>=None;
		let K=self.tiles.keys();
		let tiles_count=K.len();
		for key in K {
			let current = self.tiles.get(key).unwrap();
			if *key>0 {
				prev=Some(self.tiles.get(&(*key-1)).unwrap());
			}
			if *key<tiles_count-1 {
				next=Some(self.tiles.get(&(*key+1)).unwrap());
			}
			current.print_counts(prev,next,printer);
			prev=None;
			next=None;
		}
	}
}

fn make_printer(quiet:bool) -> Printer {
	let printer = match quiet {
		true => {
			Printer::new_quiet()
		}
		false => {
			Printer::new_verbose()
		}
	};
	printer
}

pub fn main(n:usize,b:usize,quiet:bool) {
	let Nchunks=match n {
		0..4 => 1,
		4..16 => 2,
		_ => 8
	};

	let m=((n as f32)/(Nchunks as f32)).floor() as usize;
	let b_chunk=((b as f32)/(Nchunks as f32)).floor() as usize;
	
	let indexes:Vec<usize>=(0..Nchunks).collect();
	let acc0=std::sync::Arc::new(std::sync::Mutex::new(TileAccumulator::init()));
	
	let _:Vec<()>=indexes.clone().into_par_iter()
		.map(|index| BombChunk::with_bomb_count(n,m,index,b_chunk)).into_par_iter()
		.map(|chunk| make_tile(chunk)).into_par_iter()
		.map(|tile| acc0.lock().unwrap().aggregate(tile))
		.collect();

	let mut printer=make_printer(quiet);
	println!("bombs");
	acc0.lock().unwrap().print_bombs(&mut printer);
	println!("counts");
	acc0.lock().unwrap().print_counts(&mut printer);
}

