#!/usr/bin/env bash

DIR=/tmp/elevation
mkdir -p ${DIR}/test
cp src/elevation.py ${DIR}
cp src/readtracks.py ${DIR}
cp src/utils.py ${DIR}
cp test/elevation*.gpx ${DIR}/test
cp export-for-jens.sh ${DIR}/
cp export-for-jens.md ${DIR}/
cd ${DIR}/
python3 elevation.py
tar zcvf /tmp/elevation.tgz *.*

