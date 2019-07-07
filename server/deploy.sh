#!/bin/bash

set -e
SERVER=pi
rm -vf /tmp/*.zip
zip -r /tmp/server.zip *.sh *.py html
scp /tmp/server.zip julien@$SERVER:/tmp/
pushd ../esp-01/hello
make clean
pushd swig
make clean
popd
zip -r /tmp/swig.zip *
scp /tmp/swig.zip  julien@$SERVER:/tmp/
popd
ssh julien@$SERVER "if [ ! -d /opt/hamster-server ]; then echo please mkdir -p /opt/hamster-server/ \(+chmod\) on $SERVER; fi"
scp julien@$SERVER:/opt/hamster-server/sqlite.db backup/sqlite.$(date +%Y-%m-%d-%H.%M.%S).db
scp install_server.sh julien@$SERVER:/tmp/
ssh julien@$SERVER "/tmp/install_server.sh"
