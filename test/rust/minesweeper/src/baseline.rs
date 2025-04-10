#![allow(non_snake_case)]

use rand::rng;
use rand::prelude::SliceRandom;

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

fn print_grid(grid:&[usize]) {
	let print_lookup: [char;11] = [' ','1','2','3','4','5','6','7','8','*',' '];
	debug_assert!(!grid.is_empty());
	let n=f64::sqrt(grid.len() as f64) as usize;
	for ky in 0..n {
		for kx in 0..n {
			let k=from_2d((kx,ky),n);
			print!("| {} ",print_lookup[grid[k]]);
		}
		println!("|");
	}
}

fn distinct_random_numbers(N:usize,b:usize) -> Vec<usize> {
	// generates [0,1,...,N-1]
	let mut G : Vec<usize>=(0usize..N).collect();
	let mut rng = rng();
	// shuffle it and keep the first b elements.
	G.shuffle(&mut rng);
	G.truncate(b);
	debug_assert_eq!(G.len(),b);
	G
}

fn increment_neighboors(grid:&mut [usize], pos:usize) {
	let nu=f64::sqrt(grid.len() as f64) as usize;
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

fn count_bombs(grid:&mut [usize], bombs_positions:&[usize]) {
	for bpos in bombs_positions {
		increment_neighboors(grid,*bpos);
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
	
	if ! quiet {
		log::info!("print");
		print_grid(&grid);
	}
	log::info!("count bombs");
	count_bombs(&mut grid,&Bx);
	
	if ! quiet {
		log::trace!("print");
		print_grid(&grid);
	}
	log::info!("done");
}

