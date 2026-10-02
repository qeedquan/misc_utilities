/*

https://marctenbosch.com/quaternions/
https://en.wikipedia.org/wiki/Bivector
https://en.wikipedia.org/wiki/Geometric_algebra
https://www.shapeoperator.com/2019/01/07/relating-dot-wedge/

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
  };
}

void draw() {
  background(200, 200, 200);
  drawAxes();
  
  stroke(200, 0, 0);
  drawVector(vectors[0]);
  stroke(0, 0, 200);
  drawVector(vectors[1]);

  PVector refl = lnreflect(vectors[0], vectors[1]);
  stroke(30, 50, 80);
  drawVector(refl);
  
  refl = gmreflect(vectors[0], vectors[1]);
  refl.mult(0.5);
  stroke(30, 150, 200);
  drawVector(refl);

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
  vectors[i] = new PVector(mouseX-width/2, mouseY-height/2);
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

PVector makeVector(float magnitude, float angle) {
  return new PVector(magnitude*cos(angle), magnitude*sin(angle));
}

PVector normalize(PVector a) {
  PVector n = a.copy();
  n.normalize();
  return n;
}

PVector scale(PVector a, float s) {
  PVector p = a.copy();
  p.mult(s);
  return p;
}

PVector gmmulvv(PVector a, PVector b) {
  float dot = a.x*b.x + a.y*b.y;
  float wedge = a.x*b.y - a.y*b.x;
  return new PVector(dot, wedge);
}

PVector gmmulvsb(PVector a, PVector B) {
  return new PVector(a.x*B.x - a.y*B.y, a.y*B.x + a.x*B.y);
}

// The geometric product formulation of a reflected vector at axis a is r = -ava
PVector gmreflect(PVector a, PVector v) {
  a = normalize(a);
  PVector p = gmmulvsb(a, gmmulvv(v, a));
  p = scale(p, -1);
  return p;
}

// The linear algebra formulation of a reflected vector is v - a*[2*dot(v, a)/dot(a, a)]
PVector lnreflect(PVector a, PVector v) {
  float s = 2 * v.dot(a) / a.dot(a);
  PVector p = PVector.sub(v, scale(a, s));
  return p;
}
