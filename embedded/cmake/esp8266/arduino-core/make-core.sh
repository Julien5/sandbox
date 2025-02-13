#!/usr/bin/env bash

set -e
# set -x

function init() {
	SCRIPTDIR=$(realpath $(dirname $0))
	. $HOME/.profile
}

function fixup() {
	R1=https://raw.githubusercontent.com/esp8266/Arduino/refs/heads/master
	R2=https://raw.githubusercontent.com/alexCajas/esp8266RTOSArduCore/refs/heads/main

	for a in pgmspace.h Print.h Print.cpp; do 
		wget "${R1}/cores/esp8266/${a}" -O ${COREDIR}/cores/esp8266/${a} &> /dev/null
	done 

	rm -Rf /tmp/patch
	mkdir -p /tmp/patch/include/sys/
	cp $SCRIPTDIR/pgmspace.h /tmp/patch/include/sys/
	touch /tmp/patch/include/sys/_tz_structs.h

	#wget "${R2}/cores/esp8266/pgmspace.h" -O /tmp/patch/include/sys/pgmspace.h &> /dev/null
	#wget "${R2}/cores/esp8266/Print.h" -O /tmp/patch/include/Print.h &> /dev/null
}

function test-esp8266() {
	#rm -Rf /tmp/builds/esp8266/
	dev.esp8266
	#dev.esp8266.compiler 10.3.0-arduino
	cd ~/delme
	EXE=$HOME/projects/sandbox/embedded/cmake/esp8266/arduino-core/cmakify/src/main.py
	${EXE} --board nodemcu --key recipe.cpp.o.pattern ${COREDIR}
	echo
	${EXE} --board nodemcu --key recipe.c.o.pattern ${COREDIR}
	for k in FLAGS DEFINES INCLUDES; do
		echo "* C ${k}"
		${EXE} --board nodemcu --key recipe.c.o.pattern --filter ${k} ${COREDIR}
		echo "* CPP ${k}"
		${EXE} --board nodemcu --key recipe.cpp.o.pattern --filter ${k} ${COREDIR}
		echo
	done

	${EXE} --board nodemcu --key compiler.c.cmd ${COREDIR}

	${EXE} --board nodemcu --key compiler.cpp.cmd ${COREDIR}
	${EXE} --board nodemcu --key recipe.c.combine.pattern ${COREDIR}
	${EXE} --board nodemcu --toolchain ${COREDIR}

	SDFATROOTDIR=/tmp/x/sdfat/
	if [ ! -d ${SDFATROOTDIR} ]; then
		mkdir -p ${SDFATROOTDIR}
		pushd ${SDFATROOTDIR}
		wget https://github.com/greiman/SdFat/archive/refs/tags/2.2.3.zip
		unzip 2.2.3.zip
		ln -s SdFat-2.2.3 SdFat
		popd
	fi
	SDFATDIR=${SDFATROOTDIR}/SdFat
	#SDFATDIR=$HOME/delme/SdFat

	${EXE} --board nodemcu --lib ${COREDIR}/libraries/SPI ${COREDIR}
	${EXE} --board nodemcu --lib ${SDFATDIR} ${COREDIR}

	rm -Rf /tmp/cmake-test
	mkdir -p /tmp/cmake-test
	cd /tmp/cmake-test
	mkdir -p {toolchain,core,SPI,sdfat}
	${EXE} --board nodemcu --toolchain ${COREDIR} > toolchain/toochain.cmake
	cp ${SCRIPTDIR}/root.cmake CMakeLists.txt
	for d in core SPI sdfat; do
		printf "add_subdirectory(%s)\n" "${d}" >> CMakeLists.txt
	done
	fixup
	COREDEFINES=$(${EXE} --board nodemcu --key recipe.cpp.o.pattern ${COREDIR} --filter DEFINES | cut -b3- | xargs)
	COREDEFINES=$COREDEFINES" ""$(${EXE} --board nodemcu --key menu.ip.lm2f.build.lwip_flags ${COREDIR} --filter DEFINES | cut -b3- | xargs)"
	COREDEFINES="$COREDEFINES ""$(${EXE} --board nodemcu --key menu.mmu.3232.build.mmuflags ${COREDIR} --filter DEFINES | cut -b3- | xargs)"
	COREINCLUDES="$(${EXE} --board nodemcu --key recipe.cpp.o.pattern --filter INCLUDES ${COREDIR} | cut -b3- | xargs)"
	COREINCLUDES="$COREINCLUDES ${COREDIR}/cores/esp8266  ${COREDIR}/libraries/SPI   ${COREDIR}/libraries/Wire"

	LWIPINCLUDES=${COREDIR}/tools/sdk/"$(${EXE} --board nodemcu --key menu.ip.lm2f.build.lwip_include ${COREDIR})"
	#  /opt/esp8266-toolchain/ESP8266_RTOS_SDK/components/lwip/port/esp8266/include 
	${EXE} --board nodemcu --lib ${COREDIR}/cores/esp8266 ${COREDIR} \
		   --add-include-directories \
		   /tmp/patch/include \
		   $COREINCLUDES \
		   ${COREDIR}/variants/nodemcu \
		   $LWIPINCLUDES \
		   --add-defines ${COREDEFINES} \
		   > core/CMakeLists.txt
	${EXE} --board nodemcu --lib ${COREDIR}/libraries/SPI ${COREDIR} > SPI/CMakeLists.txt
	${EXE} --board nodemcu --lib ${SDFATDIR} ${COREDIR} \
		   --add-include-directories \
		   /tmp/patch/include \
		   $COREINCLUDES \
		   ${COREDIR}/variants/nodemcu \
		   --add-link-libraries esp8266 \
		   > sdfat/CMakeLists.txt
	mkdir -p build
	#rm -Rf build/*
	dev.esp8266
	cmake -S . -B build --toolchain toolchain/toochain.cmake
	cd build
	2>&1 make -j8 SdFat # VERBOSE=1
}

function main() {
	COREDIR=$HOME/delme/Arduino-master
	COREDIR=/tmp/x/Arduino-3.1.2
	if [ ! -d ${COREDIR} ]; then
		mkdir -p /tmp/x
		pushd /tmp/x
		wget https://github.com/esp8266/Arduino/archive/refs/tags/3.1.2.zip
		unzip 3.1.2.zip
		popd
	fi	
	test-esp8266
	#download-esp8266-arduino-core
	#compare
}

init
main "$@"
echo all good
