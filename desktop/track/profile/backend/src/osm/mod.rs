mod cache;
mod download;
mod filesystem;
#[cfg(target_arch = "wasm32")]
mod indexdb;
pub mod osmpoint;

use crate::bboxes::*;
use crate::inputpoint::{InputPointMap, InputPoints};
use crate::mercator::EuclideanBoundingBox;
use crate::track::*;

fn osm3(bbox: &WGS84BoundingBox) -> String {
    format!(
        "({:0.3},{:0.3},{:0.3},{:0.3})",
        bbox._min.1, bbox._min.0, bbox._max.1, bbox._max.0
    )
}

async fn download_chunk_real(
    bbox: &WGS84BoundingBox,
) -> std::result::Result<InputPoints, std::io::Error> {
    use download::*;
    let bboxparam = osm3(&bbox);
    let result = parse_osm_content(all(&bboxparam).await.unwrap().as_bytes());
    match result {
        Ok(points) => Ok(points),
        Err(e) => {
            log::info!("could not download(ignore)");
            log::info!("reason: {}", e.to_string());
            Err(std::io::Error::new(std::io::ErrorKind::InvalidData, "data"))
        }
    }
}

async fn download_chunk(bboxes: &Vec<EuclideanBoundingBox>) -> InputPoints {
    if bboxes.is_empty() {
        return InputPoints::new();
    }
    let eucbbox = bounding_box(&bboxes);
    let wgsbbox = eucbbox.unproject();

    log::info!("downloading for {} tiles", bboxes.len());
    let osmpoints = match download_chunk_real(&wgsbbox).await {
        Ok(points) => {
            log::info!("downloaded {:3} points", points.points.len());
            cache::write(bboxes, &points).await;
            points
        }
        Err(e) => {
            log::info!("error downloading: {:?}", e);
            log::info!("assuming there is nothing");
            InputPoints::new()
        }
    };
    osmpoints
}

async fn read(bbox: &EuclideanBoundingBox) -> InputPoints {
    let osmpoints = match cache::read(bbox).await {
        Some(d) => d,
        None => {
            // "could not find any data for {} (download probably failed) => skip",
            InputPoints::new()
        }
    };
    osmpoints
}

async fn remove_cache(tiles: &BoundingBoxes) -> Vec<EuclideanBoundingBox> {
    let mut uncached = Vec::new();
    for (_index, tile) in tiles {
        if !(cache::hit_cache(&tile).await) {
            uncached.push(tile.clone());
        }
    }
    uncached
}

async fn process(bbox: &EuclideanBoundingBox) -> InputPointMap {
    let tiles = split(&bbox, &BBOXWIDTH);
    let not_cached = remove_cache(&tiles).await;
    if !not_cached.is_empty() {
        log::info!(
            "there are {} tiles, {} not in cache",
            tiles.len(),
            not_cached.len()
        );
    }
    download_chunk(&not_cached).await;
    let mut ret = InputPointMap::new();
    log::trace!("about to read {} tiles", tiles.len());
    for (_index, tile) in tiles {
        let points = read(&tile).await;
        ret.insert_points(&tile, &points.points);
    }
    ret
}

pub async fn download_for_track(track: &Track) -> InputPointMap {
    let bbox = track.euclidean_bounding_box();
    assert!(!bbox.empty());
    let ret = process(&bbox).await;
    ret
}
