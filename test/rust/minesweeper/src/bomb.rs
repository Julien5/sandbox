use rand::rngs::StdRng;
use rand::prelude::SliceRandom;
use rand::SeedableRng;

use crate::utils::*;
type BombPositions = Vec<usize>;

#[derive(Clone, Debug)]
pub struct BombChunk {
	n:usize,
	m:usize,
	index:usize,
	positions:BombPositions
}

pub struct BombChunks {
	chunks : Vec<BombChunk>,
}

fn distinct_random_numbers(n:usize,m:usize,b:usize) -> BombPositions {
	// generates [0,1,...,N-1]
	let mut G : Vec<usize>=(0usize..(n*m)).collect();
	let mut rng = StdRng::seed_from_u64(2);
	// shuffle it and keep the first b elements.
	G.shuffle(&mut rng);
	G.truncate(b);
	assert_eq!(G.len(),b);
	for g in &mut G {
		let (x,y)=_2d(*g,n,m);
		let p2=_1d((x+1,y+1),n+2,m+2);
		let (px,py)=_2d(p2,n+2,m+2);
		dbg!(n,m,x,y,p2,px,py);
		assert!(px>0 && px<(n+2-1));
		assert!(py>0 && py<(m+2-1));
		*g=p2
	}
	G
}

impl BombChunk {
	pub fn with_bomb_count(n:usize,m:usize,index:usize,b:usize) -> BombChunk {
		let chunk=BombChunk {
			n:n,
			m:m,
			index:index,
			positions:distinct_random_numbers(n,m,b)
		};
		chunk
	}
	pub fn positions(&self) -> &BombPositions {
		&self.positions
	}
	pub fn n(&self) -> usize {
		self.n
	}
	pub fn m(&self) -> usize {
		self.m
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

