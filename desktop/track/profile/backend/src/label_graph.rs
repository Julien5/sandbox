use crate::label_candidates::Candidate;
use crate::label_candidates::Candidates;
use std::cmp::Ordering;
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
    fn intersect(&self, a: &Node, b: &Node) -> bool {
        for ca in self.candidates.get(a).unwrap() {
            for cb in self.candidates.get(b).unwrap() {
                if ca.bbox.overlap(&cb.bbox) {
                    return true;
                }
            }
        }
        false
    }
    fn compute_edges(&mut self, a: &Node) {
        let mut E = Edges::new();
        for b in self.candidates.keys().clone() {
            if b == a {
                continue;
            }
            if self.intersect(&a, b) {
                E.push(*b);
            }
        }
        self.map.insert(*a, E);
    }
    pub fn build_map(&mut self) {
        self.map.clear();
        // O(n^2) if there are fixed number of candidates.
        let nodes: Vec<_> = self.candidates.keys().cloned().collect();
        for node in nodes {
            self.compute_edges(&node);
        }
    }
    pub fn add_node(&mut self, a: Node, candidates: Candidates) {
        debug_assert!(!self.map.contains_key(&a));
        self.candidates.insert(a, candidates);
    }

    fn remove_node(&mut self, a: &Node) {
        self.candidates.remove(a);
        self.build_map();
    }

    pub fn select(&mut self, a: &Node, selected: &Candidate) {
        // for all b connected to a
        assert!(self
            .candidates
            .get(a)
            .unwrap()
            .iter()
            .position(|c| c == selected)
            .is_some());
        let B = self.map.get(a).unwrap().clone();
        for b in B {
            // remove candidates of b that overlap with the
            // selected a candidate
            let Cb = self.candidates.get_mut(&b).unwrap();
            Cb.retain(|cb| !selected.bbox.overlap(&cb.bbox));
        }
        // remove a
        self.remove_node(a);
    }
    pub fn max_node(&self) -> Node {
        *self
            .map
            .iter()
            .map(|(node, edges)| (node, edges.len()))
            .max_by_key(|(_node, len)| *len)
            .unwrap()
            .0
    }

    pub fn print(&self) {
        let mut nodes: Vec<_> = self.map.keys().collect();
        nodes.sort(); // Sort the keys in ascending order

        for node in nodes {
            let edges = &self.map[node];
            let list = edges
                .iter()
                .map(|x| x.to_string())
                .collect::<Vec<_>>()
                .join(", ");
            match self.candidates.get(node) {
                Some(candidates) => {
                    println!("node: {:1} edges:{:5} |C|={}", node, list, candidates.len())
                }
                None => println!("node: {:1} edges:{:5} |C|=0", node, list),
            };
        }
    }

    pub fn candidate_blocks_other(
        &self,
        node: &Node,
        candidate_index: usize,
        other: &Node,
    ) -> bool {
        let this_candidate = &self.candidates.get(node).unwrap()[candidate_index];
        let other_candidates = &self.candidates.get(other).unwrap();
        let other_has_label = !other_candidates.is_empty();
        if !other_has_label {
            return false;
        }
        for k in 0..other_candidates.len() {
            let other_candidate = &other_candidates[k];
            if !other_candidate.bbox.overlap(&this_candidate.bbox) {
                return false;
            }
        }
        true
    }

    pub fn candidate_blocks_any(&self, node: &Node, candidate_index: usize) -> bool {
        let mut nodes: Vec<_> = self.map.keys().collect();
        for other in nodes {
            if other == node {
                continue;
            }
            if self.candidate_blocks_other(node, candidate_index, other) {
                return true;
            }
        }
        false
    }

    pub fn debug(&self) -> Vec<(Node, Edges)> {
        let mut nodes: Vec<_> = self.map.keys().collect();
        nodes.sort(); // Sort the keys in ascending order
        let mut sorted: Vec<_> = nodes
            .iter()
            .map(|n| (**n, self.map.get(n).unwrap().clone()))
            .collect();
        sorted.sort_by(|(n1, E1), (n2, E2)| E1.len().cmp(&E2.len()));
        sorted
    }

    pub fn solve(&mut self) -> HashMap<Node, Candidate> {
        let mut ret = HashMap::new();
        while !self.map.is_empty() {
            let m = self.max_node();
            match self.best_candidate_for_node(&m) {
                Some(best_index) => {
                    let best_candidate = self.candidates.get(&m).unwrap()[best_index].clone();
                    ret.insert(m, best_candidate.clone());
                    self.select(&m, &best_candidate);
                }
                None => {
                    self.remove_node(&m);
                }
            }
        }
        ret
    }

    pub fn best_candidate_for_node(&self, node: &Node) -> Option<usize> {
        match self.candidates.get(node) {
            Some(candidates) => {
                if candidates.is_empty() {
                    println!("{node} has no candidate.");
                    return None;
                }
                let mut sorted: Vec<_> = (0..candidates.len()).collect();
                sorted.sort_by(|i, j| {
                    let ci = &candidates[*i];
                    let cj = &candidates[*j];
                    ci.partial_cmp(cj).unwrap_or(Ordering::Equal)
                });
                for index in 0..sorted.len() {
                    if self.candidate_blocks_any(node, index) {
                        continue;
                    }
                    return Some(index);
                }
                println!("all candidates of {node} block some other.");
                None
            }
            _ => {
                println!("{node} has no candidate.");
                None
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use crate::label_candidates::LabelBoundingBox;

    use super::*;

    fn make_candidate(x: i32, y: i32, w: i32, h: i32) -> Candidate {
        Candidate::new(
            LabelBoundingBox::new_tlwh((x as f64, y as f64), w as f64, h as f64),
            0.,
            0.,
        )
    }

    #[test]
    fn test_graph_operations() {
        // Create a new graph
        let mut graph = Graph::new();
        let A = 0;
        let mut CA = Candidates::new();
        let mut CB = Candidates::new();
        let mut CC = Candidates::new();
        let mut CD = Candidates::new();
        let ca1 = make_candidate(0, 0, 2, 2);
        let ca2 = make_candidate(2, 2, 3, 2);
        let cb1 = make_candidate(1, 0, 3, 2);
        let cb2 = make_candidate(4, 2, 3, 2);
        assert!(ca2.bbox.overlap(&cb2.bbox));
        CA.push(ca1);
        CA.push(ca2);
        CB.push(cb1.clone());
        CB.push(cb2.clone());
        let cc1 = make_candidate(3, 3, 2, 3);
        CC.push(cc1.clone());
        let cc2 = make_candidate(4, 3, 2, 3);
        CC.push(cc2.clone());
        CC.push(make_candidate(3, 8, 2, 3));
        CD.push(make_candidate(3, 9, 2, 3));
        graph.add_node(0, CA);
        graph.add_node(1, CB);
        graph.add_node(2, CC);
        graph.add_node(3, CD);
        graph.build_map();

        graph.print();
        assert!(graph.max_node() == 2);
        println!("select {} {}", 1, "cc1");
        graph.select(&2, &cc1);
        graph.print();
        assert!(!graph.map.contains_key(&2));
        assert!(graph.candidates.get(&0).unwrap().len() == 1);
        assert!(graph.candidates.get(&1).unwrap().len() == 1);
        assert!(graph.candidates.get(&3).unwrap().len() == 1);
        assert!(graph.map.get(&0).unwrap().len() == 1);
        assert!(graph.map.get(&1).unwrap().len() == 1);
        println!("max node {}", graph.max_node());
    }
}
