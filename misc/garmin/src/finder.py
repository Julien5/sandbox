import utils;

from scipy import spatial
import numpy as np

def point_as_list(p):
	return [p.latitude,p.longitude];

class Finder:
	def __init__(self,segment):
		P=[utils.Point(g.latitude,g.longitude) for g in segment];
		self.points=utils.Points(P);
		self.A = np.array([point_as_list(p) for p in P]);
		self.spatial = spatial.KDTree(self.A);

	def segment_length(self):
		return max(self.dist.values());

	def find_distance(self,p):
		#pt=point_as_list(Point(p.latitude,p.longitude));
		pt=[p.latitude,p.longitude];
		index=self.spatial.query(pt)[1];
		# nearest=self.A[index];
		return self.points.dist(index);

	def project(self,p):
		#pt=point_as_list(Point(p.latitude,p.longitude));
		pt=[p.latitude,p.longitude];
		index=self.spatial.query(pt)[1];
		#nearest=self.A[index];
		return self.points.point(index);
