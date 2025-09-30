use crate::wgs84point::WGS84Point;
use serde::{Deserialize, Serialize};

pub type DateTime = chrono::DateTime<chrono::Utc>;

#[derive(Clone, Serialize, Deserialize, PartialEq, Debug)]
pub struct MercatorPoint(pub f64, pub f64);

impl MercatorPoint {
    pub fn x(&self) -> f64 {
        self.0
    }
    pub fn y(&self) -> f64 {
        self.1
    }
    pub fn xy(&self) -> (f64, f64) {
        (self.0, self.1)
    }
    pub fn from_xy((x, y): &(f64, f64)) -> MercatorPoint {
        MercatorPoint(*x, *y)
    }
}

pub struct WebMercatorProjection {
    wgs84_spec: proj4rs::proj::Proj,
    dst_spec: proj4rs::proj::Proj,
}

impl WebMercatorProjection {
    pub fn make() -> WebMercatorProjection {
        // The PROJ.4 parameters for EPSG:3857 (also known as Web Mercator or Pseudo-Mercator) are:
        // +proj=merc +lon_0=0 +k=1 +x_0=0 +y_0=0 +datum=WGS84 +units=m +no_defs
        // https://gis.stackexchange.com/questions/159572/proj4-for-epsg3857
        use proj4rs::proj::Proj;
        let spec = format!(
			"+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +wktext  +no_defs"
		);
        let dst_spec = Proj::from_proj_string(spec.as_str()).unwrap();

        let spec = "+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs";
        let wgs84_spec = Proj::from_proj_string(spec).unwrap();
        WebMercatorProjection {
            wgs84_spec,
            dst_spec,
        }
    }
    pub fn project(&self, wgs84: &WGS84Point) -> MercatorPoint {
        let mut p = (
            wgs84.longitude().to_radians(),
            wgs84.latitude().to_radians(),
        );
        proj4rs::transform::transform(&self.wgs84_spec, &self.dst_spec, &mut p).unwrap();
        MercatorPoint(p.0, p.1)
    }
}
