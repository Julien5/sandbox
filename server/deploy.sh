#!/bin/bash

set -e
set -x 
SERVER=pi

function backup() {
	echo backup
	ssh julien@$SERVER "tar cvf /opt/backup/hamster-server.$(date +%Y.%m.%d-%H.%M.%S).tgz /opt/hamster-server/"
	scp julien@$SERVER:/opt/hamster-server/sqlite.db backup/sqlite.$(date +%Y.%m.%d-%H.%M.%S).db
}

TARGET=/tmp/server
TARBALL=server.tar
function copy() {
	rm -f $TARBALL
	tar -cvf $TARBALL --exclude *.db --exclude backup --exclude systemd --exclude __pycache__ *
	ssh julien@$SERVER "rm -Rf $TARGET"
	ssh julien@$SERVER "mkdir -p $TARGET"
	scp $TARBALL julien@$SERVER:$TARGET
	rm $TARBALL
}

function deploy() {
	ssh julien@$SERVER "tar xvf $TARGET/$TARBALL -C /opt/hamster-server/"
	ssh julien@$SERVER "cd /opt/hamster-server/; ./start_server.sh"
}

function main() {
	backup
	copy
	deploy
}

main
