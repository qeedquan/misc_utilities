#include "colors.inc"

camera {
    location <0, .1, -25>
    look_at 0
    angle 30
}

background { color Gray50 } // to make the torus easy to see
light_source { <300, 300, -1000> White }
torus {
    4, 1              // major and minor radius
    rotate -90*x      // so we can see it from the top
    pigment { Green }
}

#declare Half_Torus = difference {
    torus {
        4, 1
        rotate -90*x  // so we can see it from the top
    }
    box { <-5, -5, -1>, <5, 0, 1> }
    pigment { Green }
}
#declare Flip_It_Over = 180*x;
#declare Torus_Translate = 8;  // twice the major radius

union {
    object { Half_Torus }
    object { Half_Torus
        rotate Flip_It_Over
        translate Torus_Translate*x
    }
}

union {
    object { Half_Torus }
    object { Half_Torus
        rotate Flip_It_Over
        translate x*Torus_Translate
    }
    object { Half_Torus
        translate x*Torus_Translate*2
    }
    object { Half_Torus
        rotate Flip_It_Over
        translate x*Torus_Translate*3
    }
    object { Half_Torus
        rotate Flip_It_Over
        translate -x*Torus_Translate
    }
    object { Half_Torus
        translate -x*Torus_Translate*2
    }
    object { Half_Torus
        rotate Flip_It_Over
        translate -x*Torus_Translate*3
    }
    object { Half_Torus
        translate -x*Torus_Translate*4
    }
    rotate y*45
    translate z*20
}

