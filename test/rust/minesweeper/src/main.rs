use std::env;

fn main() {
	let args: Vec<String> = env::args().collect();
	let n = args[1].parse::<i32>().unwrap();
	dbg!(n);
    dbg!(args);
}
