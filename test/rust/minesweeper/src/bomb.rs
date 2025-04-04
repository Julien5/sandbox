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
	let mut positions : Vec<usize>=vec![0;n*m];
	for j in 0..m {
		let line=Vec::from_iter((j+1)*(n+2)+1..(j+1)*(n+2)+n+1);
		positions[j*n..(j+1)*n].copy_from_slice(line.as_slice());
	}
	debug_assert_eq!(positions[0],n+3);
	
	for i in 0..b {
		let end=m*n-i;
		let j=rng.random_range(0..end);
		positions.swap(j,end-1);
	}
	let mut G : Vec<usize>=vec![0;b];
	let end=m*n;
	for i in 0..b  {
		G[i] = positions[end-i-1];
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

