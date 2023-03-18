#!/usr/bin/env bash

source /home/julien/projects/config/profile/profile.sh

dev.flutter
rm -f main.exe main.js
echo "run main"
time dart main.dart
echo "compile main"
dart compile exe main.dart -o main.exe
time ./main.exe
ldd ./main.exe
dart compile js main.dart -o main.js
ls -lh main.js main.dart main.exe
