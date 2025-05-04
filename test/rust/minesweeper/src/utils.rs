use rand::rngs::StdRng;

pub fn _2d(index: usize, n: usize, m: usize) -> (usize, usize) {
    debug_assert!(index < (n * m));
    let x = index % n;
    let y = index / n;
    (x, y)
}

pub fn _1d(c: (usize, usize), n: usize, _m: usize) -> usize {
    c.1 * n + c.0
}

use std::fs::File;
use std::io::BufWriter;
use std::io::Write;
use std::os::unix::io::FromRawFd;

pub struct Printer {
    writer: Option<BufWriter<File>>,
}

impl Printer {
    pub fn new_verbose() -> Printer {
        let stdout = unsafe { File::from_raw_fd(1) };
        Printer {
            writer: Some(BufWriter::new(stdout)),
        }
    }
    pub fn new_quiet() -> Printer {
        Printer { writer: None }
    }
    pub fn print(&mut self, line: &[u8]) {
        if let Some(writer) = &mut self.writer {
            writer.write_all(line).unwrap();
            let _ = writer.flush();
        }
    }
}

pub fn make_printer(quiet: bool) -> Printer {
    match quiet {
        true => Printer::new_quiet(),
        false => Printer::new_verbose(),
    }
}

use rand::{Rng, SeedableRng};

struct Random {
    xn: usize,
    params: (usize, usize, usize),
    rng: rand::rngs::StdRng,
}

impl Random {
    pub fn new(rng: rand::rngs::StdRng) -> Random {
        Random {
            xn: 0,
            params: (1664525usize, 1013904223usize, (2usize).pow(32)),
            rng: rng,
        }
    }
    fn range(&mut self, begin: usize, end: usize) -> usize {
        match std::env::var("TEST") {
            Ok(_) => {
                let d = end - begin;
                let (a, b, modulus) = self.params;
                self.xn = (a * self.xn + b) % modulus;
                return (begin + self.xn) % d;
            }
            _ => {}
        }
        self.rng.random_range(0..end)
    }
}

pub fn fisher_yates_shuffle(mut positions: Vec<usize>, B: usize) -> Vec<usize> {
    let mut random = Random::new(StdRng::seed_from_u64(3));
    let L = positions.len();
    for i in 0..B {
        let end = L - i;
        let j = random.range(0, end);
        positions.swap(j, end - 1);
    }
    let mut G: Vec<usize> = vec![0; B];
    let end = L;
    for i in 0..B {
        G[i] = positions[end - i - 1];
    }
    G
}

pub fn prepare_output(n: usize) -> Vec<u8> {
    let mut output: Vec<u8> = vec![b' '; 4 * n + 2];
    output[0] = b'|';
    output[4 * n + 1] = b'\n';
    for kn in 0..n {
        output[4 * kn + 4] = b'|';
    }
    output
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn test_2d() {
        let n = 4;
        let m = 2;

        let k = 6;
        let c = _2d(k, n, m);
        debug_assert_eq!(c, (2, 1));

        let k = 2;
        let c = _2d(k, n, m);
        debug_assert_eq!(c, (2, 0));
    }
    #[test]
    fn b1() {
        let mut rng = StdRng::from_os_rng();
        let B = 10;
        let N = 10;
        let mut set = std::collections::HashSet::new();
        while set.len() != B {
            set.insert(rng.random_range(0..N * N));
        }
        assert_eq!(set.len(), B);
        for k in set {
            println!("{}", k);
        }
    }
}
