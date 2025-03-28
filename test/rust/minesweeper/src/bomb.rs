use rand::rng;
use rand::prelude::SliceRandom;

use crate::utils::*;
type BombPositions = Vec<usize>;

#[derive(Clone, Debug)]
pub struct BombChunk {
	n:usize,	
	index:usize,
	positions:BombPositions
}

pub struct BombChunks {
	chunks : Vec<BombChunk>,
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
	pub fn with_b(n:usize,index:usize,b:usize) -> BombChunk {
		let chunk=BombChunk {
			n:n,
			index:index,
			positions:distinct_random_numbers(n,b)
		};
		chunk
	}
	pub fn positions(&self) -> &BombPositions {
		&self.positions
	}
	pub fn n(&self) -> usize {
		self.n
	}
	pub fn index(&self) -> usize {
		self.index
	}
}

impl BombChunks {
	pub fn new() -> BombChunks {
		let ret=BombChunks {
			chunks:vec![]
		};
		ret
	}
	pub fn push(&mut self, chunk:BombChunk) {
		self.chunks.push(chunk);
	}
}

impl Iterator for BombChunks {
    type Item = BombChunk;
    fn next(&mut self) -> Option<Self::Item> {
		if self.chunks.is_empty() {
			return None;
		}
		let N=self.chunks.len();
		println!("remaining number of bomb chunks:{N}");
        Some(self.chunks.remove(N-1))
    }
}

