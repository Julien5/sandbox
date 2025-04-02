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
		_1d((kn+1,km+1),self.nM,self.mM)
	}

	fn index_all(&self, kn:usize, km:usize) -> usize {
		_1d((kn,km),self.nM,self.mM)
	}
	
	fn at(&self, kn:usize, km:usize) -> Element {
		self.grid[_1d((kn+1,km+1),self.nM,self.mM)]
	}

	fn at_all(&self, kn:usize, km:usize) -> Element {
		self.grid[_1d((kn,km),self.nM,self.mM)]
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
		let kg=self.index_all(k,1);
		assert_eq!(kg,self.nM+k);
		if self.grid[kg] == BOMB {
			return;
		}
		let kp=prev.index_all(k,self.mM-1);
		self.grid[kg] += prev.grid[kp]; 
	}

	pub fn update_bottom_real_line(&mut self, k:usize, next: &Tile) {
		let kg=self.index_all(k,self.mM-2);
		assert_eq!(kg,(self.mM-2)*self.nM+k);
		if self.grid[kg] == BOMB {
			return;
		}
		let kn=next.index_all(k,0);
		assert_eq!(kn,k);
		self.grid[kg] += next.grid[kn];
	}

	pub fn merge(&mut self,prev:Option<&Tile>, next:Option<&Tile>) {
		match prev {
			Some(prev) => {
				// update first "real" line
				for k in 0..self.nM {
					self.update_first_real_line(k,prev);
				}
			}
			None => {}
		}
		match next {
			Some(next) => {
				// update last "real" line
				for k in 0..self.nM {
					self.update_bottom_real_line(k,next);
				}
			}
			None => {}
		}
	}

	pub fn print(&self, printer: &mut Printer) {
		let print_lookup: [u8;11] = [b' ',b'1',b'2',b'3',b'4',b'5',b'6',b'7',b'8',b'B',b' '];
		assert!(!self.grid.is_empty());
		let mut output:Vec<u8> = vec![b' ';self.n+2];
		output[self.n]=b'|';
		output[self.n+1]=b'\n';
		for km in 0..self.m {
			for kn in 0..self.n {
				let k=_1d((kn+1,km+1),self.nM,self.mM);
				output[kn]=print_lookup[self.grid[k]];
			}
			printer.print(&output);
		}
	}
	
	pub fn _print_all(&self, printer: &mut Printer) {
		let print_lookup: [u8;11] = [b' ',b'1',b'2',b'3',b'4',b'5',b'6',b'7',b'8',b'B',b' '];
		assert!(!self.grid.is_empty());
		let mut output:Vec<u8> = vec![b' ';self.nM+1];
		output[self.nM]=b'\n';
		for km in 0..self.mM {
			for kn in 0..self.nM {
				let k=_1d((kn,km),self.nM,self.mM);
				output[kn]=print_lookup[self.grid[k]];
			}
			printer.print(&output);
		}
	}

	fn increment_neighboors(&mut self, pos:usize) {
		let (posx,posy)=_2d(pos,self.nM,self.mM);
		assert!(posx>0 && posx<self.n+1);
		assert!(posy>0 && posy<self.m+1);
		for i in 0..3 {
			for j in 0..3 {
				if i == 1 && j == 1 {
					continue
				}
				let l=_1d((posx+i-1,posy+j-1),self.nM,self.mM);
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
