#version 3.6;
#include "colors.inc"

global_settings { assumed_gamma 1.0 }

background   { color rgb <0.25, 0.25, 0.25> }

camera {
    location  <0.0, 0.5, -4.0>
    direction 1.5*z
    right     x*image_width/image_height
    look_at   <0.0, 0.0, 0.0>
}
 
light_source {
    <0, 0, 0>
    color rgb <1, 1, 1>
    translate <-5, 5, -5>
}

light_source {
    <0, 0, 0>
    color rgb <0.25, 0.25, 0.25>
    translate <6, -6, -6>
}


#declare a = 0;

#while (a < 360)
    box {
        <-0.5, -0.5, -0.5>
 		<0.5, 0.5, 0.5>
        texture { pigment { color Red }
                  finish  { specular 0.6 }
                  normal  { agate 0.25 scale 1/2 }
        }
 		rotate a
    }
 	#declare a = a + 45;
#end
