#!/bin/sh

# https://kalitut.com/raspberry-pi-gpio-assignment/
# https://kalitut.com/control-gpio-pigpiod-Commands/
# sudo systemctl enable pigpiod
# sudo systemctl start pigpiod

# RPI4-B latencies measured on Salae Logic using this script
# Hook the GPIO pin into the channel using regular jumpers
# Don't need to hook to any ground pins, just gpio <-> data channel

# No delays
# 1 PIN:
#     4 ms pulse width for each GPIO pin
#     8-12 ms between toggles
# 3 PIN:
#     4 ms pulse width for each GPIO pin
#     24-36 ms between toggles (scales linearly with number of pins)
# The pulse width is also not stable, some wide and some narrow over time

set -e

delay=0

usage() {
	echo "Usage: [-d delay] <pin> ..."
	exit 2
}

while getopts "d:h" o; do
	case "${o}" in
	d)
		delay=${OPTARG}
		;;
	*)
		usage
		;;
	esac
done

shift $((OPTIND-1))

if [ "$#" -lt 1 ]; then
	usage
fi

for pin in "$@"
do
	pigs modes $pin w
done

while true
do
	for pin in "$@"
	do
		pigs w $pin 1
		pigs w $pin 0
	done
	sleep $delay
done

