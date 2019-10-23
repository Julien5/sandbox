#!/usr/bin/env bash

set -e
set -x

FF=--no-ff
#FF=

DIR=/tmp/foo
rm -Rf $DIR/*
rm -Rf $DIR/.git
mkdir -p $DIR
cd $DIR
git init . 

echo a > a
echo b > b
git add *
git commit . -m"* master"
echo m1 >> a
git commit . -m"* m1"
echo m2 >> a
git commit . -m"* m2"
sleep 0

git checkout -b B1
echo aB1 >> a
git commit . -m"* B1 1"
sleep 0.1
echo aB1 >> a
git commit . -m"* B1 2"
sleep 0.1
echo aB1 >> a
git commit . -m"* B1 3"
sleep 0.1

git checkout master
git checkout -b B2
echo aB2 >> b
git commit . -m"* B2 1"
sleep 0.1
echo aB2 >> b
git commit . -m"* B2 2"
sleep 0.1
echo aB2 >> b
git commit . -m"* B2 3"
sleep 0.1

git checkout master
git merge $FF B1 --no-edit 
git merge $FF B2 --no-edit 
#git log
git log --graph --oneline --all
ls
