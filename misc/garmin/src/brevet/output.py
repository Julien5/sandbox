#!/usr/bin/env python3

import os;
import subprocess;

import elevation;

def get_rwaypoints_at_page(k,W):
	xmin=max(100*k-10,0);
	xmax=100*(k+1)+10;
	R=dict();
	for distance in sorted(W.keys()):
		km=distance/1000;
		if xmin <= km and km <= xmax:
			R[distance]=W[distance];
	return R;	

def bbox(P,proj,d,dmin,dmax):
	assert(len(P)==len(d));
	kmin=None;
	kmax=None;
	for k in range(len(d)):
		if kmin is None or d[k]<dmin:
			kmin=k;
		if kmax is None or d[k]<dmax:
			kmax=k;
	U=[proj(p.longitude,p.latitude) for p in P[kmin:kmax]];
	margin=10000;
	xmin=min([u[0] for u in U])-margin;
	xmax=max([u[0] for u in U])+margin;
	ymin=min([u[1] for u in U])-margin;
	ymax=max([u[1] for u in U])+margin;
	return xmin,xmax,ymin,ymax;

def map_csv_waypoints(F,utm):
	L=[];
	for distance in F.keys():
		lon=F[distance].point.longitude;
		lat=F[distance].point.latitude;
		label=F[distance].name[:2];
		x, y = utm(lon, lat)
		L.append(f"{x:10.1f}\t{y:10.1f}\t{lat:10.6f}\t{lon:10.6f}\t{label:s}\t{distance/1000:4.1f}");
	f=open("/tmp/profile/map-wpt.csv","w");
	f.write("\n".join(L));
	f.close();

def profile_csv_waypoints(F):
	L=list();
	assert(F);
	for distance in F.keys():
		w=F[distance];
		wx=w.distance/1000;
		wy=w.point.elevation;
		label1=w.name[:2];
		label2=f"{wy:4.0f}";
		L.append(f"{wx:5.2f}\t{wy:5.0f}\t{label1:s}\t{label2:s}");
	f=open("/tmp/profile/elevation-wpt.csv","w");
	f.write("\n".join(L));
	f.close();


def findtemplate(dir,filename):
        return os.path.join(os.path.dirname(__file__),"..","..",dir,filename);
        
import pyproj
def gnuplot_map(P,W):
	os.makedirs("/tmp/profile",exist_ok=True);
	utm = pyproj.Proj("+proj=utm +zone=32K, +north +ellps=WGS84 +datum=WGS84 +units=m +no_defs")
	d,dummy=elevation.load(P);
	
	L=[];
	for k in range(len(P)):
		lon=P[k].longitude;
		lat=P[k].latitude;
		x, y = utm(lon, lat)
		L.append(f"{x:10.1f}\t{y:10.1f}\t{lat:10.6f}\t{lon:10.6f}");
	f=open("/tmp/profile/map-track.csv","w");
	f.write("\n".join(L));
	f.close();

	f=open(findtemplate("gnuplot","map.gnuplot"),"r");
	content0=f.read();
	f.close();
	for k in range(math.floor(d[-1]/100)+1):
		Wk=get_rwaypoints_at_page(k,W);
		F=filter_waypoints(Wk);
		map_csv_waypoints(F,utm);
		xmin,xmax,ymin,ymax=bbox(P,utm,d,k*100,(k+1)*100);
		content=content0;
		content=content.replace("{pngx}",str(10*int((xmax-xmin)/1000)));
		content=content.replace("{pngy}",str(10*int((ymax-ymin)/1000)));
		content=content.replace("{xmin}",str(xmin));
		content=content.replace("{xmax}",str(xmax));
		content=content.replace("{ymin}",str(ymin));
		content=content.replace("{ymax}",str(ymax));
		filename=f"/tmp/profile/map-{k:d}.gnuplot";
		f=open(filename,"w");
		f.write(content);
		f.close();
		subprocess.run(["gnuplot",filename]);
		os.rename("/tmp/profile/map.png",f"/tmp/profile/map-{k:d}.png");
	return 	

