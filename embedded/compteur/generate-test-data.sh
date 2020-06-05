#!/usr/bin/env bash

set -e

function process {
    D="$1"
    find $D/ -iname "*.bin" | sort | while read a; do
	xxd $a | cut -f2 -d: | cut -b1-40 | tr -d " " | tr -d "\n"; echo;
    done | tr "[:lower:]" "[:upper:]" > /tmp/irdata 
    if [[ "$D" = "sd-data/22/" ]]; then
	sampling_period=50; offset=0;
	
    elif [[ "$D" = "sd-data/23/" ]]; then
	sampling_period=10; offset=2000;
    else
	echo "invalid param"
	exit 1
    fi
    python3 readirdata.py $sampling_period $offset
    mv -v /tmp/data.csv $D/hex/data.csv
}

rm -f data.csv
find sd-data -name "data.csv" -print -delete
process sd-data/22/
process sd-data/23/
# csv are sampled at 50 ms
find sd-data -name "data.csv" -exec ls -lh "{}" \;
# sub sample to 200 ms
find sd-data -name "data.csv" | while read a; do
    awk 'NR % 4 == 0' $a > /tmp/sub4
    mv /tmp/sub4 $a
done

# concatenate all
cat sd-data/22/hex/data.csv | cut -f2 > /tmp/data.csv
echo >> /tmp/data.csv
cat sd-data/23/hex/data.csv | cut -f2 >> /tmp/data.csv
python3 add-time-column.py 200 > /tmp/addline
mv /tmp/addline /tmp/data.csv
# cleanup 
# find sd-data -name "data.csv" -print -delete
ls -lh /tmp/data.csv
head /tmp/data.csv
