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
        for ca in self.candidates.get(a).unwrap() {
            for cb in self.candidates.get(b).unwrap() {
                if ca.bbox.intersect(&cb.bbox) {
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
    pub fn refresh_graph_from_candidates(&mut self) {
        self.map.clear();
        let nodes: Vec<_> = self.candidates.keys().cloned().collect();
        for node in nodes {
            self.compute_edges(&node);
        }
    }
    pub fn add_node(&mut self, a: Node, candidates: Candidates) {
        debug_assert!(!self.map.contains_key(&a));
        self.candidates.insert(a, candidates);
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
            Cb.retain(|cb| !selected.bbox.intersect(&cb.bbox));
        }
        // remove a
        self.candidates.remove(a);
        self.refresh_graph_from_candidates();

        // this is not enough:
        // we must remove b-c edges if there is no candidate backing the edge.
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
        assert!(ca2.bbox.intersect(&cb2.bbox));
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
        graph.refresh_graph_from_candidates();

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
