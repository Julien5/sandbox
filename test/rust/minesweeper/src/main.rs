#![allow(non_snake_case)]

use std::env;

use rand::rng;
use rand::prelude::SliceRandom;

#[cfg(test)]
type Result<T> = std::result::Result<T, Error>;

#[cfg(test)]
#[derive(Debug, Clone)]
struct Error;

#[cfg(test)]
fn to_2d(index:usize,n:usize) -> Result<(usize,usize)> {
	if index>(n*n) {
		return Err(Error);
	}
	let x=index%n;
	let y=index/n;
	Ok((x,y))
}

fn from_2d(c:(usize,usize),n:usize) -> usize {
	c.1*n+c.0
}

fn print_grid(grid:&[char]) {
	let l=grid.len();
	assert!(!grid.is_empty());
	let n=f64::sqrt(grid.len() as f64) as usize;
	println!("grid len is {l} and the square is {n}");
	for k1 in 0..n {
		for k2 in 0..n {
			let k=from_2d((k1,k2),n);
			// println!("k1={k1} k2={k2} k={k}");
			print!("| {} ",grid[k]);
			
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

fn main() {
	let args: Vec<String> = env::args().collect();
	// dbg!(&args);
	let n = args[1].parse::<usize>().unwrap();
	let N = n*n;
	dbg!(n);
	let mut grid : Vec<char> = vec![' '; N];

	let b = args[2].parse::<usize>().unwrap();
	dbg!(b);

	for p in distinct_random_numbers(N,b) {
		grid[p]='B';
	}

	print_grid(&grid);
}

#[cfg(test)]
mod tests {
	use super::*;
    #[test]
    fn test_2d() -> std::result::Result<(), Error> {
		let c = to_2d(0_usize,4)?;
		assert_eq!(c,(0,0));

		let c = to_2d(1_usize,4)?;
		assert_eq!(c,(1,0));

		let c = to_2d(4_usize,4)?;
		assert_eq!(c,(0,1));
		
		let c = to_2d(15_usize,4)?;
		assert_eq!(c,(3,3));
		
		Ok(())		
    }
}
