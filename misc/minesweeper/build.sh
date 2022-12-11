#!/usr/bin/env bash

set -e
set -x

mkdir -p /tmp/build_pc
DIR=$(pwd)
pushd /tmp/build_pc
cmake $DIR
make
./app
