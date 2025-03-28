#![allow(non_snake_case)]

use std::env;

pub mod utils;
pub mod bomb;
use bomb::*;

use crate::utils::*;

use rayon::prelude::*;

type Element = usize;

const _EMPTY : Element = 10;
const BOMB  : Element = 9;
const ZERO  : Element = 0;

struct Tile {
	grid: Vec<Element>,
	bomb_chunk: BombChunk,
}

impl Tile {
	fn with_chunk(chunk:BombChunk) -> Tile {
		let mut g=Tile {
			grid: vec![ZERO; (chunk.n()+2)*(chunk.n()+2)],
			bomb_chunk:chunk
		};
		for p in g.bomb_chunk.positions() {
			g.grid[*p]=BOMB;
		}
		g
	}

	fn print(&self, writer: &mut BufWriter<File>) {
		let print_lookup: [u8;11] = [b' ',b'1',b'2',b'3',b'4',b'5',b'6',b'7',b'8',b'B',b' '];
		assert!(!self.grid.is_empty());
		let n=self.bomb_chunk.n();
		let mut output:Vec<u8> = vec![b' ';n+1];
		output[n]=b'\n';
		for k1 in 0..n {
			for k2 in 0..n {
				let k=_1d((k1+1,k2+1),n+2);
				output[k2]=print_lookup[self.grid[k]];
			}
			writer.write_all(&output).unwrap();
		}
	}

	fn increment_neighboors(&mut self, pos:usize) {
		let n=self.bomb_chunk.n();
		let (posx,posy)=_2d(pos,n+2);
		assert!(posx>0 && posx<n+1);
		assert!(posy>0 && posy<n+1);
		for i in 0..3 {
			for j in 0..3 {
				if i == 1 && j == 1 {
					continue
				}
				let l=_1d((posx+i-1,posy+j-1),n+2);
				assert!(l<self.grid.len());
				if self.grid[l] != BOMB {
					self.grid[l]+=1;
				}
			}
		}
	}
	
	fn count_bombs(&mut self) {
		let positions=self.bomb_chunk.positions().clone();
		positions.iter().for_each(|bpos| {
			self.increment_neighboors(*bpos);
		}
		);
	}
}

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
		let index=tile.bomb_chunk.index();
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
