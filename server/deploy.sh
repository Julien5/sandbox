#!/bin/bash

set -e
set -x 
SERVER=pi

if [[ $(hostname) = "$SERVER" ]]; then
	echo i am on the server
	pwd
	cd /tmp
	if [[ ! -f server.tar ]]; then
	   echo could not find server.tar
	   exit 1;
	fi
	mkdir -p /opt/{backup,hamster-server}
	tar zcvf /opt/backup/hamster-server.$(date "+%d.%m.%Y.%H.%M.%S").tgz /opt/hamster-server
	pushd /opt/hamster-server
	find . -type f -not -name "*.db" -and -not -name "pid" -delete -print
	find . -type d -empty -print -delete 
	tar xvf /tmp/server.tar
	./start_server.sh
	cat pid
	popd
	df -h --type=ext4
else
	tar cvf /tmp/server.tar $(find . -name "*.py") start_server.sh
	scp deploy.sh julien@$SERVER:/tmp/
	scp /tmp/server.tar julien@$SERVER:/tmp/
	ssh julien@$SERVER "/tmp/deploy.sh"
fi
echo bye
