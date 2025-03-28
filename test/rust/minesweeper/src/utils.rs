pub fn _2d(index:usize,n:usize) -> (usize,usize) {
	assert!(index<(n*n));
	let x=index%n;
	let y=index/n;
	(x,y)
}

pub fn _1d(c:(usize,usize),n:usize) -> usize {
	c.1*n+c.0
}
