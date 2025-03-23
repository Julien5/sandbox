#![allow(non_snake_case)]

use std::env;

use rand::rng;
use rand::prelude::SliceRandom;

// use rayon::prelude::*;

fn _2d(index:usize,n:usize) -> (usize,usize) {
	assert!(index<(n*n));
	let x=index%n;
	let y=index/n;
	(x,y)
}

fn _1d(c:(usize,usize),n:usize) -> usize {
	c.1*n+c.0
}

type Element = usize;
type BombPositions = Vec<usize>;

const _EMPTY : Element = 10;
const BOMB  : Element = 9;
const ZERO  : Element = 0;

struct BombChunk {
	positions:BombPositions,
	n:usize
}

fn distinct_random_numbers(n:usize,b:usize) -> BombPositions {
	// generates [0,1,...,N-1]
	let mut G : Vec<usize>=(0usize..(n*n)).collect();
	let mut rng = rng();
	// shuffle it and keep the first b elements.
	G.shuffle(&mut rng);
	G.truncate(b);
	assert_eq!(G.len(),b);
	for g in &mut G {
		let (x,y)=_2d(*g,n);
		let p2=_1d((x+1,y+1),n+2);
		*g=p2
	}
	G
}

impl BombChunk {
	fn with_positions(n:usize,positions:BombPositions) -> BombChunk {
		let chunk=BombChunk {
			n:n,
			positions:positions
		};
		chunk
	}
}

struct Tile {
	grid: Vec<Element>,
	bomb_chunk: BombChunk,
}

impl Tile {
	fn with_chunk(chunk:BombChunk) -> Tile {
		let mut g=Tile {
			grid: vec![ZERO; (chunk.n+2)*(chunk.n+2)],
			bomb_chunk:chunk
		};
		for p in &g.bomb_chunk.positions {
			g.grid[*p]=BOMB;
		}
		g
	}

	fn print(&self, writer: &mut BufWriter<File>) {
		let print_lookup: [u8;11] = [b' ',b'1',b'2',b'3',b'4',b'5',b'6',b'7',b'8',b'B',b' '];
		assert!(!self.grid.is_empty());
		let n=self.bomb_chunk.n;
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
		let n=self.bomb_chunk.n;
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
		let positions=self.bomb_chunk.positions.clone();
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

fn worker(bomb_chunk:BombChunk) -> Tile {
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
}

fn aggregate(acc:TileAccumulator,tile:Tile) -> TileAccumulator {
	println!("aggregating tile:{} tiles:{}",tile.bomb_chunk.n,acc.tiles.len());
	acc
}

fn main() {
	let args: Vec<String> = env::args().collect();

	let stdout = unsafe { File::from_raw_fd(1) };
	let mut writer = BufWriter::new(stdout);
	
	let quiet : bool = args[1].contains("quiet");
	let n = args[2].parse::<usize>().unwrap();
	let b = args[3].parse::<usize>().unwrap();

	let bomb_chunk = BombChunk::with_positions(n,distinct_random_numbers(n,b));
	let mut grid = Tile::with_chunk(bomb_chunk);
	if quiet == false {
		grid.print(&mut writer);
	}
	grid.count_bombs();
	if quiet == false {
		grid.print(&mut writer);
	}
	let _ = writer.flush();

	let pos=distinct_random_numbers(n,b);
	println!("len={}",pos.len());
	let mut bomb_chunks=vec![];
	for _k in 0..3 {
		bomb_chunks.push(BombChunk::with_positions(n,distinct_random_numbers(n,b)));
	}
	let _acc=TileAccumulator::init();
	let _ret=bomb_chunks.into_iter()
		.map(|chunk| worker(chunk))
		.fold(TileAccumulator::init(),|acc,tile| aggregate(acc,tile));
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
