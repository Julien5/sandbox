use core::fmt;
use std::str::FromStr;

use serde::{Deserialize, Serialize};
use serde_json::json;

use crate::waypoint::{WGS84Point, Waypoint, WaypointOrigin};

pub type Tags = std::collections::BTreeMap<String, String>;

#[derive(Debug, PartialEq, Eq, PartialOrd, Ord, Serialize, Deserialize)]
pub enum OSMType {
    City,
    MountainPass,
    Village,
}

#[derive(Clone, Serialize, Deserialize)]
pub struct OSMPoint {
    pub wgs84: WGS84Point,
    pub tags: Tags,
    pub track_index: Option<usize>,
}

fn read<T>(data: Option<&String>) -> Option<T>
where
    T: FromStr,
{
    match data {
        Some(text) => match text.parse::<T>() {
            Ok(f) => {
                return Some(f);
            }
            Err(_e) => {}
        },
        None => {}
    }
    return None;
}

fn shorten_name(name: &String) -> String {
    if name.len() < 10 {
        return name.clone();
    }
    let parts = name.split_whitespace().collect::<Vec<_>>();
    let n = 1;
    for n in 0..parts.len() {
        let mut ret = parts.clone();
        ret.truncate(n);
        let candidate = ret.join(" ");
        if candidate.len() > 5 {
            return candidate;
        }
    }
    name.clone()
}

impl OSMPoint {
    pub fn ele(&self) -> Option<f64> {
        read::<f64>(self.tags.get("ele"))
    }
    pub fn name(&self) -> Option<String> {
        let ret = self.tags.get("name");
        if ret.is_some() {
            return Some(ret.unwrap().clone());
        }
        for (k, v) in &self.tags {
            if k.contains("name") {
                return Some(v.as_str().to_string());
            }
        }
        return None;
    }
    pub fn short_name(&self) -> Option<String> {
        match self.name() {
            Some(n) => Some(shorten_name(&n)),
            None => None,
        }
    }
    pub fn population(&self) -> Option<i32> {
        read::<i32>(self.tags.get("population"))
    }
    pub fn kind(&self) -> OSMType {
        match self.tags.get("place") {
            Some(place) => {
                if place == "town" {
                    return OSMType::City;
                }
                if place == "village" {
                    return OSMType::Village;
                }
            }
            _ => {}
        }
        match self.tags.get("mountain_pass") {
            Some(pass) => {
                if pass == "yes" {
                    return OSMType::MountainPass;
                }
            }
            _ => {}
        }
        OSMType::Village
    }
    pub fn waypoint(&self) -> Waypoint {
        Waypoint {
            wgs84: self.wgs84.clone(),
            track_index: self.track_index,
            name: self.name().clone(),
            description: None,
            info: None,
            origin: WaypointOrigin::OpenStreetMap,
        }
    }
}

impl fmt::Display for OSMPoint {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(
            f,
            "{}=({:.2},{:.2},{:.1})",
            if self.name().is_none() {
                String::new()
            } else {
                self.name().clone().unwrap()
            },
            self.wgs84.longitude(),
            self.wgs84.latitude(),
            if self.ele().is_none() {
                0f64
            } else {
                self.ele().unwrap()
            },
        )
    }
}

#[derive(Clone, Serialize, Deserialize)]
pub struct OSMPoints {
    pub points: Vec<OSMPoint>,
}

impl OSMPoints {
    pub fn new() -> OSMPoints {
        OSMPoints { points: Vec::new() }
    }
    pub fn from_string(data: &String) -> OSMPoints {
        match serde_json::from_str(data.as_str()) {
            Ok(points) => points,
            Err(e) => {
                log::error!("could not read osmpoints from: {}", data);
                log::error!("because: {:?}", e);
                OSMPoints::new()
            }
        }
    }
    pub fn as_string(&self) -> String {
        json!(self).to_string()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use serde_json::json;

    fn testpoint() -> OSMPoint {
        OSMPoint {
            lat: 1.0,
            lon: 1.1,
            tags: Tags::new(),
        }
    }

    #[test]
    fn point() {
        let p = testpoint();
        let data = json!(p);
        log::info!("{}", data)
    }

    #[test]
    fn points() {
        let p1 = testpoint();
        let p2 = testpoint();
        let points = OSMPoints {
            points: vec![p1, p2],
        };
        let data = json!(points);
        log::info!("{}", data)
    }
}
