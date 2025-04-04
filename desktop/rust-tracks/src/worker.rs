extern crate gpx;
extern crate geo;

use crate::speed;

use gpx::read;
use gpx::{Gpx, Track, TrackSegment};

type Time = chrono::NaiveDateTime;
fn time_delta(t0:&Time,t1:&Time) -> i64 {
	let diff=t1.time()-t0.time();
	diff.num_seconds()
}

use geo::Distance;
fn distance(x1:f64,y1:f64,x2:f64,y2:f64) -> f64 {
	//let p0 = geoutils::Location::new(xprev, yprev);
	//let p1 = geoutils::Location::new(x, y);
	//let distance = p0.distance_to(&p1).unwrap().meters();
	let p1 = geo::Point::new(x1,y1);
	let p2 = geo::Point::new(x2,y2);
	let ret = geo::Haversine::distance(p1,p2);
	ret
}

pub fn worker(content : &Vec<u8>) -> usize {
	let reader_mem=std::io::Cursor::new(content);
	let gpx: Gpx = read(reader_mem).unwrap();
    let track: &Track = &gpx.tracks[0];
    let segment: &TrackSegment = &track.segments[0];

	let mut previous: Option<(f64,f64,Time)> = None;
	let mut total_distance = 0f64;
	let mut total_time = 0i64;
	for point in &segment.points {
		let (x,y)=point.point().x_y();
		if point.time.is_none() {
			break;
		}

		let timestring=point.time.unwrap().format().unwrap();
		let time : Time = chrono::NaiveDateTime::parse_from_str(timestring.as_str(),"%Y-%m-%dT%H:%M:%S.%fZ").ok().unwrap();
		let (distance,dtime) = match previous {
			Some((xprev,yprev,tprev)) => {
				let d=distance(xprev,yprev,x,y);
				(d,time_delta(&tprev,&time))},
			None => (0f64,0i64),
		};
		total_distance += distance;
		total_time += dtime;
		//let speed = kmh(distance/dtime as f64);
		//println!("point: ({x:.2},{y:.2}) {distance:4.0}m {dtime:3.0}s {speed:.1} kmh");
		previous = Some((x,y,time));
	}
	let total_distance_km = total_distance/1000f64;
	let tt=chrono::TimeDelta::new(total_time,0).unwrap().num_minutes();
	let avg=speed::kmh(total_distance/total_time as f64);
	let npoints = segment.points.len();
	println!("total || {npoints:5} points | {total_distance_km:6.1} km | {tt:4.0} min | {avg:5.2} kmh |");
	npoints
}
