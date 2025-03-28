use crate::bomb::*;
use crate::utils::*;
use std::io::BufWriter;
use std::io::Write;
use std::fs::File;

type Element = usize;

const _EMPTY : Element = 10;
const BOMB  : Element = 9;
const ZERO  : Element = 0;

pub struct Tile {
	grid: Vec<Element>,
	bomb_chunk: BombChunk,
}

impl Tile {
	pub fn with_chunk(chunk:BombChunk) -> Tile {
		let mut g=Tile {
			grid: vec![ZERO; (chunk.n()+2)*(chunk.n()+2)],
			bomb_chunk:chunk
		};
		for p in g.bomb_chunk.positions() {
			g.grid[*p]=BOMB;
		}
		g
	}

	pub fn print(&self, writer: &mut BufWriter<File>) {
		let print_lookup: [u8;11] = [b' ',b'1',b'2',b'3',b'4',b'5',b'6',b'7',b'8',b'B',b' '];
		assert!(!self.grid.is_empty());
		let n=self.bomb_chunk.n();
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
		let n=self.bomb_chunk.n();
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
	
	pub fn count_bombs(&mut self) {
		let positions=self.bomb_chunk.positions().clone();
		positions.iter().for_each(|bpos| {
			self.increment_neighboors(*bpos);
		}
		);
	}

	pub fn index(&self) -> usize {
		self.bomb_chunk.index()
	}
}
