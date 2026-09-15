#!/bin/sh

# https://imagemagick.org/fx/
# fx is a way to do math on every pixel input (kind of like a pixel shader)

# can be used like a calculator
# mod(13, 10)
echo $(convert xc: -format "%[fx:mod(8,2)]" info:)
# sin(pi/4)
echo $(convert xc: -format "%[fx:sin(pi/4)]" info:)
# max(10, 30)
echo $(convert xc: -format "%[fx:max(10, 30)]" info:)


