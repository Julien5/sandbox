#!/usr/bin/env bash

# starts with systemd.
# do not background like
cd /opt/hamster-server
nohup python3 server.py &

