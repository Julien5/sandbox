use crate::bomb::*;
use crate::utils::*;

type Element = usize;

const _EMPTY : Element = 10;
const BOMB  : Element = 9;
const ZERO  : Element = 0;

#[derive(Clone)]
pub struct Tile {
	grid: Vec<Element>,
	bomb_chunk: BombChunk,
	n:usize,
	m:usize,
	nM:usize,
	mM:usize
}

impl Tile {
	fn index(&self, kn:usize, km:usize) -> usize {
		_1d((kn+1,km+1),self.n+2,self.m+2)
	}

	fn index_all(&self, kn:usize, km:usize) -> usize {
		_1d((kn,km),self.n+2,self.m+2)
	}
	
	fn at(&self, kn:usize, km:usize) -> Element {
		self.grid[_1d((kn+1,km+1),self.n+2,self.m+2)]
	}

	fn at_all(&self, kn:usize, km:usize) -> Element {
		self.grid[_1d((kn,km),self.n+2,self.m+2)]
	}
	
	pub fn with_chunk(chunk:BombChunk) -> Tile {
		let n=chunk.n();
		let m=chunk.m();
		let nM=n+2;
		let mM=m+2;
		let mut g=Tile {
			grid: vec![ZERO; nM*mM],
			bomb_chunk:chunk,
			n:n,
			m:m,
			nM:nM,
			mM:mM
		};
		for p in g.bomb_chunk.positions() {
			g.grid[*p]=BOMB;
		}
		g.count_bombs();
		g
	}

	pub fn update_first_real_line(&mut self, k:usize, prev: &Tile) {
		if self.grid[self.nM+k] == BOMB {
			return;
		}
		let kg=self.index_all(k,1);
		let kp=prev.index_all(k,self.mM-1);
		self.grid[kg] += prev.grid[kp]; 
	}

	pub fn merge(&mut self,prev:Option<&Tile>, next:Option<&Tile>) {
		let m=self.bomb_chunk.m();
		let n=self.bomb_chunk.n();
		let mM=m+2;
		let nM=n+2;
		match prev {
			Some(prev) => {
				// update first "real" line
				for k in 0..nM {
					self.update_first_real_line(k,prev);
				}
			}
			None => {}
		}
		match next {
			Some(next) => {
				// update last "real" line
				for k in 0..nM {
					if self.grid[(mM-2)*nM+k] != BOMB {
						self.grid[(mM-2)*nM+k] += next.grid[k];
					}
				}
			}
			None => {}
		}
	}

	pub fn print(&self, printer: &mut Printer) {
		let print_lookup: [u8;11] = [b' ',b'1',b'2',b'3',b'4',b'5',b'6',b'7',b'8',b'B',b' '];
		assert!(!self.grid.is_empty());
		let n=self.bomb_chunk.n();
		let m=self.bomb_chunk.m();
		let nM=n+2;
		let mM=m+2;
		let mut output:Vec<u8> = vec![b' ';n+2];
		output[n]=b'|';
		output[n+1]=b'\n';
		for km in 0..m {
			for kn in 0..n {
				let k=_1d((kn+1,km+1),nM,mM);
				output[kn]=print_lookup[self.grid[k]];
			}
			printer.print(&output);
		}
	}
	
	pub fn _print_all(&self, printer: &mut Printer) {
		let print_lookup: [u8;11] = [b' ',b'1',b'2',b'3',b'4',b'5',b'6',b'7',b'8',b'B',b' '];
		assert!(!self.grid.is_empty());
		let n=self.bomb_chunk.n();
		let m=self.bomb_chunk.m();
		let nM=n+2;
		let mM=m+2;
		let mut output:Vec<u8> = vec![b' ';nM+1];
		output[nM]=b'\n';
		for km in 0..mM {
			for kn in 0..nM {
				let k=_1d((kn,km),nM,mM);
				output[kn]=print_lookup[self.grid[k]];
			}
			printer.print(&output);
		}
	}

	fn increment_neighboors(&mut self, pos:usize) {
		let n=self.bomb_chunk.n();
		let m=self.bomb_chunk.m();
		let (posx,posy)=_2d(pos,n+2,m+2);
		assert!(posx>0 && posx<n+1);
		assert!(posy>0 && posy<m+1);
		for i in 0..3 {
			for j in 0..3 {
				if i == 1 && j == 1 {
					continue
				}
				let l=_1d((posx+i-1,posy+j-1),n+2,m+2);
				assert!(l<self.grid.len());
				if self.grid[l] != BOMB {
					self.grid[l]+=1;
				}
			}
		}
	}
	
	pub fn count_bombs(&mut self) {
		let positions=self.bomb_chunk.positions().clone();
		positions.iter().for_each(|bpos| {
			self.increment_neighboors(*bpos);
		}
		);
	}

	pub fn tile_index(&self) -> usize {
		self.bomb_chunk.index()
	}
}
