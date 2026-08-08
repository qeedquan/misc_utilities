/*

https://mathworld.wolfram.com/OrthogonalCircles.html
https://www.geogebra.org/m/S5xyRmtZ
https://www.cut-the-knot.org/Curriculum/Geometry/GeoGebra/OrthogonalCircle.shtml
http://cs.uef.fi/matematiikka/kurssit/HyperbolicGeometry/HyperbolicGeometryLehmanMarsApril2014.pdf

*/

PVector circular;
PVector angle;
boolean animating;

void setup() {
  size(1024, 768);
  strokeWeight(3);
  
  circular = new PVector(width/2, height/2, 100);
  angle = new PVector(radians(0), radians(45));
}

void draw() {
  background(200);
  stroke(200, 160, 80);
  drawTangentLine(circular, angle.x);
  stroke(30);
  drawTangentLine(circular, angle.y);
  drawCirculars(circular, PVector.mult(angle, 1));
  if (animating)
    angle.y += radians(1);
  angle.x %= TWO_PI;
  angle.y %= TWO_PI;
}

void keyPressed() {
  if (key == ' ')
    animating = !animating;
  else if (keyCode == LEFT)
    angle.x -= radians(1);
  else if (keyCode == RIGHT)
    angle.x += radians(1);
}

void mouseWheel(MouseEvent ev) {
  circular.z *= exp(0.2*ev.getCount());
}

/*

Two circles are orthogonal/perpendicular to each other if they intersect at right angles

They satisfy the following relation:
r^2 + R^2 = d^2 where d is the distance of the two circle center point and r/R are the radius of the two circles.

There are a couple of formulation of a orthogonal circle:

1) Given a circle with center A and a arbitrary point C, construct a orthogonal circle
This formulation defines fixes the tangent point from the center A to the boundary of the circle called point T.
The point C will then connect to the point T to make a line we will call G.
The intersection of line G and the tangent line at the tangent point of the circle with center A is the center
point of our orthogonal circle.

2) Given a circle with center A and two points on the boundary of the circle which we call B and C,
   construct an orthogonal circle.

This formulation is the one we implement.
We can generate point B and C using the parametric equation <cos(t), sin(t)> at 2 user chosen angles.
We can use the fact that the tangent vector is the first derivative of the parametric equation <-sin(t), cos(t)>.
Generate 2 lines at point B and C using the boundary point position and the tangent vector at that position.
The intersection will give us the center point of the orthogonal circle.
Finally, the orthogonal circle radius can be found using the relation r^2 + R^2 = d^2.

*/

void drawCirculars(PVector c, PVector a) {
  PVector[] u = getTangentLine(c, a.x, width);
  PVector[] v = getTangentLine(c, a.y, width);
  PVector o = lineIntersection(u[0], v[0], u[1], v[1]);
  float d = dist(c.x, c.y, o.x, o.y);
  float or = sqrt(d*d - c.z*c.z);
  
  // reference circle
  noFill();
  stroke(0, 0, 255);
  circle(c.x, c.y, 2*c.z);
  
  // orthogonal circle
  stroke(0, 200, 0);
  circle(o.x, o.y, 2*or);
  
  // markers
  stroke(80);
  float r = 15;
  fill(180, 230, 100, 255);
  circle(c.x + c.z*cos(a.x), c.y + c.z*sin(a.x), r);
  fill(210, 80, 170, 255);
  circle(c.x + c.z*cos(a.y), c.y + c.z*sin(a.y), r);
  fill(50, 200, 180);
  circle(c.x, c.y, r);
  
  circle(o.x, o.y, r);
}

void drawTangentLine(PVector c, float t) {
  PVector[] r = getTangentLine(c, t, width);
  line(r[0].x-r[1].x, r[0].y-r[1].y, r[0].x+r[1].x, r[0].y+r[1].y);
}

PVector[] getTangentLine(PVector c, float t, float l) {
  float co = cos(t);
  float si = sin(t);
  
  PVector[] r = new PVector[2];
  r[0] = new PVector(c.x + c.z*co, c.y + c.z*si);
  r[1] = new PVector(-si*l, co*l);
  return r;
}

PVector lineIntersection(PVector p1, PVector p2, PVector d1, PVector d2) {
  float t = ((p2.y - p1.y) * d1.x - (p2.x - p1.x) * d1.y) / (d2.x * d1.y - d2.y * d1.x);
  return new PVector(p2.x + t*d2.x, p2.y + t*d2.y);
}
