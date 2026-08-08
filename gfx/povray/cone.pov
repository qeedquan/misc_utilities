#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"

cone {
	<0, 1, 0>, 0.3    // Center and radius of one end
	<1, 2, 3>, 1.0    // Center and radius of other end
	open
	texture { T_Stone25 scale 4 }
}

cylinder {
	<0, 1, 0>,     // Center of one end
	<1, 2, 3>,     // Center of other end
	0.5            // Radius
	open           // Remove end caps
	texture { T_Stone21 scale 4 }
}

plane { 
	y, -1
	pigment {
		checker color Black, color White
	}
}

camera {
	look_at <0, 1, 0>
	location <0, 10, 0>
}