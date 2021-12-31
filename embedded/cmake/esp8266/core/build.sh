#!/usr/bin/env bash

set -e
# set -x

BUILDDIR=/tmp/esp82266/core;
FILENAME=$BUILDDIR/stdout
mkdir -p $BUILDDIR;
#rm -f $FILENAME
if [[ ! -f $FILENAME ]]; then
	rm -Rf $BUILDDIR;
	echo cmaking core for esp8266
	echo CONFIG_COMPILER_OPTIMIZATION_LEVEL_RELEASE=y >> sdkconfig
	cmake -S $IDF_PATH/examples/get-started/hello_world -B $BUILDDIR
	echo generating core for esp8266 
	make -C $BUILDDIR VERBOSE=1 &> $FILENAME 
else
	echo processing $FILENAME
fi

echo grabing CFLAGS 
cat $FILENAME | grep hello_world_main.c | grep "\-o" | tr " " "\n" | grep "^\-" | egrep -v "(-o|-c|-Werror=all|-std=gnu99)" | tr "\n" " " > $BUILDDIR/CFLAGS.txt

echo grabing LFLAGS
qopened=false
cat $FILENAME | grep hello-world.elf | grep g++ | grep "\-o" | tr " " "\n"  | grep -v "^$" | egrep -v "(-o|hello)" | grep -v "xxxlibmain.a" | while read a; do
	if [[ -f $BUILDDIR/$a ]]; then
		echo $(realpath $BUILDDIR/$a)
	elif [[ -f $BUILDDIR/$a ]]; then
		echo $(realpath $BUILDDIR/$a)
	elif [[ "$a" = \-u ]]; then
		echo \"SHELL:$a
		qopened=true
	elif [[ "$a" = \-L ]]; then
		echo \"SHELL:$a
		qopened=true
	elif [[ "$a" = "esp8266"*".ld" ]]; then
		find $IDF_PATH $BUILDDIR -name "$a" | head -1
		echo find $IDF_PATH $BUILDDIR -name "$a" 1>&2
		#echo $a
	elif [[ "$a" = *"xtensa-lx106-elf-g++"* ]]; then
		echo -n
	else
		if $($qopened); then
			# close
			echo $a"\" "
			qopened=false
		else
			echo $a
		fi
	fi
done | tr "\n" " " > $BUILDDIR/LFLAGS.txt

echo set\(CORE_CFLAGS $(cat /tmp/esp82266/core/CFLAGS.txt)\) >  /tmp/esp82266/core/core.cmake
echo set\(CORE_LFLAGS $(cat /tmp/esp82266/core/LFLAGS.txt)\) >> /tmp/esp82266/core/core.cmake
