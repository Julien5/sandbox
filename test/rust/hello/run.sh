#!/usr/bin/env bash

. $HOME/projects/config/profile/profile.sh
rm -Rf target
ls -l
dev.rust.desktop
# export CARGO_TARGET_DIR=/tmp/delme/rust-targets
cargo build
cargo run
cargo test
cargo doc # --open 
sleep 1

