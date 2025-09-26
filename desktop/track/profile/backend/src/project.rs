#![allow(non_snake_case)]

use crate::track::Track;
use crate::waypoint;
use crate::waypoint::WGS84Point;
use crate::waypoint::Waypoints;
use sphere_knn::SphereKnnGetters;

#[derive(Clone)]
struct IndexedWGS84Point {
    pub wgs84: WGS84Point,
    pub index: usize,
}

impl std::fmt::Debug for IndexedWGS84Point {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("IndexWGS84Point")
            .field("lon", &self.wgs84.longitude())
            .field("lat", &self.wgs84.latitude())
            .field("index", &self.index)
            .finish()
    }
}

impl SphereKnnGetters for IndexedWGS84Point {
    fn get_lat(&self) -> f64 {
        self.wgs84.latitude()
    }
    fn get_lng(&self) -> f64 {
        self.wgs84.longitude()
    }
}

fn convert(track: &Track) -> Vec<IndexedWGS84Point> {
    let mut ret = Vec::new();
    for k in 0..track.wgs84.len() {
        ret.push(IndexedWGS84Point {
            wgs84: track.wgs84[k].clone(),
            index: k,
        });
    }
    ret
}

pub fn nearest_neighboor(
    track: &Track,
    waypoints: &Vec<waypoint::Waypoint>,
) -> std::collections::BTreeMap<usize, usize> {
    log::trace!("build tree");
    let find_nearest = sphere_knn::run(convert(&track));
    log::trace!("project {} points", waypoints.len());
    let mut ret = std::collections::BTreeMap::new();
    for k in 0..waypoints.len() {
        let point = &waypoints[k];
        let result = find_nearest(
            point.wgs84.latitude(),
            point.wgs84.longitude(),
            sphere_knn::Opts {
                max_distance_threshold_meters: Some(1000f64),
                number_results: Some(1 as usize),
            },
        );
        match result.first() {
            Some(res) => {
                ret.insert(k, res.index);
            }
            None => {}
        }
    }
    log::trace!("project done");
    ret
}

pub fn project_on_track(track: &Track, waypoints: &mut Waypoints) {
    let indexmap = nearest_neighboor(&track, &waypoints);
    debug_assert!(waypoints.len() >= indexmap.len());
    for (src, dest) in indexmap {
        waypoints[src].track_index = Some(dest);
    }
}

#[cfg(test)]
mod tests {
    #[test]
    fn kdtree() {
        let items = vec![[10, 20, 30], [30, 10, 20], [20, 30, 10]];
        let kdtree = kd_tree::KdIndexTree::build(&items);
        assert_eq!(kdtree.nearest(&[30, 10, 20]).unwrap().item, &1);
        assert_eq!(kdtree.nearest(&[29, 9, 20]).unwrap().item, &1);
    }
}
