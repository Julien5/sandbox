#!/usr/bin/env bash

set -e
# set -x

if [[ -z $OBJSDIR ]]; then
    OBJSDIR=/tmp/build/esp8266/core
fi

# rm -Rf $OBJSDIR
mkdir -p $OBJSDIR/include/config;
cd $OBJSDIR/

if [[ -z $IDF_PATH ]]; then
    echo IDF_PATH undefined
    exit
fi

export KCONFIG_AUTOHEADER=$OBJSDIR/include/sdkconfig.h
export COMPONENT_KCONFIGS="$(find $IDF_PATH/components/ -name "Kconfig")"
export KCONFIG_CONFIG=$HOME/projects/sandbox/embedded/Makefiles/esp8266/sdkconfig
export COMPONENT_KCONFIGS_PROJBUILD="$(find $IDF_PATH/components/ -name "Kconfig.projbuild" | grep -v mqtt)"

export IDF_CMAKE=n

$IDF_PATH/tools/kconfig/conf-idf --silentoldconfig $IDF_PATH/Kconfig;
