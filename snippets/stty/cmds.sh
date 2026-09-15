#!/bin/sh

DEV=/dev/ttyACM0

# set baud rate
BAUD=115200
stty -F $DEV $BAUD

# enable RTS/CTS flow control
stty -F $DEV crtscts

# disable RTS/CTS flow control
stty -F $DEV -crtscts

# view settings for device
stty -a -F $DEV