import math;
def gnuplot_profile(P,W):
	os.makedirs("/tmp/profile",exist_ok=True);
	d,y=elevation.load(P);
	f=open("/tmp/profile/elevation.csv","w");
	for k in range(len(d)):
		f.write(f"{d[k]:5.2f}\t{y[k]:5.2f}\n");
	f.close();

	f=open(findtemplate("gnuplot","profile.gnuplot"),"r");
	content0=f.read();
	f.close();
	
	for xk in range(math.floor(d[-1]/100)+1):
		xmin=max(100*xk-10,0);
		xmax=100*(xk+1)+10;
		ymin=math.floor(min(y)/500)*500;
		ymax=math.ceil(max(y)/500)*500;
		content=content0;
		content=content.replace("{xmin}",str(xmin));
		content=content.replace("{xmax}",str(xmax));
		content=content.replace("{ymin}",str(ymin));
		content=content.replace("{ymax}",str(ymax));
		filename=f"/tmp/profile/profile-{xk:d}.gnuplot";
		f=open(filename,"w");
		f.write(content);
		f.close();
		Wk=get_rwaypoints_at_page(xk,W);
		assert(Wk);
		F=filter_waypoints(Wk);
		assert(F);
		profile_csv_waypoints(F);
		subprocess.run(["gnuplot",filename]);
		os.rename("/tmp/profile/profile.png",f"/tmp/profile/profile-{xk:d}.png");

def index_with(L,string):
	for k in range(len(L)):
		if string in L[k]:
			return k;
	assert(False);	
	return None;

def latex_waypoint(rwaypoint):
	L=list();
	L.append(rwaypoint.name[:2]);
	L.append(f"{rwaypoint.distance/1000:3.1f} km");
	L.append(rwaypoint.time.strftime("%H:%M"));
	L.append(f"{rwaypoint.point.elevation:3.1f}");
	separator=" & ";
	return separator.join(L);

def value(rw):
	if rw.isControlPoint():
		return 10000; # "infinity"
	assert(rw.point.elevation);
	return rw.point.elevation;

def sort_waypoints(W):
	return sorted(W, key=lambda rw: value(rw), reverse=True)

def closest(W,w0):
	assert(len(W)>=2);
	return sorted(W, key=lambda rw: abs(rw.distance-w0.distance))[1];

def filter_waypoints(W):
	if not W:
		return W;
	K=[W[d] for d in W.keys() if W[d].isControlPoint()];
	A=[W[d] for d in W.keys() if not W[d].isControlPoint()];
	W=A+K;
	assert(W);
	remove=list();
	for w in W:
		c=closest(W,w);
		d=abs(c.distance-w.distance);
		if d<5000:
			S=sort_waypoints([w,c]);
			remove.append(S[-1]);
	assert(len(remove)<len(W));		
	W2=[w for w in W if not w in remove];
	assert(W2);
	S=sort_waypoints(W2);
	ret=dict();
	for rw in S[:12]:
		ret[rw.distance]=rw;
	return ret;

def latex_profile(W):
	f=open(findtemplate("tex","profile-template.tex"),"r");
	L=f.read().split("\n");
	f.close();
	start=index_with(L,"% begin-template");
	end=index_with(L,"% end-template");
	template0="\n".join(L[start:end+1]);
	filelist=os.listdir('/tmp/profile')
	k=0;
	out="\n".join(L);
	assert(template0 in out);
	parts=[];
	while True:
		template=template0;
		profilepng=f"/tmp/profile/profile-{k:d}.png";
		mappng=f"/tmp/profile/map-{k:d}.png";
		if not os.path.exists(profilepng):
			break;
		template=template.replace("{profile-png}",profilepng);
		template=template.replace("{map-png}",mappng);
		Wk=get_rwaypoints_at_page(k,W);
		print(f"page:{k:d} waypoints:{len(Wk):d}");
		pointlist=list();
		F=filter_waypoints(Wk);
		for d in sorted(F.keys()):
			w=F[d];
			pointlist.append(latex_waypoint(w));
		newline="\\\\";	
		template=template.replace("{pointlist}",f"{newline:s}\n\t".join(pointlist)+newline);
		parts.append(template);
		k=k+1;
	out=out.replace(template0,"\n".join(parts));
	f=open("/tmp/profile/profile.tex","w");
	f.write(out);
	f.close();
