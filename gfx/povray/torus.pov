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