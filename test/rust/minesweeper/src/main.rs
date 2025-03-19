#![allow(non_snake_case)]

use std::env;

use rand::rng;
use rand::prelude::SliceRandom;

fn to_2d(index:usize,n:usize) -> (usize,usize) {
	assert!(index<(n*n));
	let x=index%n;
	let y=index/n;
	(x,y)
}

fn from_2d(c:(usize,usize),n:usize) -> usize {
	c.1*n+c.0
}

const empty : usize = 10;
const bomb  : usize = 9;
const zero  : usize = 0;

fn print_grid(grid:&[usize]) {
	let print_lookup: [char;11] = [' ','1','2','3','4','5','6','7','8','B',' '];
	let l=grid.len();
	assert!(!grid.is_empty());
	let n=f64::sqrt(grid.len() as f64) as usize;
	println!("grid len is {l} and the square is {n}");
	for k1 in 0..n {
		for k2 in 0..n {
			let k=from_2d((k1,k2),n);
			//print!("| {:2} ",grid[k]);
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
	assert_eq!(G.len(),b);
	G
}

fn increment_neighboors(grid:&mut [usize], pos:usize) {
	let n=f64::sqrt(grid.len() as f64) as usize;
	let (posx,posy)=to_2d(pos,n);
	for i in 0..3 {
		if posx+i<1 {
			continue;
		}
		for j in 0..3 {
			if i == 1 && j == 1 {
				continue
			}
			if posy+j<1 {
				continue;
			}
			let l=from_2d((posx+i-1,posy+j-1),n);
			if l<grid.len() && grid[l] != bomb {
				grid[l]+=1;
			}
		}
	}
}

fn count_bombs(grid:&mut [usize], bombs_positions:&[usize]) {
	for bpos in bombs_positions {
		increment_neighboors(grid,*bpos);
	}
}

fn main() {
	let args: Vec<String> = env::args().collect();
	// dbg!(&args);
	let n = args[1].parse::<usize>().unwrap();
	let N = n*n;
	dbg!(n);
	let mut grid : Vec<usize> = vec![zero; N];

	let b = args[2].parse::<usize>().unwrap();
	dbg!(b);

	let Bx = distinct_random_numbers(N,b);
	//let Bx = [12];
	for p in &Bx {
		grid[*p]=bomb;
	}

	print_grid(&grid);
	count_bombs(&mut grid,&Bx);
	print_grid(&grid);
}

#[cfg(test)]
mod tests {
	use super::*;
    #[test]
    fn test_2d() {
		let c = to_2d(0_usize,4);
		assert_eq!(c,(0,0));

		let c = to_2d(1_usize,4);
		assert_eq!(c,(1,0));

		let c = to_2d(4_usize,4);
		assert_eq!(c,(0,1));
		
		let c = to_2d(15_usize,4);
		assert_eq!(c,(3,3));
    }
}
