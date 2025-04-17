extern crate gpx;
extern crate geo;
extern crate poloto;
extern crate tagu;

use std::collections::BTreeMap;

use poloto::build;
use tagu::prelude::*;

use gpx::read;
use gpx::{Gpx, Track, TrackSegment};


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


use plotters::prelude::*;
pub fn plot_data(segment:&TrackSegment)  -> Result<(), Box<dyn std::error::Error>> {
    let root = SVGBackend::new("0.svg", (800*2, 400*2)).into_drawing_area();
    root.fill(&WHITE)?;

	let mut prev:Option<geo::Point> = None;
	let mut xdata = Vec::new();
	let mut ydata = Vec::new();
	let mut d = 0f64;
	for k in 0..segment.points.len() {
		if k%100 != 0 {
			continue;
		}
		let point = &segment.points[k];
		let (x,y)=point.point().x_y();
		match prev {
			Some(p) => {
				d+=distance(p.x(),p.y(),x,y);
				let dy=point.elevation.unwrap();
				xdata.push(d/1000f64);
				ydata.push(dy);
			}
			_ => {}
		}
		//println!("point: ({x:.2},{y:.2}:{z:.2})");
		prev=Some(point.point());
	}
	let xmax = xdata.last().unwrap().clone();
	
    let mut chart = ChartBuilder::on(&root)
        .margin(5)
        .x_label_area_size(40)
        .y_label_area_size(40)
        .build_cartesian_2d(0f64..xmax, 0f64..2000f64)?;

    chart.configure_mesh().draw()?;

	let style = ShapeStyle {
		color: BLUE.mix(0.6),
		filled: false,
		stroke_width: 10,
	};

	xdata.clear();
	ydata.clear();
	xdata.push(0f64);
	xdata.push(80f64);
	xdata.push(90f64);
	xdata.push(100f64);
	
	ydata.push(0f64);
	ydata.push(1600f64);
	ydata.push(60f64);
	ydata.push(1600f64);

    chart
        .draw_series(LineSeries::new(
            (0..xdata.len()).map(|k| (xdata[k],ydata[k])),
            BLUE.mix(0.9).stroke_width(100),
        ))?
        .label("profile");

	chart.draw_series(PointSeries::of_element(
        vec![(0.0, 0.0), (50.0, 500.0), (80.0, 700.0)],
        5,
        &RED,
        &|c, s, st| {
            return EmptyElement::at(c)    // We want to construct a composed element on-the-fly
				+ Circle::new((0,0),s,st.filled()) // At this point, the new pixel coordinate is established
				+ Text::new(format!("{:?}", c), (10, 0), ("sans-serif", 20).into_font());
        },
    ))?;

    chart
        .configure_series_labels()
        .background_style(&WHITE.mix(0.8))
        .border_style(&BLACK)
        .draw()?;

    root.present()?;

    Ok(())
}

pub fn svg(segment:&TrackSegment) {
	let mut prev:Option<geo::Point> = None;
	let mut xdata = Vec::new();
	let mut ydata = Vec::new();
	let mut d = 0f64;
	for k in 0..segment.points.len() {
		if k%100 != 0 {
			continue;
		}
		let point = &segment.points[k];
		let (x,y)=point.point().x_y();
		match prev {
			Some(p) => {
				d+=distance(p.x(),p.y(),x,y);
				let dy=point.elevation.unwrap();
				let kx=(d/1000f64).floor() as i32;
				xdata.push(kx);
				ydata.push(dy);
				//ydata.insert(kx,dy);
			}
			_ => {}
		}
		//println!("point: ({x:.2},{y:.2}:{z:.2})");
		prev=Some(point.point());
	}
	let xmax = xdata.last().unwrap().clone();
	
	let collatz = |mut a: i128| {
        std::iter::from_fn(move || {
            if a == 1 {
                None
            } else {
                a = if a % 2 == 0 { a / 2 } else { 3 * a + 1 };
                Some(a)
            }
        })
			.fuse()
    };

    let svg = poloto::header().with_viewbox_width(1200.0);

    let style = poloto::render::Theme::dark().append(tagu::build::raw(
        ".poloto_line{stroke-dasharray:2;stroke-width:2;}",
    ));

    let _a = (1000..1006).map(|i| poloto::build::plot(format!("c({})", i)).line((0..).zip(collatz(i))));
	let a = poloto::build::plot("title").line(std::iter::zip(xdata,ydata));

		//xdata.into_iter().map(|x| -> f64 { *ydata.get(&x).unwrap() }).zip());

    poloto::frame()
        .with_tick_lines([true, true])
        .with_viewbox(svg.get_viewbox())
        .build()
        .data(poloto::plots!(poloto::build::origin(), a))
        .build_and_label(("collatz", "x", "y"))
        .append_to(svg.append(style))
        .render_stdout();
}

pub fn worker(content : &Vec<u8>) {
	let reader_mem=std::io::Cursor::new(content);
	let gpx: Gpx = read(reader_mem).unwrap();
    let track: &Track = &gpx.tracks[0];
    let segment: &TrackSegment = &track.segments[0];
	match plot_data(segment) {
		Ok(()) => {},
		_ => {
			println!("failed");
		}
	}
}
