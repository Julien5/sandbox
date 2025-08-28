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
        for b in &E {
            self.map.get_mut(b).unwrap().push(a);
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
            Cb.retain(|cb| !selected.bbox.intersect(&cb.bbox));
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
            println!("node: {} edges:{}", node, list);
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
        let ca = make_candidate(2, 2, 3, 2);
        let cb = make_candidate(4, 2, 3, 2);
        assert!(ca.bbox.intersect(&cb.bbox));
        CA.push(ca);
        CB.push(cb);
        CC.push(make_candidate(3, 3, 2, 3));
        let c2 = make_candidate(4, 3, 2, 3);
        CC.push(c2.clone());
        CC.push(make_candidate(3, 8, 2, 3));
        CD.push(make_candidate(3, 9, 2, 3));
        graph.add_node(0, CA);
        graph.add_node(1, CB);
        graph.add_node(2, CC);
        graph.add_node(3, CD);

        graph.print();
        println!("max node {}", graph.max_node());
        println!("select {} {}", 1, "c2");
        graph.select(&1, &c2);
        graph.print();
        println!("max node {}", graph.max_node());
    }
}
