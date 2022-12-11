#!/usr/bin/env bash

set -e
set -x

BUILDDIR=/tmp/build_pc
mkdir -p $BUILDDIR
DIR=$(pwd)
pushd $BUILDDIR
cmake $DIR
make
echo jbo
$BUILDDIR/jbo/jbo 10 10 10
echo msa
$BUILDDIR/msa/msa 10 10 10
