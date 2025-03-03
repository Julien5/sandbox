use std::env;

use rand::rng;
use rand::prelude::SliceRandom;

fn print_grid(grid:&Vec<bool>) {
	for (index,value) in grid.iter().enumerate() {
		//let (index,value) = e.enumerate();
		println!("grid[{}]={}",index,value);
	}
}

fn main() {
	let args: Vec<String> = env::args().collect();
	dbg!(&args);
	let n = args[1].parse::<usize>().unwrap();
	dbg!(n);
	let mut grid : Vec<bool> = vec![false; n*n];

	let b = args[2].parse::<usize>().unwrap();
	dbg!(b);

	// init 
	for k in 0..b {
		grid[k]=true;
	}

	print_grid(&grid);
	println!("** shuffle **");
	let mut rng = rng();
	grid.shuffle(&mut rng);
	print_grid(&grid);
}
