#!/usr/bin/env bash

OUT=/tmp/wifi.curloutput.internal
echo "$@" &> args
URL=
TIME=0
for a in $@; do
	if [[ "$a" = "http://"* ]];then
		URL="$a"
	fi
	if [[ "$a" = "time:"* ]];then
		b=$(echo $a | cut -f2 -d:)
		TIME=$((b/1000))
	fi
done

if [[ "$URL" = *"epoch"* ]];then
	echo $((EPOCH+TIME)) > $OUT
	sleep 0.1
fi

if [[ "$URL" = *"tickcounter/data"* ]];then
	# nothing to do
	echo -n
fi
