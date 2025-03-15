extern crate gpx;
extern crate chrono;
extern crate geographiclib_rs;

use std::io::BufReader;
use std::fs::File;

use gpx::read;
use gpx::{Gpx, Track, TrackSegment};
use geographiclib_rs::{Geodesic, InverseGeodesic};

use chrono::format::ParseError;

type Time = chrono::NaiveDateTime;

fn time_delta(t0:&Time,t1:&Time) -> i64 {
	let diff=t1.time()-t0.time();
	diff.num_seconds()
}

fn kmh(speed:f64) -> f64 {
	// m/s => kmh
	speed * 3600f64 / 60f64
}

fn main() {
    let file = File::open("/home/julien/projects/tracks/3d31374775331e8f5ef06ddc5a8d213e/gpx/GAP00-00-moving.gpx").unwrap();
    let reader = BufReader::new(file);

    let gpx: Gpx = read(reader).unwrap();
    let track: &Track = &gpx.tracks[0];
    let segment: &TrackSegment = &track.segments[0];

	// Determine the distance between two points - the "inverse" geodesic calculation.


	let g = Geodesic::wgs84();
	let p1 = (34.095925, -118.2884237);
	let p2 = (59.4323439, 24.7341649);
	let s12: f64 = g.inverse(p1.0, p1.1, p2.0, p2.1);

	let mut previous: Option<(f64,f64,Time)> = None;

	for point in &segment.points {
		let (x,y)=point.point().x_y();
		let elevation=point.elevation.unwrap();
		let speed=point.speed.unwrap_or_default();
		if point.time.is_none() {
			break;
		}

		let timestring=point.time.unwrap().format().unwrap();
		let time : Time = chrono::NaiveDateTime::parse_from_str(timestring.as_str(),"%Y-%m-%dT%H:%M:%S.%fZ").ok().unwrap();
		let (distance,dtime) = match previous {
			Some((xprev,yprev,tprev)) => (g.inverse(x,y,xprev,yprev),time_delta(&tprev,&time)),
			None => (0f64,0i64),
		};
		let speed = distance/dtime as f64;
		println!("point: ({x:.2},{y:.2}) {elevation:.0} {speed:.0} {distance:4.0}  {dtime:3.0} kmh");
		previous = Some((x,y,time));
	}
}
