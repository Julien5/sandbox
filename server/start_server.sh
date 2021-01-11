#!/usr/bin/env bash

set -e
set -x

cd /opt/hamster-server
if [[ -f pid ]]; then
	pid=$(cat pid)
	if ! kill $pid; then
		echo could not kill old pid $pid
	fi
else
	echo could not find pid
fi
echo "still running?"
ps -edf | grep python
nohup python3 server.py &
echo "$!" > pid

