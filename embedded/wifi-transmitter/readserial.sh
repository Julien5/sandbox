#!/usr/bin/env bash

set -e


stty -F /dev/ttyUSB0 9600   # sets the speed of the port
exec 99<>/dev/ttyUSB0

echo "sending command."
cat /tmp/serial.write >&99

echo "reading response."
while true; do
    read d <&99 || true
    echo -n $d | xxd 
    sleep 0.25;
done
exec 99>&-
