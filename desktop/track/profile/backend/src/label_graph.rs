use crate::label_candidates::Candidates;
use std::collections::HashMap;

type Node = usize;
type Edge = usize;
type Edges = Vec<Edge>;
type CandidateMap = HashMap<usize, Candidates>;

pub struct Graph {
    pub graph: HashMap<Node, Edges>,
    pub candidates: CandidateMap,
}

impl Graph {
    pub fn new() -> Self {
        Self {
            graph: HashMap::<Node, Edges>::new(),
            candidates: CandidateMap::new(),
        }
    }
}
