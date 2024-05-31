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
	printf "|%10s|%50s|%100s|\n" "${size}" "${dir}"  "${file}"
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

function sort-same-size() {
	echo size="$1"
	shift
	grep "|[[:space:]]*${size}|" $a
	grep "|[[:space:]]*${size}|" $a | cut -f4 -d"|" | sed 's/^[ \t]*//;s/[ \t]*$//' | while read f; do
		md5=$(md5sum "${f}" | cut -f1 -d" ")
		echo "${f}" >> /tmp/lists/md5/${md5}
	done
}

function sort-uniq() {
	echo size="$1"
	shift
	grep "|[[:space:]]*${size}|" $a
	grep "|[[:space:]]*${size}|" $a | cut -f4 -d"|" | sed 's/^[ \t]*//;s/[ \t]*$//' | while read f; do
		echo "${f}" >> /tmp/lists/uniq/all
	done
}

function main() {
	if [ ! -d  /tmp/lists ]; then
		mkdir -p /tmp/lists
		makelist /media/julien/backup /tmp/lists/backup.txt &
		makelist /data /tmp/lists/data.txt &
		makelist /home/julien /tmp/lists/home.txt &
		sleep 1
		echo wait...
		wait $(jobs -p)

		cleanlist /tmp/lists/backup.txt &
		cleanlist /tmp/lists/data.txt &
		cleanlist /tmp/lists/home.txt &
		echo wait...
		wait $(jobs -p)	

		extendlist /tmp/lists/backup.txt.clean &
		extendlist /tmp/lists/data.txt.clean &
		extendlist /tmp/lists/home.txt.clean &
		sleep 1
		echo wait...
		wait $(jobs -p)
	fi

	echo merge all
	cat /tmp/lists/{backup,data,home}.txt.clean.extended > /tmp/lists/all.extended

	a=/tmp/lists/all.extended
	n=$(cat $a | wc -l)
	ndup=$(cat $a | cut -f2 -d"|" | sort -n | uniq -c | sort -n | grep -v " 1 " | wc -l)
	printf "%50s n=%5d ndup=%5d\n" "${a}" "${n}" "${ndup}"

	mkdir -p /tmp/lists/{md5,uniq}
	rm -f /tmp/lists/uniq/*
	rm -f /tmp/lists/md5/*

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
