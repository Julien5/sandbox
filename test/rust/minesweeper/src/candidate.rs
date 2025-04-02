#![allow(non_snake_case)]

use crate::bomb::*;
use crate::tile::*;
use crate::utils::*;

// use rayon::prelude::*;

fn make_tile(bomb_chunk:BombChunk, printer:&mut Printer) -> Tile {
	println!("worker on grid of size {} {}",bomb_chunk.n(), bomb_chunk.m());
	let grid = Tile::with_chunk(bomb_chunk);
	grid.print(printer);
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
	fn aggregate(&mut self,mut tile:Tile,_printer:&mut Printer) {
		let index=tile.index();
		println!("aggregating tile index:{} tiles:{}",index,self.tiles.len());
		tile.count_bombs();
		//tile.print(printer);
		self.tiles.push(tile);	
	}
	fn close(&mut self,printer:&mut Printer) {
		let mut prev:Option<&Tile>=None;
		let mut next:Option<&Tile>=None;
		let K=self.tiles.len();
		for p in 0..K {
			let mut current = self.tiles[p].clone();
			prev=None;
			next=None;
			if p>0 {
				prev=Some(&self.tiles[p-1]);
			}
			if p<K-1 {
				next=Some(&self.tiles[p+1]);
			}
			current.merge(prev,next);
			current.print(printer);
		}
	}
}

pub fn main(n:usize,b:usize,quiet:bool) {
	let mut printer = match quiet {
		true => {
			Printer::new_quiet()
		}
		false => {
			Printer::new_verbose()
		}
	};

	let mut bomb_chunks=BombChunks::new();
	let Nchunks=2;
	let m=((n as f32)/(Nchunks as f32)).floor() as usize;
	let b_chunk=((b as f32)/(Nchunks as f32)).floor() as usize;
	dbg!(n,Nchunks,m);
	println!("generate chunks");
	for index in 0..Nchunks {
		let chunk=BombChunk::with_bomb_count(n,m,index,b_chunk);
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
	acc.lock().unwrap().close(&mut printer);
}

