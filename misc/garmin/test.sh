#!/usr/bin/env bash

rm -f book.cache

python3 src/main.py &> /tmp/out
if diff -q /tmp/out test/output; then
	echo "outputs are the same. good."
else
	meld /tmp/out test/output;
fi
	
