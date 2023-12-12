#!/usr/bin/env python3

class Point:
	def __init__(self,la,lo,ele,time):
		self.latitude=la;
		self.longitude=lo;
		self.elevation=ele;
		self.time=time;

		
class Interval:
	def __init__(self,typename,begin,end):
		self.begin=begin;
		self.end=end;
		self.typename=typename;

	def contains(self,n):
		return self.begin<=n and n<self.end;

	def duration(self,points):
		return points[self.end].time - points[self.begin].time;

	def __str__(self):
		return f"{self.typename:10s}:{self.begin:d}-{self.end:d}";

	def join(self,other):
		b=min(self.begin,other.begin);
		e=max(self.end,other.end);
		self.begin=b;
		self.end=e;

class StartTimeComparator:
	def __init__(self,points):
		self.points=points;

	def key(self,interval):
		return self.points[interval.begin].time;

def union(T1,T2):
	startTimeCompare=StartTimeComparator(points);
	ret=list();
	ret.extend(T1);
	ret.extend(T2);
	return sorted(T,key=startTimeCompare.key);

def filter_intervals(intervals,points,function):
	ret=list();
	for I in intervals:
		if function(points,I):
			ret.append(I);
	return ret;

def split(J,condition):
	R=list();
	packet=list();
	for k in range(len(J)):
		if condition(J[k]):
			R.append(packet);
			packet=list();
		else:
			packet.append(J[k]);
	return R;

def join_intervals(intervals,points,join_condition):
	ret=list();
	N=len(intervals);
	assert(intervals);
	ret=[intervals[0]];
	k=1;
	for k in range(1,N):
		tail=ret[-1]
		nexti=intervals[k];
		if join_condition(points,tail,nexti):
			ret[-1].join(nexti);
		else:
			ret.append(nexti);
		k=k+1;
	return ret;

def complement_intervals(typename,intervals,points):
	# fixme
	N=len(intervals);
	ret=list();
	for k in range(len(N)-1):
		I1=intervals[k];
		I2=intervals[k+1];
		begin=I1.end;
		end=I2.begin;
		ret.append(Interval(typename,begin,end));
	return ret;	


def test_join_condition(points,I1,I2):
	return I2.begin - I1.end < 3;

def test_join_intervals():
	I=list();
	I.append(Interval("",1,3));
	I.append(Interval("",5,6))
	I.append(Interval("",15,17))
	I.append(Interval("",19,21))
	I.append(Interval("",22,24))
	I.append(Interval("",35,37))
	points=None;
	R=join_intervals(I,points,test_join_condition);
	for r in I:
		print(r)
	print("=>");
	for r in R:
		print(r)


def annotate_intervals(points,annotation_function):
	assert(points);
	intervals=[Interval(annotation_function(points,0),0,None)];
	N=len(points);
	for n in range(1,N):
		annotation=annotation_function(points,n);
		assert(not annotation is None);
		change=intervals[-1].typename != annotation;
		if not change:
			continue;
		# close the current one
		intervals[-1].end=n;
		# open new one 
		intervals.append(Interval(annotation,n,None));
	intervals[-1].end=N;
	return intervals;

def test_annotation_function(points,n):
	if points[n].elevation<1:
		return "low";
	if points[n].elevation>2:
		return "high";
	return "middle";

def test_annotate():
	I=list();
	points=list();
	points.append(Point(0,0,0.1,0));
	points.append(Point(0,0,0.2,0));
	points.append(Point(0,0,0.2,0));
	points.append(Point(0,0,0.2,0));
	points.append(Point(0,0,1.1,0));
	points.append(Point(0,0,2.3,0));
	points.append(Point(0,0,2.3,0));
	points.append(Point(0,0,2.3,0));
	J=annotate_intervals(points,test_annotation_function);
	assert(J);
	for I in J:
		print(I);

def main():
	test_annotate();
	#test_join_intervals();

if __name__ == "__main__":
	main();	
