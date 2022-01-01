#!/usr/bin/env bash

set -e
# set -x

if [[ -z "$IDF_PATH" ]]; then
	echo IDF_PATH not defined
	exit 1
fi

BUILDDIR=/tmp/esp8266/core;
FILENAME=$BUILDDIR/stdout
mkdir -p $BUILDDIR;
#rm -f $FILENAME
if [[ ! -f $FILENAME ]]; then
	rm -Rf $BUILDDIR;
	pushd $IDF_PATH
	git clean -dfx
	popd 
	echo cmaking core for esp8266
	pushd $IDF_PATH/examples/get-started/hello_world
	rm -f sdkconfig
	cmake -S $IDF_PATH/examples/get-started/hello_world -B $BUILDDIR
	echo CONFIG_COMPILER_OPTIMIZATION_LEVEL_RELEASE=y >> sdkconfig
	echo generating core for esp8266 
	make -j4 -C $BUILDDIR VERBOSE=1 &> $FILENAME
	popd
else
	echo processing $FILENAME
fi

echo grabing CFLAGS 
cat $FILENAME | grep hello_world_main.c | grep "\-o" | tr " " "\n" | grep "^\-" | egrep -v "(-o|-c|-Werror=all|-std=gnu99)" | tr "\n" " " > $BUILDDIR/CFLAGS.txt

echo grabing LFLAGS
qopened=false
cat $FILENAME | grep hello-world.elf | grep g++ | grep "\-o" | tr " " "\n"  | grep -v "^$" | egrep -v "(-o|hello)" | grep -v "libmain.a" | while read a; do
	if [[ -f $BUILDDIR/$a ]]; then
		echo $(realpath $BUILDDIR/$a)
	elif [[ "$a" = "esp8266"*".ld" ]]; then
		find $IDF_PATH $BUILDDIR -name "$a" | head -1
		echo find $IDF_PATH $BUILDDIR -name "$a" 1>&2
		#echo $a
	elif [[ "$a" = *"xtensa-lx106-elf-g++"* ]]; then
		echo -n
	else
		echo $a
	fi
done | tr "\n" " " > $BUILDDIR/LFLAGS.txt

echo set\(CORE_CFLAGS $(cat /tmp/esp82266/core/CFLAGS.txt)\) >  /tmp/esp82266/core/core.cmake
echo set\(CORE_LFLAGS \"SHELL:$(cat /tmp/esp82266/core/LFLAGS.txt)\"\) >> /tmp/esp82266/core/core.cmake
