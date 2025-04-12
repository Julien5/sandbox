use crate::bomb::*;
use crate::utils::*;

type Element = u8;

const _EMPTY : Element = 10;
const BOMB  : Element = 9;
const ZERO  : Element = 0;

pub struct Tile {
	pub grid: Vec<Element>,
	bomb_chunk: BombChunk,
}

pub fn merge(tile1: &mut Tile, tile2: &mut Tile) {
	log::trace!("merging {} and {}",tile1.tile_index(),tile2.tile_index());
	debug_assert!(tile1.tile_index()<tile2.tile_index());
	for kx in 0..tile1.X() {
		let ktop=_1d((kx+1,1),tile1.LX(),tile1.LY());
		let kbot=_1d((kx+1,tile1.LY()-2),tile1.LX(),tile1.LY());
		if tile1.grid[kbot] != BOMB {
			debug_assert!(tile2.at_all(kx+1,0)!=BOMB);
			tile1.grid[kbot]+=tile2.at_all(kx+1,0);
		}
		if tile2.grid[ktop] != BOMB {
			debug_assert!(tile1.at_all(kx+1,tile1.LY()-1)!=BOMB);
			tile2.grid[ktop]+=tile1.at_all(kx+1,tile1.LY()-1);
		}
	}
}

impl Tile {
	fn at(&self, x:usize, y:usize) -> Element {
		self.grid[_1d((x+1,y+1),self.LX(),self.LY())]
	}

	fn at_all(&self, x:usize, y:usize) -> Element {
		self.grid[_1d((x,y),self.LX(),self.LY())]
	}
	
	pub fn with_chunk(chunk:BombChunk) -> Tile {
		log::trace!("make tile for chunk index:{}",chunk.index());
		let LX=chunk.X()+2;
		let LY=chunk.Y()+2;
		let mut g=Tile {
			grid: vec![ZERO; LX*LY],
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

	fn LX(&self) -> usize {
		self.X()+2
	}
	
	fn LY(&self) -> usize {
		self.Y()+2
	}

	fn X(&self) -> usize {
		self.bomb_chunk.X()
	}
	
	fn Y(&self) -> usize {
		self.bomb_chunk.Y()
	}
	
	pub fn print_bombs(&self, printer: &mut Printer) {
		let print_lookup: [u8;11] = [b' ',b' ',b' ',b' ',b' ',b' ',b' ',b' ',b' ',b'*',b' '];
		debug_assert!(!self.grid.is_empty());
		let mut output:Vec<u8> = prepare_output(self.X());
		for ky in 0..self.Y() {
			for kx in 0..self.X() {
				let k=_1d((kx+1,ky+1),self.LX(),self.LY());
				output[4*kx+2]=print_lookup[self.grid[k] as usize];
			}
			printer.print(&output);
		}
	}

	pub fn print_counts(&self,printer: &mut Printer) {
		let print_lookup: [u8;11] = [b'.',b'1',b'2',b'3',b'4',b'5',b'6',b'7',b'8',b'*',b' '];
		debug_assert!(!self.grid.is_empty());
		let mut output:Vec<u8> = prepare_output(self.X());
		for ky in 0..self.Y() {
			for kx in 0..self.X() {
				output[4*kx+2]=print_lookup[self.at(kx,ky) as usize];
			}
			printer.print(&output);
		}
	}
	
	pub fn _print_all(&self) {
		let print_lookup: [u8;11] = [b' ',b'1',b'2',b'3',b'4',b'5',b'6',b'7',b'8',b'B',b' '];
		debug_assert!(!self.grid.is_empty());
		let mut output:Vec<u8> = vec![b' ';self.LX()+1];
		output[self.LX()]=b'\n';
		for ky in 0..self.LY() {
			for kx in 0..self.LX() {
				let k=_1d((kx,ky),self.LX(),self.LY());
				output[kx]=print_lookup[self.grid[k] as usize];
			}
			print!("{}",String::from_utf8(output.clone()).unwrap());
		}
	}

	fn increment_neighboors(&mut self, pos:usize) {
		let (posx,posy)=_2d(pos,self.LX(),self.LY());
		debug_assert!(posx>0 && posx<self.X()+1);
		debug_assert!(posy>0 && posy<self.Y()+1);
		for i in 0..3 {
			for j in 0..3 {
				if i == 1 && j == 1 {
					continue
				}
				let l=_1d((posx+i-1,posy+j-1),self.LX(),self.LY());
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
