#include "colors.inc"
#include "stones.inc"
#include "textures.inc"
#include "shapes.inc"
#include "glass.inc"
#include "metals.inc"
#include "woods.inc"

camera {
	location <0, 2, -3>
	look_at  <0, 1,  2>
}

sphere {
	<0, 1, 2>, 2
	texture {
		pigment {
			wood
			color_map {
				[0.0 color DarkTan]
				[0.9 color DarkBrown]
				[1.0 color VeryDarkBrown]
			}
			turbulence 0.05
			scale <0.2, 0.3, 1>
		}
		finish { phong 1 }
	}
}

sphere {
	<0, 1, 2>, 2
	texture {
		pigment {
			wood
			color_map {
				[0.0 color Red]
				[0.5 color Red]
				[0.5 color Blue]
				[1.0 color Blue]
			}
			scale <0.2, 0.3, 1>
		}
		finish { phong 1 }
	}
}
