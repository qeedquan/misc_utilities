#!/bin/sh

# XC stands for X constant image
convert -size 100x100 'xc:red' red.png
convert -size 1920x1080 'xc:#00ff00' green.png
convert -size 100x100 'xc:rgba(255,0,255,0.5)' pink.png

# generate a gradient
convert -size 400x200 gradient: gradient.png

