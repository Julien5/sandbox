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


fn print_grid(grid:&[bool]) {
	let l=grid.len();
	assert!(!grid.is_empty());
	let n=f64::sqrt(grid.len() as f64) as usize;
	println!("grid len is {l} and the square is {n}");
	for k1 in 0..n {
		for k2 in 0..n {
			let k=from_2d((k1,k2),n);
			// println!("k1={k1} k2={k2} k={k}");
			if grid[k] {
				print!("| * ");
			} else {
				print!("|   ");
			}
			
		}
		println!("|");
	}
}

fn main() {
	let args: Vec<String> = env::args().collect();
	// dbg!(&args);
	let n = args[1].parse::<usize>().unwrap();
	dbg!(n);
	let mut grid : Vec<bool> = vec![false; n*n];

	let b = args[2].parse::<usize>().unwrap();
	dbg!(b);

	// init 
	for k in 0..b {
		grid[k]=true;
	}

	let mut rng = rng();
	grid.shuffle(&mut rng);
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
