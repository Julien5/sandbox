#![allow(non_snake_case)]

use crate::utils::{self, prepare_output};

fn to_2d(index:usize,n:usize) -> (usize,usize) {
	debug_assert!(index<(n*n));
	let x=index%n;
	let y=index/n;
	(x,y)
}

fn from_2d(c:(usize,usize),n:usize) -> usize {
	c.1*n+c.0
}

const BOMB  : usize = 9;
const ZERO  : usize = 0;

fn print_grid(grid:&[usize], n:usize, printer : &mut utils::Printer ) {
	let print_lookup: [u8;11] = [b' ',b'1',b'2',b'3',b'4',b'5',b'6',b'7',b'8',b'*',b' '];
	debug_assert!(!grid.is_empty());
	let mut output:Vec<u8> = prepare_output(n);
	for ky in 0..n {
		for kx in 0..n {
			let k=from_2d((kx,ky),n);
			output[4*kx+2]=print_lookup[grid[k] as usize];
		}
		printer.print(&output);
	}
}

fn distinct_random_numbers(N:usize,b:usize) -> Vec<usize> {
	// populate the available positions excluding the margins.
	let positions : Vec<usize>=(0..N).collect();
	utils::distinct_random_numbers(positions,b)
}

fn increment_neighboors(grid:&mut [usize], nu:usize, pos:usize) {
	let (posxu,posyu)=to_2d(pos,nu);
	let n=nu as i64;
	let (posx,posy)=(posxu as i64, posyu as i64);
	for dx in [-1,0,1] {
		let posnx=posx+dx;
		if posnx<0 || posnx>=n {
			continue;
		}
		for dy in [-1,0,1] {
			if dx == 0 && dy == 0 {
				continue
			}
			let posny=posy+dy;
			if posny<0 || posny>=n {
				continue;
			}
			let l=posny*n + posnx;
			let lu=l as usize;
			if grid[lu] != BOMB {
				grid[lu]+=1;
			}
		}
	}
}

fn count_bombs(grid:&mut [usize], n:usize,bombs_positions:&[usize]) {
	for bpos in bombs_positions {
		increment_neighboors(grid,n,*bpos);
	}
}

pub fn main(n : usize, b: usize, quiet: bool) {
	let N = n*n;
	let mut grid : Vec<usize> = vec![ZERO; N];
	log::info!("make bombs");
	let Bx = distinct_random_numbers(N,b);
	for p in &Bx {
		grid[*p]=BOMB;
	}

	let mut printer=utils::make_printer(quiet);
	if ! quiet {
		log::info!("print");
		print_grid(&grid,n,&mut printer);
	}
	log::info!("count bombs");
	count_bombs(&mut grid,n,&Bx);
	
	if ! quiet {
		log::trace!("print");
		print_grid(&grid,n,&mut printer);
	}
	log::info!("done");
}

