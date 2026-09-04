/*

http://geocalc.clas.asu.edu/GA_Primer/GA_Primer/introduction-to-geometric/rotors-and-rotations-in-the.html
https://bitworking.org/news/ga/2d
https://marctenbosch.com/quaternions/
https://en.wikipedia.org/wiki/Bivector
https://en.wikipedia.org/wiki/Geometric_algebra

*/

PVector[] vectors;

void setup() {
  size(1024, 768);
  strokeWeight(2);
  textSize(16);
  
  float magnitude = 300;
  vectors = new PVector[]{
    makeVector(magnitude, radians(-45)),
    makeVector(magnitude, radians(-120)),
    makeVector(magnitude, radians(-60)),
  };
}

void draw() {
  background(200, 200, 200);
  drawAxes();
  
  stroke(200, 0, 0);
  drawVector(vectors[0]);
  stroke(0, 0, 200);
  drawVector(vectors[1]);
  stroke(0, 0, 0);
  drawVector(vectors[2]);
  
  float angleBasisAxis = PVector.angleBetween(vectors[0], vectors[1]);
  float angleTestVector = getVectorAngle(vectors[2]);
  
  stroke(0);
  text("Angle of Basis Axis: " + degrees(angleBasisAxis), 16, 16);
  text("Current Angle of Test Vector: " + degrees(angleTestVector), 16, 32);

  if (mousePressed)
    mousePressed();
}

void drawVector(PVector vec) {
  arrow(width/2, height/2, width/2+vec.x, height/2+vec.y, 10);
}

void drawAxes() {
  float tz = 10;
  stroke(0, 0, 0);
  fill(0, 0, 0);
  arrow(0, height/2, width-tz-5, height/2, tz);
  arrow(width/2, height, width/2, tz+5, tz);
}

void mousePressed() {
  int i = 0;
  if (mouseButton == RIGHT)
    i = 1;
  else if (mouseButton == CENTER)
    i = 2;
  vectors[i] = new PVector(mouseX-width/2, mouseY-height/2);
}

void keyPressed() {
  PVector v1 = vectors[0].copy().normalize();
  PVector v2 = vectors[1].copy().normalize();
  PVector ev = gmmulvv(v1, v2);
  PVector er = new PVector(ev.x, -ev.y);
  if (key == CODED) {
    if (keyCode == RIGHT)
      vectors[2] = gmmulvsb(vectors[2], ev);
    else if (keyCode == LEFT)
      vectors[2] = gmmulvsb(vectors[2], er);
  }
}

void arrow(float x1, float y1, float x2, float y2, float tz) {
  line(x1, y1, x2, y2);
  
  float ang = atan2(y2-y1, x2-x1) + radians(270);
  pushMatrix();
  translate(x2, y2);
  rotate(ang);
  triangle(-tz, 0, tz, 0, 0, tz);
  popMatrix();
}

/*

The geometric product between 2 vectors is defined as follows in 2D:
ab = dot(a, b) + wedge(a, b)
vector * vector = scalar+bivector
Where wedge(a, b) is equal to the determinant of a 2x2 matrix [[a1 b1] [a2 b2]] in 2D

There is only one plane in 2D (The XY plane), hence there is only one bivector component
Any bivector can be written as a scalar scaling the basis plane, in 2D that is
B_xy*(x ^ y) where B_xy is the signed area (determinant) of that plane

Any shape that has that signed area is considered the same to the plane in the bivector sense but we use a plane for convention

A unit multivector is called a rotor and it can be used to represent rotations. It is isomorphic to the complex numbers in 2D and isomorphic
to quaternions in 3D in the sense that they can be used to derive the same formulas one would get working purely with complex
numbers and quaternions but they give a different conceptual way of thinking about things that can generalize to N dimensions.

In 2D, we use the geometric products of unit vectors to present a rotor, since unit vector dot product and wedge product gives
cos(t) and sin(t) respectively, we can view them as a rotation parameter

ab = dot(a, b) + wedge(a, b) = cos(t) + I*sin(t) = exp(I*t) hence they can represent the complex exponentials

A scalar+bivector multiplied with another scalar+bivector gives a scalar+bivector due to the identity of complex exponentials

(ab)(dc) = exp(I*t)*exp(I*s) = exp(I*(t+s))
scalar+bivector * scalar+bivector = scalar+bivector

The scalar and bivector components can be used to multiply another vector to rotate that vector
by some angle encoded in the scalar+bivector (the angle that the vector is rotated by is the angle between the
original axis vectors that created the scalar+bivector using the geometric product)

In 2D e1 is the X axis and e2 is the Y axis
aB = (a1*e1 + a2*e2) * (B1 + B2e12) = (a1*b1 - a2*b2)e1 + (a2*b1 + a1*b2)e2
vector * scalar+bivector = vector

*/

// given two vectors, return a scalar+bivector that encodes a rotation angle between the 2 vectors
PVector gmmulvv(PVector a, PVector b) {
  float dot = a.x*b.x + a.y*b.y;
  float wedge = a.x*b.y - a.y*b.x;
  return new PVector(dot, wedge);
}

// given a vector and a scalar+bivector, return a rotated vector that is rotated by
// the angle encoded inside scalar+bivector
PVector gmmulvsb(PVector a, PVector B) {
  return new PVector(a.x*B.x - a.y*B.y, a.y*B.x + a.x*B.y);
}

float getVectorAngle(PVector v) {
  float r = atan2(v.y, v.x);
  if (r < 0)
    r += 2*PI;
  return 2*PI - r;
}

PVector makeVector(float magnitude, float angle) {
  return new PVector(magnitude*cos(angle), magnitude*sin(angle));
}
