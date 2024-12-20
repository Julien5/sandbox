#!/usr/bin/env python3

import os;
import subprocess;

import elevation;

arguments=None;

def get_rwaypoints_at_page(p,W):
	global arguments;
	size=arguments.segment_length;
	margin=math.ceil(size/10);
	xmin=max(size*p-margin,0);
	xmax=size*(p+1)+margin;
	R=list();
	for k in range(len(W)):
		w=W[k];
		if w.hide:
			continue;
		km=w.distance/1000;
		if xmin <= km and km <= xmax:
			R.append(k);
	return R;

def get_begin_end(xmin,xmax,E):
	(d,y)=E.xy();
	begin=None;
	end=None;
	for k in range(len(d)):
		if d[k]<xmin:
			continue;
		if d[k]>xmax:
			break;
		end=k;
		if begin is None:
			begin=k;
	return begin,end;

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
	global arguments;
	margin=1000*(arguments.segment_length/10);
	xmin=min([u[0] for u in U])-margin;
	xmax=max([u[0] for u in U])+margin;
	ymin=min([u[1] for u in U])-margin;
	ymax=max([u[1] for u in U])+margin;
	return xmin,xmax,ymin,ymax;

def map_csv_waypoints(F,utm):
	L=[];
	for w in F:
		lon=w.point.longitude;
		lat=w.point.latitude;
		label=""
		if w.label_on_profile:
			label=w.name[:2];
		x, y = utm(lon, lat)
		L.append(f"{x:10.1f}\t{y:10.1f}\t{lat:10.6f}\t{lon:10.6f}\t{label:s}\t{w.distance/1000:4.1f}");
	f=open("/tmp/profile/map-wpt.csv","w");
	f.write("\n".join(L));
	f.close();

def profile_csv_waypoints(F):
	L=list();
	assert(F);
	for w in F:
		wx=w.distance/1000;
		wy=w.point.elevation;
		label1="";
		label2=""
		if w.label_on_profile:
			label1=w.name[:2];
			label2=f"{wy:4.0f}";
		L.append(f"{wx:5.2f}\t{wy:5.0f}\t{label1:s}\t{label2:s}");
	f=open("/tmp/profile/elevation-wpt.csv","w");
	f.write("\n".join(L));
	f.close();


def findtemplate(dir,filename):
        return os.path.join(os.path.dirname(__file__),"..","..",dir,filename);
        
import pyproj
def gnuplot_map(P,E,W):
	os.makedirs("/tmp/profile",exist_ok=True);
	utm = pyproj.Proj("+proj=utm +zone=32 +north +ellps=WGS84 +datum=WGS84 +units=m +no_defs")
	d,dummy=E.xy();
	
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
	global arguments;
	sizem=arguments.segment_length*1000;
	for k in range(math.floor(d[-1]/sizem)+1):
		I=get_rwaypoints_at_page(k,W);
		map_csv_waypoints([W[i] for i in  I],utm);
		xmin,xmax,ymin,ymax=bbox(P,utm,d,k*sizem,(k+1)*sizem);
		content=content0;
		content=content.replace("{xmin}",str(xmin));
		content=content.replace("{xmax}",str(xmax));
		content=content.replace("{ymin}",str(ymin));
		content=content.replace("{ymax}",str(ymax));
		filename=f"/tmp/profile/map-{k:d}.gnuplot";
		f=open(filename,"w");
		f.write(content);
		f.close();
		subprocess.run(["gnuplot",filename],stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL);
		os.rename("/tmp/profile/map.png",f"/tmp/profile/map-{k:d}.png");

import math;
def gnuplot_profile(E,W):
	os.makedirs("/tmp/profile",exist_ok=True);
	(d,y)=E.xy();
	f=open("/tmp/profile/elevation.csv","w");
	for k in range(len(d)):
		f.write(f"{d[k]/1000:5.2f}\t{y[k]:5.2f}\n");
	f.close();

	f=open(findtemplate("gnuplot","profile.gnuplot"),"r");
	content0=f.read();
	f.close();
	length_km=d[-1]/1000;
	global arguments;
	sizekm=arguments.segment_length;
	marginkm=math.ceil(sizekm/10);
	for xk in range(math.floor(length_km/sizekm)+1):
		xmin=max(sizekm*xk-marginkm,0);
		xmax=sizekm*(xk+1)+marginkm;
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
		I=get_rwaypoints_at_page(xk,W);
		assert(I);
		profile_csv_waypoints([W[i] for i in I]);
		subprocess.run(["gnuplot",filename],stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL);
		os.rename("/tmp/profile/profile.png",f"/tmp/profile/profile-{xk:d}.png");

