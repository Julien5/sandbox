use rand::rngs::StdRng;
use rand::{Rng, SeedableRng};

use crate::utils::*;
type BombPositions = Vec<usize>;

#[derive(Clone, Debug)]
pub struct BombChunk {
	n:usize,
	m:usize,
	index:usize,
	positions:BombPositions
}

fn distinct_random_numbers(n:usize,m:usize,b:usize) -> BombPositions {
	let mut rng = StdRng::seed_from_u64(3);
	let mut positions : Vec<usize>=(0usize..(n*m)).collect();
	for i in 0..b {
		let end=m*n-i;
		let j=rng.random_range(0..end);
		positions.swap(j,end-1);
	}
	let mut G : Vec<usize>=vec![0;b];
	let end=m*n;
	for i in 0..b  {
		let g = positions[end-i-1];
		let (x,y)=_2d(g,n,m);
		let p2=_1d((x+1,y+1),n+2,m+2);
		let (px,py)=_2d(p2,n+2,m+2);
		assert!(px>0 && px<(n+2-1));
		assert!(py>0 && py<(m+2-1));
		G[i]=p2;
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

