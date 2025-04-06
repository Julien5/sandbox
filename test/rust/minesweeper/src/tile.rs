use crate::bomb::*;
use crate::utils::*;

type Element = u8;

const _EMPTY : Element = 10;
const BOMB  : Element = 9;
const ZERO  : Element = 0;

pub struct Tile {
	grid: Vec<Element>,
	bomb_chunk: BombChunk,
}

impl Tile {
	fn at(&self, kn:usize, km:usize) -> Element {
		self.grid[_1d((kn+1,km+1),self.nM(),self.mM())]
	}

	fn at_all(&self, kn:usize, km:usize) -> Element {
		self.grid[_1d((kn,km),self.nM(),self.mM())]
	}
	
	pub fn with_chunk(chunk:BombChunk) -> Tile {
		log::trace!("make tile for chunk index:{}",chunk.index());
		let n=chunk.n();
		let m=chunk.m();
		let nM=n+2;
		let mM=m+2;
		let mut g=Tile {
			grid: vec![ZERO; nM*mM],
			bomb_chunk:chunk,
		};
		let L=g.bomb_chunk.positions().len();
		for p in 0..L {
			let pos=g.bomb_chunk.positions()[p];
			g.grid[pos]=BOMB;
			g.increment_neighboors(pos);
		}
		g
	}

	fn nM(&self) -> usize {
		self.n()+2
	}
	
	fn mM(&self) -> usize {
		self.m()+2
	}

	fn n(&self) -> usize {
		self.bomb_chunk.n()
	}
	
	fn m(&self) -> usize {
		self.bomb_chunk.m()
	}
		
	pub fn bomb_count_at(&self,kn:usize,km:usize,prev:Option<&Tile>, next:Option<&Tile>) -> u8 {
		if self.at(kn,km) == BOMB {
			return BOMB;
		}
		if km == 0 {
			// add last line from prev
			match prev {
				Some(tile) => {
					debug_assert!(tile.at_all(kn+1,tile.mM()-1) != BOMB);
					return self.at(kn,0) + tile.at_all(kn+1,tile.mM()-1);
				}
				_ => {}
			}
		}
		if km == self.m()-1 {
			// add first line from next
			match next {
				Some(tile) => {
					debug_assert!(tile.at_all(kn+1,0) != BOMB);
					return self.at(kn,self.m()-1) + tile.at_all(kn+1,0);
				}
				_ => {}
			}
			
		}
		self.at(kn,km)
	}

	pub fn print_bombs(&self, printer: &mut Printer) {
		let print_lookup: [u8;11] = [b' ',b' ',b' ',b' ',b' ',b' ',b' ',b' ',b' ',b'*',b' '];
		debug_assert!(!self.grid.is_empty());
		let mut output:Vec<u8> = vec![b' ';4*self.n()+2];
		output[0]=b'|';
		output[4*self.n()+1]=b'\n';
		for km in 0..self.m() {
			for kn in 0..self.n() {
				let k=_1d((kn+1,km+1),self.nM(),self.mM());
				output[4*kn+2]=print_lookup[self.grid[k] as usize];
				output[4*kn+4]=b'|';
			}
			printer.print(&output);
		}
	}

	pub fn print_counts(&self, prev:Option<&Tile>, next:Option<&Tile>, printer: &mut Printer) {
		let print_lookup: [u8;11] = [b'.',b'1',b'2',b'3',b'4',b'5',b'6',b'7',b'8',b'*',b' '];
		debug_assert!(!self.grid.is_empty());
		let mut output:Vec<u8> = vec![b' ';4*self.n()+2];
		output[0]=b'|';
		output[4*self.n()+1]=b'\n';
		for km in 0..self.m() {
			for kn in 0..self.n() {
				let c=self.bomb_count_at(kn,km,prev,next);
				output[4*kn+2]=print_lookup[c as usize];
				output[4*kn+4]=b'|';
			}
			printer.print(&output);
		}
	}
	
	pub fn _print_all(&self, printer: &mut Printer) {
		let print_lookup: [u8;11] = [b' ',b'1',b'2',b'3',b'4',b'5',b'6',b'7',b'8',b'B',b' '];
		debug_assert!(!self.grid.is_empty());
		let mut output:Vec<u8> = vec![b' ';self.nM()+1];
		output[self.nM()]=b'\n';
		for km in 0..self.mM() {
			for kn in 0..self.nM() {
				let k=_1d((kn,km),self.nM(),self.mM());
				output[kn]=print_lookup[self.grid[k] as usize];
			}
			printer.print(&output);
		}
	}

	fn increment_neighboors(&mut self, pos:usize) {
		let (posx,posy)=_2d(pos,self.nM(),self.mM());
		debug_assert!(posx>0 && posx<self.n()+1);
		debug_assert!(posy>0 && posy<self.m()+1);
		for i in 0..3 {
			for j in 0..3 {
				if i == 1 && j == 1 {
					continue
				}
				let l=_1d((posx+i-1,posy+j-1),self.nM(),self.mM());
				debug_assert!(l<self.grid.len());
				if self.grid[l] != BOMB {
					self.grid[l]+=1;
				}
			}
		}
	}
	
	pub fn _count_bombs(&mut self) {
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
