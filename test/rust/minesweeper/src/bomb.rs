use crate::utils;

type BombPositions = Vec<usize>;

pub struct BombChunk {
    X: usize,
    Y: usize,
    index: usize,
    positions: BombPositions,
}

fn distinct_random_numbers(n: usize, m: usize, b: usize) -> BombPositions {
    // populate the available positions excluding the margins.
    let mut positions: Vec<usize> = vec![0; n * m];
    let mut k = 0;
    for j in 0..m {
        for i in 0..n {
            positions[k] = (j + 1) * (n + 2) + (i + 1);
            k += 1;
        }
    }
    utils::fisher_yates_shuffle(positions, b) as BombPositions
}

impl BombChunk {
    pub fn with_bomb_count(X: usize, Y: usize, index: usize, b: usize) -> BombChunk {
        log::trace!(
            "[{:?}] make bombs for chunk index:{}",
            std::thread::current().id(),
            index
        );
        BombChunk {
            X,
            Y,
            index,
            positions: distinct_random_numbers(X, Y, b),
        }
    }
    pub fn positions(&self) -> &BombPositions {
        &self.positions
    }
    pub fn X(&self) -> usize {
        self.X
    }
    pub fn Y(&self) -> usize {
        self.Y
    }
    pub fn index(&self) -> usize {
        self.index
    }
}
