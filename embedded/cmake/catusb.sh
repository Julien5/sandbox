#/usr/bin/env bash

function catusb() {
	for sysdevpath in $(find /sys/bus/usb/devices/usb*/ -name dev); do
    syspath="${sysdevpath%/dev}"
    if [[ "$syspath" == *"tty"* ]]; then
		path=$(udevadm info -q name -p $syspath)
		infos=$(udevadm info -q property --export -p $syspath | grep DATABASE | cut -f2 -d"=" | tr "\n" " ")
		printf "%s:%s\n" "$path" "$infos"
	fi
	done
}
