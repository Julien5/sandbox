pub fn _2d(index:usize,n:usize,m:usize) -> (usize,usize) {
	assert!(index<(n*m));
	let x=index%n;
	let y=index/n;
	(x,y)
}

pub fn _1d(c:(usize,usize),n:usize,_m:usize) -> usize {
	c.1*n+c.0
}

use std::io::BufWriter;
use std::io::Write;
use std::fs::File;
use std::os::unix::io::FromRawFd;

pub struct Printer {
	writer : Option<BufWriter<File>>,
}

impl Printer {
	pub fn new_verbose() -> Printer {
		let stdout=unsafe { File::from_raw_fd(1) };
		let p=Printer {
			writer : Some(BufWriter::new(stdout))
		};
		p
	}
	pub fn new_quiet() -> Printer {
		let p=Printer {
			writer : None
		};
		p
	}
	pub fn print(&mut self,line:&Vec<u8>) {
		match &mut self.writer {
			Some(writer) => {
				writer.write_all(line).unwrap();
				let _=writer.flush();
			}
			_ => {	}
		}
	}
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
