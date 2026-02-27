#!/bin/sh

usage() {
	echo "Usage: <log level>"
	echo "Available levels: error warning info trace"
	exit 2
}

DEBUG_DIR=/sys/kernel/debug/vchiq/log
LOG_LEVEL="trace"
while [ "$#" -gt 0 ]
do
	key="$1"
	case "$key" in
		-h|--help)
			usage
			;;
		error|warning|info|trace)
			LOG_LEVEL="$key"
			shift
			;;
		*)
			echo "Unknown log level"
			exit 1
			;;
	esac
done

for i in $(ls $DEBUG_DIR/*)
do
	echo "$i - Setting log level to $LOG_LEVEL"
	echo "$LOG_LEVEL" > $i
done
