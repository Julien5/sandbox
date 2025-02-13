#!/usr/bin/env bash

. $HOME/projects/config/profile/profile.sh
dev.rust.desktop
# export CARGO_TARGET_DIR=/tmp/delme/rust-targets
cargo build
cargo run
cargo test
cargo doc # --open 
sleep 1

