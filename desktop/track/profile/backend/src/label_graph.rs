use crate::label_candidates::Candidates;
use std::collections::HashMap;

type Node = usize;
type Edge = usize;
type Edges = Vec<Edge>;
type CandidateMap = HashMap<usize, Candidates>;

pub struct Graph {
    pub map: HashMap<Node, Edges>,
    pub candidates: CandidateMap,
}

impl Graph {
    pub fn new() -> Self {
        Self {
            map: HashMap::<Node, Edges>::new(),
            candidates: CandidateMap::new(),
        }
    }
    pub fn add_node(&mut self, node: Node) {
        debug_assert!(!self.map.contains_key(&node));
        self.map.insert(node, Edges::new());
    }
    pub fn add_edge(&mut self, start: usize, end: usize) {
        self.map.get_mut(&start).unwrap().push(end);
    }
    pub fn remove_node_deep(&mut self, a: &Node) {
        for b in self.map.get(a).unwrap() {
            let Cb = self.candidates.get(b).unwrap();
            let mut Cba = Candidates::new();
            for ca in self.candidates.get(a).unwrap() {
                for cb in Cb {
                    if ca.bbox.intersect(&cb.bbox) {
                        Cba.insert(cb.clone());
                    }
                }
            }
            let clean = Cb - &Cba;
            self.candidates.insert(*b, clean);
        }
        self.map.get_mut(a).unwrap().clear();
        self.map.remove(a);
    }
}
