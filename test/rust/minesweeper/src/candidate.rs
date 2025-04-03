#![allow(non_snake_case)]

use crate::bomb::*;
use crate::tile::*;
use crate::utils::*;

use rayon::prelude::*;

type SharedPrinter = std::sync::Arc<std::sync::Mutex<Printer>>;

fn make_tile(bomb_chunk:BombChunk) -> Tile {
	Tile::with_chunk(bomb_chunk)
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
		let index=tile.tile_index();
		println!("collect tile index:{} tiles:{}",index,self.tiles.len());
		self.tiles.push(tile);	
	}
	fn print(&mut self,tile:Tile,printer:SharedPrinter) -> Tile {
		let index=tile.tile_index();
		println!("printing tile index:{} tiles:{}",index,self.tiles.len());
		tile.print_bombs(&mut printer.lock().unwrap());
		tile
	}
	fn close(&mut self,printer:&mut Printer) {
		let mut prev:Option<&Tile>=None;
		let mut next:Option<&Tile>=None;
		let K=self.tiles.len();
		for p in 0..K {
			let current = &self.tiles[p];
			if p>0 {
				prev=Some(&self.tiles[p-1]);
			}
			if p<K-1 {
				next=Some(&self.tiles[p+1]);
			}
			current.print_counts(prev,next,printer);
			prev=None;
			next=None;
		}
	}
}

fn make_printer(quiet:bool) -> Printer {
	let mut printer = match quiet {
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
	let Nchunks=2;

	let m=((n as f32)/(Nchunks as f32)).floor() as usize;
	let b_chunk=((b as f32)/(Nchunks as f32)).floor() as usize;
	
	let indexes:Vec<usize>=(0..Nchunks).collect();
	let acc0=std::sync::Arc::new(std::sync::Mutex::new(TileAccumulator::init()));
	let mut shared_printer=std::sync::Arc::new(std::sync::Mutex::new(make_printer(quiet)));
	
	let _:Vec<()>=indexes.clone().into_par_iter()
		.map(|index| BombChunk::with_bomb_count(n,m,index,b_chunk)).into_par_iter()
		.map(|chunk| make_tile(chunk)).into_par_iter()
		.map(|tile| acc0.lock().unwrap().print(tile,shared_printer.clone()))
		.map(|tile| acc0.lock().unwrap().aggregate(tile))
		.collect();

	acc0.lock().unwrap().close(&mut shared_printer.lock().unwrap());
}