def index_with(L,string):
	for k in range(len(L)):
		if string in L[k]:
			return k;
	assert(False);	
	return None;

def format(text,wp):
	if wp.isControlPoint():
		return "\\textbf{"+text+"}";
	return f"{text:s}";

def formatFloat(f,ndec=0):
	if f is None:
		return "---";
	if ndec==1:
		return f"{f:3.1f}";
	return f"{f:3.0f}";

def latex_waypoint(W,k):
	rw=W[k];
	prev=None;
	if k>=1:
		prev=W[k-1];
	distance=0;
	if not prev is None:
		assert(not prev.hide);
		distance=rw.distance - prev.distance;
		assert(distance>0);
	L=list();
	L.append(format(rw.name[:2],rw));
	L.append(formatFloat(rw.distance/1000));
	L.append(rw.time.strftime("%H:%M"));
	L.append(formatFloat(rw.point.elevation));
	L.append(formatFloat(distance/1000));
	L.append(formatFloat(rw.dplus));
	L.append(formatFloat(rw.slope)+"\%");
	# the current layout has no more space for the description
	L.append(f"{rw.description[:15]:s}");
	separator=" & ";
	return separator.join(L);

def latex_total(E,begin,end):
	L=list();
	(d,y)=E.xy();
	dplus=None;
	distance=0;
	slope=None;
	assert(not (begin is None or end is None));
	if not (begin is None or end is None):
		distance=d[end]-d[begin];
		ebegin=E.floor_index(begin);
		eend=E.floor_index(end);
		xdplus,dplus=E.elevation_from_to(ebegin,eend);
		slope=None;
		assert(distance>0);
		slope=100*dplus/distance;
	dash="\\textemdash";
	line="\\hline";
	L.append(" ".join([line,line,"TOTAL"]));
	L.append(formatFloat(d[end]/1000));
	L.append(dash);
	L.append(dash);
	L.append(formatFloat(distance/1000));
	L.append(formatFloat(dplus));
	L.append(formatFloat(slope)+"\%");
	L.append(dash);
	separator=" & ";
	return separator.join(L);


def latex_profile(E,W,outpdf):
	f=open(findtemplate("tex","profile-template.tex"),"r");
	L=f.read().split("\n");
	f.close();
	start=index_with(L,"% begin-template");
	end=index_with(L,"% end-template");
	template0="\n".join(L[start:end+1]);
	filelist=os.listdir('/tmp/profile')
	page=0;
	out="\n".join(L);
	assert(template0 in out);
	parts=[];
	global arguments;
	sizem=arguments.segment_length*1000;
	while True:
		template=template0;
		profilepng=f"/tmp/profile/profile-{page:d}.png";
		mappng=f"/tmp/profile/map-{page:d}.png";
		begin,end=get_begin_end(page*sizem,(page+1)*sizem,E);
		if begin is None:
			break;
		template=template.replace("{profile-png}",profilepng);
		template=template.replace("{map-png}",mappng);
		I=get_rwaypoints_at_page(page,W);
		print(f"page:{page:d} waypoints:{len(I):d}");
		pointlist=list();
		for i in I:
			pointlist.append(latex_waypoint(W,i));
		pointlist.append(latex_total(E,begin,end));	
		newline="\\\\";	
		template=template.replace("{pointlist}",f"{newline:s}\n\t".join(pointlist)+newline);
		parts.append(template);
		page=page+1;
		if page>0 and page%2==0:
			parts.append("\\pagebreak");
	out=out.replace(template0,"\n".join(parts));
	f=open("/tmp/profile/profile.tex","w");
	f.write(out);
	f.close();
	pdf = os.path.abspath(outpdf);
	os.chdir("/tmp/profile")
	if os.path.exists("/tmp/profile/profile.pdf"):
		os.remove("/tmp/profile/profile.pdf")
	subprocess.run(["pdflatex","/tmp/profile/profile.tex"],stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL);
	assert(os.path.exists("/tmp/profile/profile.pdf"));
	os.rename("/tmp/profile/profile.pdf",pdf);
