#!/usr/bin/env bash

set -e
#set -x

function makelist() {
	local dir=$1
	shift
	local file=$1
	shift
	echo make ${file}
	find ${dir} -type f -iname "*.jpg" | sort > ${file}
	echo ${file} is done
}

function extendlist-element() {
	local file="$1"
	shift
	local dir="$(echo "${file}" | cut -f1-6 -d/)"
	local size="$(stat "${file}" | grep Size | cut -f2 -d: | cut -f1 | tr -d " ")"
	local date="$(stat "${file}" | grep Modify | cut -f2-5 -d: | cut -b2-)"
	printf "|%s|%s|%s|\n" "${size}" "${dir}"  "${file}"
}

function extendlist() {
	local list=$1
	shift
	echo extending ${list}
	cat ${list} | while read file; do
		extendlist-element "${file}"
	done > ${list}.extended
	echo extending ${list} done
}

function cleanlist() {
	local list=$1
	shift
	echo cleaning ${list}
	cat ${list} | grep -v "/.cache/" | grep -v "/.xplanet/" > ${list}.clean
	echo cleaning ${list} done
}

DIR=$HOME/tmp

function sort-same-size() {
	echo size="$1"
	shift
	grep "|[[:space:]]*${size}|" $a
	grep "|[[:space:]]*${size}|" $a | cut -f4 -d"|" | sed 's/^[ \t]*//;s/[ \t]*$//' | while read f; do
		md5=$(md5sum "${f}" | cut -f1 -d" ")
		echo "${f}" >> $DIR/tmp/lists/md5/${md5}
	done
}

function sort-uniq() {
	echo size="$1"
	shift
	grep "|[[:space:]]*${size}|" $a
	grep "|[[:space:]]*${size}|" $a | cut -f4 -d"|" | sed 's/^[ \t]*//;s/[ \t]*$//' | while read f; do
		echo "${f}" >> $DIR/tmp/lists/uniq/all
	done
}

function main() {
	if [ ! -d  $DIR/tmp/lists ]; then
		mkdir -p $DIR/tmp/lists
		makelist /media/julien/backup $DIR/tmp/lists/backup.txt &
		makelist /data $DIR/tmp/lists/data.txt &
		makelist /home/julien $DIR/tmp/lists/home.txt &
		sleep 1
		echo wait...
		wait $(jobs -p)

		cleanlist $DIR/tmp/lists/backup.txt &
		cleanlist $DIR/tmp/lists/data.txt &
		cleanlist $DIR/tmp/lists/home.txt &
		echo wait...
		wait $(jobs -p)	

		extendlist $DIR/tmp/lists/backup.txt.clean &
		extendlist $DIR/tmp/lists/data.txt.clean &
		extendlist $DIR/tmp/lists/home.txt.clean &
		sleep 1
		echo wait...
		wait $(jobs -p)
		
		echo merge all
		cat $DIR/tmp/lists/{backup,data,home}.txt.clean.extended > $DIR/tmp/lists/all.extended
	fi

	a=$DIR/tmp/lists/all.extended
	n=$(cat $a | wc -l)
	ndup=$(cat $a | cut -f2 -d"|" | sort -n | uniq -c | sort -n | grep -v " 1 " | wc -l)
	printf "%50s n=%5d ndup=%5d\n" "${a}" "${n}" "${ndup}"

	mkdir -p $DIR/tmp/lists/{md5,uniq}
	rm -f $DIR/tmp/lists/uniq/*
	rm -f $DIR/tmp/lists/md5/*

	echo sort uniq
	for size in $(cat $a | cut -f2 -d"|" | sort -n | uniq -c | sort -n | grep " 1 "  | cut -b8- | xargs); do
		sort-uniq ${size}
	done


	echo sort md5
	for size in $(cat $a | cut -f2 -d"|" | sort -n | uniq -c | sort -n | grep -v " 1 " | cut -b8- | xargs); do
		sort-same-size ${size}
	done
			 
}

main
