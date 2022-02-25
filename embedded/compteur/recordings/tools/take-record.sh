#!/usr/bin/env bash

set -e

n=$(find simulation/data/ -type d -name "test-*" | wc -l)
m=$((n+1))
DIR=simulation/data/test-$(printf "%02d" $m)
mkdir -p $DIR
egrep "(common::analog::read)" /tmp/build_arduino/cmake/screenlog.0 | cut -f6 -d: | tr -d "\r" > $DIR/output.adc
cp /tmp/build_arduino/cmake/screenlog.0 $DIR/
