#!/bin/sh

# use socat to make serve virtual tty device 
# this uses the /dev/pts/* devices
# it automatically cleans up the device when socat quits
socat -d -d pty,raw,echo=0 pty,raw,echo=0
