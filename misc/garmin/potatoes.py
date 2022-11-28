#!/usr/bin/env python3

import track
import geometry;
import segment;
import sys;

def get_size(obj, seen=None):
	"""Recursively finds size of objects"""
	size = sys.getsizeof(obj)
	if seen is None:
		seen = set()
	obj_id = id(obj)
	if obj_id in seen:
		return 0
	# Important mark as seen *before* entering recursion to gracefully handle
	# self-referential objects
	seen.add(obj_id)
	if isinstance(obj, dict):
		size += sum([get_size(v, seen) for v in obj.values()])
		size += sum([get_size(k, seen) for k in obj.keys()])
	elif hasattr(obj, '__dict__'):
		size += get_size(obj.__dict__, seen)
	elif hasattr(obj, '__iter__') and not isinstance(obj, (str, bytes, bytearray)):
		size += sum([get_size(i, seen) for i in obj])
	return size

def stringify(combinaison):
	return ":".join([str(ix) for ix in sorted(combinaison)]);

def harvest(pool):
	all=dict();
	print("harvest: compute all indexes in",len(pool),"segments");
	for k in range(len(pool)):
		seg=pool[k];
		for index in seg:
			if not index in all:
				all[index]=set();
			all[index].update(seg.tracks);
	# invert
	print("#all",len(all),"boxes are in potatoes", "size (Mb)",get_size(all)/1e6);
	indexes=dict();
	for index in all:
		combinaison=sorted(list(all[index]));
		key=tuple(combinaison);
		#print(combinaison)
		if not key in indexes:
			indexes[key]=set();
		indexes[key].add(index);
	print("#indexes size (Mb)",get_size(indexes)/1e6);
	print("harvest:", len(indexes),"combinations")
	return indexes;

def harvestmarkers(pool):
	markers=dict();
	for k in range(len(pool)):
		for m in pool[k].trackmarkers():
			if not m in markers:
				markers[m]=set();
			markers[m].add(k)
	# invert
	for m in markers:
		print(m,markers[m]);
	return markers;	

def main():
	test();	

if __name__ == '__main__':
	import sys;
	sys.exit(main())  
