#!/bin/sh

set -e

test_message() {
	STARTBITS=$1
	STOPBITS=$2
	TEXT=$3

	go run ascii2baudot.go -startbits $STARTBITS -stopbits $STOPBITS "$TEXT" > input.bin
	grcc -r rtty_encoder.grc > /dev/null

	echo -n $TEXT | minimodem -t rtty --baudot --startbits $STARTBITS --stopbits $STOPBITS -f ref.wav
	minimodem -r rtty --baudot --startbits $STARTBITS --stopbits $STOPBITS -f ref.wav
	echo
	minimodem -r rtty --baudot --startbits $STARTBITS --stopbits $STOPBITS -f output.wav
	echo "--------------------------"
}

cleanup() {
	rm -f *.wav *.bin *.py
}

test_message 1 1 "HELLO WORLD!"
test_message 1 2 "AARDVARK"
test_message 1 1.5 "WAX ON WAX OFF"
cleanup
