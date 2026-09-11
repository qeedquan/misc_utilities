#!/bin/sh

# RTTY modulation can be decoded in fldigi with the settings RTTY-45 and RTTYR
# RTTY corresponds to RTTY 45.45 bps --baudot --stopbits 1.5

# modulate data to transmit (save to a file in this case)
minimodem -t rtty -f out.wav

# demodulate data to receive (from a file in this case)
minimodem -r rtty -f out.wav
