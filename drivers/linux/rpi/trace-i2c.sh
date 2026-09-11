#!/bin/sh

set -e

usage() {
	echo "usage: on/off"
	exit 2
}

toggle() {
	cd /sys/kernel/debug/tracing
	echo nop > current_tracer
	echo $1 > events/i2c/enable
	echo $1 > tracing_on
}

if [ "$1" = "on" ]
then
	toggle 1
	echo "tracing on: check /sys/kernel/debug/tracing/trace"
elif [ "$1" = "off" ]
then
	toggle 0
	echo "tracing off"
else
	usage
fi

