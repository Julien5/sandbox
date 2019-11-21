#!/bin/bash

set -e

rm -Rvf /tmp/build
mkdir -p /tmp/build
pushd /tmp/build

DST=/opt/hamster-server
mkdir -p $DST
unzip /tmp/server.zip
cp -v *.py *.sh $DST/
mkdir -p $DST/html
cp -v html/*.* $DST/html/

unzip /tmp/swig.zip
pushd swig
make
popd

#sudo cp hamster-server.service  /etc/systemd/user
#sudo chmod u+r /etc/systemd/user/hamster-server.service
#sudo systemctl enable hamster-server
#sudo systemctl start  hamster-server

popd

#cd $DST
#./start_server.sh &> start_server.log &
