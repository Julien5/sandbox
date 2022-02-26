#!/usr/bin/env bash

OUT=/tmp/wifi.curloutput.internal
echo "$@" &> args
URL=
REQUEST_TIME=0
for a in $@; do
	if [[ "$a" = "http://"* ]];then
		URL="$a"
	fi
	if [[ "$a" = "time:"* ]];then
		b=$(echo $a | cut -f2 -d:)
		REQUEST_TIME=$((b/1000))
	fi
done
EPOCH0=1643670000
if [[ ! -z "$START_TIME" ]]; then
	EPOCH0=$((EPOCH0+START_TIME))
fi

if [[ "$URL" = *"epoch"* ]];then
	echo $((EPOCH0+REQUEST_TIME)) > $OUT
	sleep 0.1
fi

if [[ "$URL" = *"tickcounter/data"* ]];then
	# nothing to do
	echo -n
fi
