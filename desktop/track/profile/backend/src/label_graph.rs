use crate::label_candidates::Candidate;
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
    fn intersect(&self, a: &Node, b: &Node) -> bool {
        for ka in self.candidates.get(a).unwrap() {
            for kb in self.candidates.get(b).unwrap() {
                if ka.bbox.intersect(&kb.bbox) {
                    return true;
                }
            }
        }
        false
    }
    pub fn add_node(&mut self, a: Node, candidates: Candidates) {
        debug_assert!(!self.map.contains_key(&a));
        self.candidates.insert(a, candidates);
        let mut E = Edges::new();
        for b in self.map.keys().clone() {
            if self.intersect(&a, b) {
                E.push(*b);
            }
        }
        self.map.insert(a, E);
    }
    pub fn select(&mut self, a: &Node, selected: &Candidate) {
        // for all b connected to a
        let B = self.map.get(a).unwrap().clone();
        for b in B {
            // remove candidates of b that overlap with the
            // selected a candidate
            let Cb = self.candidates.get_mut(&b).unwrap();
            for k in 0..Cb.len() {
                let cb = &Cb[k];
                if selected.bbox.intersect(&cb.bbox) {
                    // TODO: this is slow if Cb is large.
                    Cb.remove(k);
                }
            }
            // remove the (b,a) edge
            let Eb = self.map.get_mut(&b).unwrap();
            Eb.retain(|&x| x != *a);
        }
        // remove a
        self.map.remove(a);
        self.candidates.remove(a);
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
}

#[cfg(test)]
mod tests {
    use crate::label_candidates::LabelBoundingBox;

    use super::*;

    #[test]
    fn test_graph_operations() {
        // Create a new graph
        let mut graph = Graph::new();

        // Add nodes and candidates (example placeholders)
        let candidates_a = Candidates::new(); // Replace with actual candidate data
        let candidates_b = Candidates::new(); // Replace with actual candidate data
        graph.add_node(1, candidates_a);
        graph.add_node(2, candidates_b);

        // Perform operations (example placeholders)
        let f = (0f64, 0f64);
        let bb = LabelBoundingBox::new_tlbr(f, f);
        let candidate = Candidate::new(bb, 0f64, 0f64); // Replace with actual candidate initialization
        graph.select(&1, &candidate);

        // Add assertions (example placeholders)
        assert!(graph.map.contains_key(&2));
        assert!(!graph.map.contains_key(&1));
    }
}
