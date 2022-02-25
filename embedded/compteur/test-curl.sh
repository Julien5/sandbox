#!/usr/bin/env bash

OUT=/tmp/wifi.curloutput.internal
echo "$@" &> $OUT
URL=
for a in $@; do
	if [[ "$a" = "http://"* ]];then
		URL="$a"
		break;
	fi
done
echo $URL
if [[ "$URL" = *"epoch"* ]];then
	echo $EPOCH
fi

if [[ "$URL" = *"tickcounter/data"* ]];then
	echo $EPOCH
fi
