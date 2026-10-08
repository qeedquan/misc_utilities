#!/bin/sh

magick -background lightblue -fill blue -font Corsiva -pointsize 36 \
	-size 320x caption:'This is a very long caption line.' \
	caption.gif

magick -background lightblue  -fill blue  -font Candice -pointsize 40 \
        -size 320x  -gravity Center  caption:'ImageMagick Rules OK!' \
        caption_centered.gif


magick -background lightblue -fill blue \
        -font Ravie -pointsize 24 -size 360x \
        caption:"Here I use caption to wordwrap.\nTwo separate lines." \
        caption_multi_line.gif



magick -pointsize 48 -font WebDings label:' " _ ~ ) - '  label_webdings.gif
magick -pointsize 48 -font LittleGidding label:' x o w ' label_ltgidding.gif
magick -pointsize 48 -font WingDings2      label:'ab'    label_wingdings2.gif
magick -pointsize 48 -font Zymbols  label:' ? , - I Z '  label_zymbols.gif
magick -pointsize 48 -font TattoEF  label:' B Y D I H '  label_tatooef.gif
magick -pointsize 48 -font SoundFX  label:' V 3 t f 9 '  label_soundfx.gif
