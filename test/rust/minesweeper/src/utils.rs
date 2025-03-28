pub fn _2d(index:usize,n:usize) -> (usize,usize) {
	assert!(index<(n*n));
	let x=index%n;
	let y=index/n;
	(x,y)
}

pub fn _1d(c:(usize,usize),n:usize) -> usize {
	c.1*n+c.0
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
