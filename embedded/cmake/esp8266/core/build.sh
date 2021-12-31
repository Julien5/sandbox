#!/usr/bin/env bash

set -e
# set -x

BUILDDIR=/tmp/esp82266/core;
FILENAME=$BUILDDIR/stdout
mkdir -p $BUILDDIR;
if [[ ! -f $FILENAME ]]; then
	rm -Rf $BUILDDIR;
	echo cmaking core for esp8266
	cmake -S $IDF_PATH/examples/get-started/hello_world -B $BUILDDIR
	echo generating core for esp8266 
	make -C $BUILDDIR VERBOSE=1 &> $FILENAME 
else
	echo processing $FILENAME
fi

echo grabing CFLAGS 
cat $FILENAME | grep hello_world_main.c | grep "\-o" | tr " " "\n" | grep "^\-" | egrep -v "(-o|-c|-Werror=all|-std=gnu99)" | tr "\n" " " > $BUILDDIR/CFLAGS.txt

echo grabing LFLAGS
cat $FILENAME | grep hello-world.elf | grep g++ | grep "\-o" | tr " " "\n"  | grep -v "^$" | egrep -v "(-o|hello)" | grep -v "libmain.a" | while read a; do
	if [[ -f $BUILDDIR/$a ]]; then
		echo $(realpath $BUILDDIR/$a)
	elif [[ "$a" = *"xtensa-lx106-elf-g++"* ]]; then
		echo -n
	else
		echo $a
	fi
done | tr "\n" " " > $BUILDDIR/LFLAGS.txt


