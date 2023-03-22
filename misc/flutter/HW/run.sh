#!/usr/bin/env bash

source /home/julien/projects/config/profile/profile.sh

dev.flutter
rm -f main.exe main.js
rm -Rf bin
mkdir bin

set -x

echo "run main"
time dart main.dart

echo "compile main"
dart compile exe main.dart -o bin/main.exe
time ./bin/main.exe
# dart compile js main.dart -o bin/main.js

dart compile jit-snapshot main.dart; mv main.jit bin/

time dart run bin/main.jit
