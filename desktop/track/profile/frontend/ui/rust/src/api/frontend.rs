#![allow(non_snake_case)]

use flutter_rust_bridge::frb;

pub struct Frontend {
    backend: Box<tracks::backend::Backend>,
}

pub struct Segment {
    _backend: Box<tracks::backend::Segment>,
}

use std::{str::FromStr, time::Duration};
use tokio::time::sleep;

impl Frontend {
    pub fn create() -> Frontend {
        Frontend {
            backend: Box::new(tracks::backend::Backend::new()),
        }
    }
    #[frb(sync)]
    pub fn changeParameter(&mut self, eps: f32) {
        self.backend.changeParameter(eps);
    }
    pub async fn renderTrack(&mut self) -> String {
        self.backend.render_track()
    }
    pub async fn renderWaypoints(&mut self) -> String {
        self.backend.render_waypoints()
    }
    pub async fn renderSegmentTrack(&self, segment: Segment) {
        self.backend.render_segment_track(&segment._backend);
    }
    pub async fn renderSegmentWaypoints(&self, segment: Segment) {
        self.backend.render_segment_waypoints(&segment._backend);
    }
    pub async fn segments(&self) -> Vec<Segment> {
        let segb = self.backend.segments();
        let mut ret = Vec::new();
        for s in segb {
            let f = Segment {
                _backend: Box::new(s.clone()),
            };
            ret.push(f);
        }
        ret
    }
}

impl Segment {
    pub fn start(&self) -> usize {
        self._backend.range.start
    }
    pub fn end(&self) -> usize {
        self._backend.range.end
    }
}

pub async fn svgCircle() -> String {
    sleep(Duration::from_secs(1)).await;
    let s = r#"<svg height="100" width="100" xmlns="http://www.w3.org/2000/svg">
  <circle r="45" cx="50" cy="50" fill="red" />
</svg>"#;
    String::from_str(s).unwrap()
}

#[flutter_rust_bridge::frb(init)]
pub fn init_app() {
    // Default utilities - feel free to customize
    flutter_rust_bridge::setup_default_user_utils();
}
