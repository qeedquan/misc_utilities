# https://ximera.osu.edu/mooculus/calculus3/shapeOfThingsToCome/digInDrawingAMobiusStrip
set hidden3d
set isosamples 64, 64
set parametric
splot (1 + u*cos(v/2))*cos(v), (1 + u*cos(v/2))*sin(v), u*sin(v/2)
pause -1
