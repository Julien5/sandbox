#!/usr/bin/env bash

set -e
# set -x

function checkgaps() {
	a="$1"
	t_old=
	N=$(xmllint --format "$a" | grep time | wc -l)
	n=0;
	xmllint --format "$a" | grep time | cut -f2 -d">" | cut -b1-20 | while read d; do
		t=$(date -d "$d" +%s)
		n=$((n+1))
		if [[ ! -z "$t_old" ]]; then
			gap=$((t-t_old))
			if [[ $gap -gt 600 ]]; then
				p=$((100*n/N))
				printf "%8d seconds (%2d%s)[%5d/%5d] %s\n" $gap $p "%" $n $N "$a"
			fi
		fi
		t_old=$t
	done | sort -n | uniq -c 
}

function times() {
	find ~/tracks/ -name "Track_*05-MAR*1612*.gpx" | while read a; do
		xmllint --format "$a" 
		#xmllint --format "$a" | grep time | cut -f2 -d">" | cut -b1-20
	done 
}

# times
checkgaps "$@"

