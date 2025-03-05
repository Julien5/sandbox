#!/usr/bin/env bash

set -e

source ~/projects/config/profile/profile.sh 
dev.rust.desktop
# install server
# cargo install mini-redis
echo test server
killall mini-redis-server || true
sleep 1
mini-redis-server &
sleep 1
mini-redis-cli get foo
killall mini-redis-server || true

cargo new my-redis
