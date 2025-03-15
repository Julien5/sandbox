extern crate gpx;

use std::io::BufReader;
use std::fs::File;

use gpx::read;
use gpx::{Gpx, Track, TrackSegment};

fn main() {
    let file = File::open("/home/julien/projects/tracks/3d31374775331e8f5ef06ddc5a8d213e/gpx/GAP00-00-moving.gpx").unwrap();
    let reader = BufReader::new(file);

    // read takes any io::Read and gives a Result<Gpx, Error>.
    let gpx: Gpx = read(reader).unwrap();

    // Each GPX file has multiple "tracks", this takes the first one.
    let track: &Track = &gpx.tracks[0];

    // Each track will have different segments full of waypoints, where a
    // waypoint contains info like latitude, longitude, and elevation.
    let segment: &TrackSegment = &track.segments[0];

    // This is an example of retrieving the elevation (in meters) at certain points.
	for point in &segment.points {
		let (x,y)=point.point().x_y();
		let elevation=point.elevation.unwrap();
		let speed=point.speed.unwrap_or_default();
		let time=match point.time {
			Some(t) => t.format().unwrap_or_default(),
			None => String::from("None"),
		};
		println!("point: ({x:.2},{y:.2}) {elevation:.0} {speed:.0} {time}");
	}
}
