use crate::utils;

type BombPositions = Vec<usize>;

pub struct BombChunk {
	n:usize,
	m:usize,
	index:usize,
	positions:BombPositions
}

fn distinct_random_numbers(n:usize,m:usize,b:usize) -> BombPositions {
	// populate the available positions excluding the margins.
	let mut positions : Vec<usize>=vec![0;n*m];
	for j in 0..m {
		let begin=(j+1)*(n+2)+1;
		let end  = begin+n;
		let line=Vec::from_iter(begin..end);
		positions[j*n..(j+1)*n].copy_from_slice(line.as_slice());
	}
	utils::distinct_random_numbers(positions,b) as BombPositions
}

impl BombChunk {
	pub fn with_bomb_count(n:usize,m:usize,index:usize,b:usize) -> BombChunk {
		log::trace!("make bombs for chunk index:{}",index);
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

