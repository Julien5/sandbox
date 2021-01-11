#!/usr/bin/env bash

set -e
set -x

cd /opt/hamster-server
if [[ -f pid ]]; then
	pid=$(cat pid)
	kill $pid
else
	echo could not find pid
fi
echo "still running?"
ps -edf | grep python
nohup python3 server.py &
echo "$!" > pid

