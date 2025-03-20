#![allow(non_snake_case)]

use std::env;

use rand::rng;
use rand::prelude::SliceRandom;

fn _2d(index:usize,n:usize) -> (usize,usize) {
	assert!(index<(n*n));
	let x=index%n;
	let y=index/n;
	(x,y)
}

fn _1d(c:(usize,usize),n:usize) -> usize {
	c.1*n+c.0
}

type Element = usize;

const _EMPTY : Element = 10;
const BOMB  : Element = 9;
const ZERO  : Element = 0;

fn distinct_random_numbers(n:usize,b:usize) -> Vec<usize> {
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

struct Grid {
	n: usize,
	grid: Vec<Element>,
	bomb_positions: Vec<usize>
}


impl Grid {
	fn with_bombs(n:usize, b:usize) -> Grid {
		let mut g=Grid {
			n:n,
			bomb_positions:distinct_random_numbers(n,b),
			grid: vec![ZERO; (n+2)*(n+2)]
		};
		for p in &g.bomb_positions {
			g.grid[*p]=BOMB;
		}
		g
	}

	fn increment_neighboors(&mut self, pos:usize) {
		let (posx,posy)=_2d(pos,self.n+2);
		assert!(posx>0 && posx<self.n+1);
		assert!(posy>0 && posy<self.n+1);
		for i in 0..3 {
			for j in 0..3 {
				if i == 1 && j == 1 {
					continue
				}
				let l=_1d((posx+i-1,posy+j-1),self.n+2);
				assert!(l<self.grid.len());
				if self.grid[l] != BOMB {
					self.grid[l]+=1;
				}
			}
		}
	}
	
	fn count_bombs(&mut self) {
		let positions=self.bomb_positions.clone();
		for bpos in positions { 
			self.increment_neighboors(bpos);
		}
	}
}

// to stdout efficiency
use std::{
    fs::File,
    io::{BufWriter, Write},
    os::unix::io::FromRawFd,
};

fn print_grid(grid:&Grid, writer: &mut BufWriter<File>) {
	let print_lookup: [u8;11] = [b' ',b'1',b'2',b'3',b'4',b'5',b'6',b'7',b'8',b'B',b' '];
	assert!(!grid.grid.is_empty());
	let mut output:Vec<u8> = vec![b' ';grid.n+1];
	output[grid.n]=b'\n';
	for k1 in 0..grid.n {
		for k2 in 0..grid.n {
			let k=_1d((k1+1,k2+1),grid.n+2);
			output[k2]=print_lookup[grid.grid[k]];
		}
		writer.write_all(&output).unwrap();
	}
}


fn main() {
	let args: Vec<String> = env::args().collect();

	let stdout = unsafe { File::from_raw_fd(1) };
	let mut writer = BufWriter::new(stdout);
	
	let quiet : bool = args[1].contains("quiet");
	let n = args[2].parse::<usize>().unwrap();
	let b = args[3].parse::<usize>().unwrap();
	
	let mut grid = Grid::with_bombs(n,b);
	if quiet == false {
		print_grid(&grid,&mut writer);
	}
	grid.count_bombs();
	if quiet == false {
		print_grid(&grid,&mut writer);
	}
}

#[cfg(test)]
mod tests {
	use super::*;
    #[test]
    fn test_2d() {
		let c = _2d(0_usize,4);
		assert_eq!(c,(0,0));

		let c = _2d(1_usize,4);
		assert_eq!(c,(1,0));

		let c = _2d(4_usize,4);
		assert_eq!(c,(0,1));
		
		let c = _2d(15_usize,4);
		assert_eq!(c,(3,3));
    }
}
