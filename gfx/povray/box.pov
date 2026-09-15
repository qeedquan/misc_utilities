#include "colors.inc"
#include "stones.inc"

box {
	<-2, 0, -1>
	<1, 0.5, 3>
	texture {
		T_Stone25
		scale 4
	}

	rotate y*20
	translate <-1, 0, 0>
}

light_source { <3, 3, 3> White }

camera {
	look_at <0, 0, 0>
	location <1, 2, 5>
	rotate <0, 90, 0>
}
