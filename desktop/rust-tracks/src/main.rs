extern crate gpx;
extern crate chrono;
extern crate geographiclib_rs;
extern crate geoutils;

use std::io::BufReader;
use std::fs::File;

use gpx::read;
use gpx::{Gpx, Track, TrackSegment};
use geographiclib_rs::{Geodesic, InverseGeodesic};


type Time = chrono::NaiveDateTime;

fn time_delta(t0:&Time,t1:&Time) -> i64 {
	let diff=t1.time()-t0.time();
	diff.num_seconds()
}

fn kmh(speed:f64) -> f64 {
	// m/s => kmh
	speed * 3.6f64 
}

fn main() {
    let file = File::open("/home/julien/projects/tracks/3d31374775331e8f5ef06ddc5a8d213e/gpx/GAP00-00-moving.gpx").unwrap();
    let reader = BufReader::new(file);

    let gpx: Gpx = read(reader).unwrap();
    let track: &Track = &gpx.tracks[0];
    let segment: &TrackSegment = &track.segments[0];

	let g = Geodesic::wgs84();

	let mut previous: Option<(f64,f64,Time)> = None;
	let mut total_distance = 0f64;
	let mut total_time = 0i64;
	for point in &segment.points {
		let (x,y)=point.point().x_y();
		let elevation=point.elevation.unwrap();
		if point.time.is_none() {
			break;
		}

		let timestring=point.time.unwrap().format().unwrap();
		let time : Time = chrono::NaiveDateTime::parse_from_str(timestring.as_str(),"%Y-%m-%dT%H:%M:%S.%fZ").ok().unwrap();
		let (distance,dtime) = match previous {
			Some((xprev,yprev,tprev)) => {
				let p0 = geoutils::Location::new(xprev, yprev);
				let p1 = geoutils::Location::new(x, y);
				let distance = p0.distance_to(&p1).unwrap().meters();
				(distance,time_delta(&tprev,&time))},
			None => (0f64,0i64),
		};
		total_distance += distance;
		total_time += dtime;
		let speed = kmh(distance/dtime as f64);
		println!("point: ({x:.2},{y:.2}) {elevation:.0}m {distance:4.0}m {dtime:3.0}s {speed:.1} kmh");
		previous = Some((x,y,time));
	}
	let total_distance_km = total_distance/1000f64;
	let tt=chrono::TimeDelta::new(total_time,0).unwrap().num_minutes();
	let avg=kmh(total_distance/total_time as f64);
	println!("total: distance:{total_distance_km:.1} km time:{tt} min speed:{avg:.2} kmh");
}
