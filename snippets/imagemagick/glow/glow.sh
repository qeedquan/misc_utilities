#!/bin/sh

cat << EOF >/dev/null

https://github.com/ImageMagick/ImageMagick/discussions/5048

Read the image.
Extract the alpha channel and process it as follows:
Flood fill the center with white (so you have a white filled rectangle on black background)
Blur it and stretch appropriately to remove the blur effect inside the rectangle
Flood fill the center with black
Add that to the original alpha channel
Put the result into the alpha channel of the input replacing the old alpha channel
Save the result

EOF

convert red_rect.png \
\( -clone 0 -alpha extract \) \
\( -clone 1 -fill white -draw "color 110,175 floodfill" -alpha off \
-blur 0x7 -level 0x50% \
-fill black -draw "color 110,175 floodfill" -alpha off \) \
\( -clone 2,1 -compose plus -composite \) \
-delete 1,2 \
-alpha off -compose copy_opacity -composite \
red_rect3.png
