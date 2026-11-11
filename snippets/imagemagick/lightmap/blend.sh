#!/bin/sh

# https://www.slembcke.net/blog/2DLightingTechniques/
# Screenspace technique, given a lightmap multiply it with the input to get the output
# Pros:
# Very easy to implement using offscreen rendering and a couple blend modes.
# Doesn't complicate 2D rendering.
# Easy to extend with shadows.
# Extremely fast.
# Cons:
# “Lighting” is somewhat generous. You just get color * light.
# Doesn't work with normal mapping.
# Strictly a 2D effect, and may not work well in a “2.5D” game.
magick input.png lightmap.png -compose multiply -composite blended.png

