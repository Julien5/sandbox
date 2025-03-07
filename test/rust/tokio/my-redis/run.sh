#!/usr/bin/env bash

function tokio() {
	cd /home/julien/projects/sandbox/test/rust/tokio/my-redis/
	dev.rust.desktop
	# https://tokio.rs/tokio/tutorial/spawning
	cargo run --bin server &
	sleep 1
	cargo run --bin client
}

function main() {
	tokio
}

main "$@"
echo all good
