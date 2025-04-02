pub fn _2d(index:usize,n:usize,m:usize) -> (usize,usize) {
	assert!(index<(n*m));
	let x=index%n;
	let y=index/n;
	(x,y)
}

pub fn _1d(c:(usize,usize),n:usize,_m:usize) -> usize {
	c.1*n+c.0
}

#[cfg(test)]
mod tests {
	use super::*;
    #[test]
    fn test_2d() {
		let n=4;
		let m=2;

		let k = 6;
		let c = _2d(k,n,m);
		assert_eq!(c,(2,1));

		let k = 2;
		let c = _2d(k,n,m);
		assert_eq!(c,(2,0));
    }
}
