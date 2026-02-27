/*

https://en.wikipedia.org/wiki/Hyperbolic_geometry
https://en.wikipedia.org/wiki/Poincar%C3%A9_disk_model
https://en.wikipedia.org/wiki/Cayley%E2%80%93Klein_metric
https://www.cs.unm.edu/~joel/NonEuclid/
http://cs.uef.fi/matematiikka/kurssit/HyperbolicGeometry/BEG_Geometry.pdf
https://mphitchman.com/geometry/frontmatter.html
http://www.malinc.se/noneuclidean/en/poincaretiling.php

*/

ArrayList<PVector[]> lines;
ArrayList<Integer> colors;
PVector[] workLine;
PVector disc;
float dotsz;
int fontsz;
int state;

void setup() {
  size(1200, 800);
  fontsz = 16;
  textSize(fontsz);
  reset();
}

void reset() {
  lines = new ArrayList<PVector[]>();
  colors = new ArrayList<Integer>();
  disc = new PVector(width/2, height/2, 350);
  dotsz = 10;
  state = 0;
}

void draw() {
  background(140, 150, 180);

  push();
  strokeWeight(2);
  circle(disc.x, disc.y, 2*disc.z);
  for (int i = 0; i < lines.size(); i++) {
    PVector[] l = lines.get(i);
    color     c = colors.get(i);
    hyperbolicLine(disc, l[0], l[1], c, dotsz);
  }
  if (workLine != null) {
    hyperbolicLine(disc, workLine[0], workLine[1], color(50, 100, 180), dotsz);
    showMetric(disc, workLine[0], workLine[1]);
  }
  pop();
}

void showMetric(PVector c, PVector p, PVector q) {
  float ed = PVector.dist(p, q);
  float hda = acoshHyperbolicDistance(c, p, q);
  float hdl = logHyperbolicDistance(c, p, q);
  
  fill(60, 70, 80);
  text(String.format("Disc: [%.6f %.6f] (%.6f)", c.x, c.y, c.z), 16, 16 + fontsz*0);
  text(String.format("P:    [%.6f %.6f]", p.x, p.y), 16, 16 + fontsz*1);
  text(String.format("Q:    [%.6f %.6f]", q.x, q.y), 16, 16 + fontsz*2);
  text(String.format("Euclidean Distance: %.6f", ed), 16, 16 + fontsz*3);
  text(String.format("Hyperbolic Distance (acosh): %.6f", hda), 16, 16 + fontsz*4);
  text(String.format("Hyperbolic Distance (log): %.6f", hdl), 16, 16 + fontsz*5);
  text(String.format("Euclidean/Hyperbolic Ratio: %.6f", ed/hda), 16, 16 + fontsz*6);
}

void mouseWheel(MouseEvent ev) {
  float s = exp(0.1*ev.getCount());
  for (PVector[] l : lines) {
    for (PVector p : l) {
      p.x = (p.x - disc.x)*s + disc.x;
      p.y = (p.y - disc.y)*s + disc.y;
    }
  }
  disc.z *= s;
}

void mouseMoved() {
  if (state == 1) {
    PVector m = mouseDiscPosition();
    workLine[1].set(m.x, m.y);
  }
}

void mousePressed() {
  PVector m = mouseDiscPosition();
  switch (state) {
  case 0:
    workLine = new PVector[]{m.copy(), m.copy()};
    state = 1;
    break;
  case 1:
    workLine[1] = m;
    lines.add(workLine);
    colors.add(color(random(255), random(255), random(255)));
    workLine = null;
    state = 0;
    break;
  }
}

void keyPressed() {
  if (key == ' ')
    reset();
}

/*

There are a few models of hyperbolic geometry, we implement the Poincare disk model.
The Poincare disk model takes place inside a unit circle where the metric and shapes are defined as follows:

Metric:
The distances between points are given by the Cayley-Klein metric.
For the Poincare disk model, this means that we need the disc center and radius in addition to the two points to calculate the distance between them.

Points/Circles:
Hyperbolic points/circles are still Euclidean points/circles in terms of their definitions and what they look like.
However, the center point of a circle is not the generally the same as the Euclidean circle center due
to the fact that the length between two points are different under the hyperbolic metric.

Line:
A hyperbolic line connects two hyperbolic points, this can either be represented as follows:
  Euclidean line if the two points are on the boundary of the disc and their distances is the diameter of the disc.
  Circular arc of a circle that intersected with the disc.
Another name for this kind of shape is called a generalized circle.
This is the biggest change, we can draw all other common shapes from this.

Triangle/Square/Rectangle/Polygon/etc:
These shapes are the same in Euclidean space in the sense that they are defined as a set of points and you connect lines to them.
The difference is that we connect the points with hyperbolic lines, giving us a different geometry for them.
One notable fact is that angles of a triangle in hyperbolic space adds up to less than 180 degrees.

Historically, non-Euclidean geometry was discovered by trying to prove the parallel postulate from the other Euclid's axioms of geometry.
It turns out that the parallel postulate is independent from the other Euclid's axioms of geometry
and we can get different geometry based on how we define the parallel postulate axiom.

Hyperbolic: For any given line R and point P not on R, in the plane containing both line R and point P there are at least two distinct lines through P that do not intersect R.
Euclidean: In a plane, given a line and a point not on it, at most one line parallel to the given line can be drawn through the point.
Elliptic:  Any two lines intersect in at least one point.

The curvature of the space defines the geometry and we can characterize the classical curvature as follows:
K = -1 (Hyperbolic)
K = 0  (Euclidean)
K = 1  (Elliptic)
Riemannian geometry generalizes this notion of curvature and allow us to define much more general geometrical spaces.

To draw a line in the Poncaire disk model:

1. Let M be the midpoint of segment PQ
2. Draw line m through M perpendicular to segment PQ
3. Let P' be the inversion in the boundary circle of point P
4. Let N be the midpoint of segment PP'
5. Draw line n through N perpendicular to segment PP'
6. Let C be where line m and line n intersect.
7. Draw circle c with center C and going through P (and Q).
8. The part of circle c that is inside the disk is the hyperbolic line.

*/

void hyperbolicLine(PVector c, PVector p, PVector q, color col, float dz) {  
  PVector C = complement(c, p, q);

  float t0 = arcAngle(C, p);
  float t1 = arcAngle(C, q);
  float a0 = min(t0, t1);
  float a1 = max(t0, t1);
  
  noFill();
  if (a1-a0 >= PI) {
    arc(C.x, C.y, 2*C.z, 2*C.z, a1, 2*PI);
    arc(C.x, C.y, 2*C.z, 2*C.z, 0, a0);
  }
  else
    arc(C.x, C.y, 2*C.z, 2*C.z, a0, a1);
  if (c.z >= dz*dz) {
    fill(col);
    circle(p.x, p.y, dz);
    circle(q.x, q.y, dz);
  }
}

PVector complement(PVector c, PVector p, PVector q) {
  PVector M = midpoint(p, q);
  PVector MP = perpendicularDirection(p, q);  
  PVector pi = inversion(c, p);
  PVector N = midpoint(p, pi);
  PVector NP = perpendicularDirection(p, pi);
  
  PVector C = lineIntersection(M, N, MP, NP);
  C.z = sqrt(distanceSquared(C, c) - c.z*c.z);
  return C;
}

PVector inversion(PVector c, PVector p) {
  float dx = p.x - c.x;
  float dy = p.y - c.y;
  float l = sqrt(dx*dx + dy*dy);
  if (l == 0)
    return new PVector();

  PVector di = new PVector(dx, dy).normalize();
  float   li = c.z*c.z / l;
  return new PVector(c.x + di.x*li, c.y + di.y*li);
}

PVector midpoint(PVector p, PVector q) {
  return new PVector((p.x+q.x)/2, (p.y+q.y)/2);
}

PVector perpendicularDirection(PVector p, PVector q) {
  float dx = q.x - p.x;
  float dy = q.y - p.y;
  return new PVector(-dy*width, dx*width);
}

PVector lineIntersection(PVector p1, PVector p2, PVector d1, PVector d2) {
  float t = ((p2.y - p1.y) * d1.x - (p2.x - p1.x) * d1.y) / (d2.x * d1.y - d2.y * d1.x);
  return new PVector(p2.x + t*d2.x, p2.y + t*d2.y);
}

float arcAngle(PVector c, PVector p) {
  float x = p.x - c.x;
  float y = p.y - c.y;
  float t = atan2(y, x);
  return (t > 0) ? t : (2*PI + t);
}

float distanceSquared(PVector p, PVector q) {
  float dx = p.x - q.x;
  float dy = p.y - q.y;
  return dx*dx + dy*dy;
}

boolean inCircle(PVector c, PVector p) {
  float dx = p.x - c.x;
  float dy = p.y - c.y;
  float r = c.z;
  return dx*dx + dy*dy < r*r;
}

PVector discClamp(PVector c, PVector p) {
  if (inCircle(c, p))
    return p;
  float t = arcAngle(c, p);
  float co = cos(t);
  float si = sin(t);
  return new PVector(c.x + c.z*co, c.y + c.z*si);
}

int circleVsCircle(PVector c1, PVector c2, PVector[] r) {
  float r0 = c1.z;
  float r1 = c2.z;

  float dx = c2.x - c1.x;
  float dy = c2.y - c1.y;
  float d = sqrt(dx*dx + dy*dy);
  if (d > r0+r1 || d < abs(r0-r1))
    return -1;
  if (d == 0 && r0 == r1)
    return 1;
  
  float a = (r0*r0 - r1*r1 + d*d) / (2*d);
  float h = sqrt(r0*r0 - a*a);
  float x2 = c1.x + (dx*a)/d;
  float y2 = c1.y + (dy*a)/d;
  float rx = -dy*(h/d);
  float ry = dx*(h/d);
  
  r[0].set(x2+rx, y2+ry);
  r[1].set(x2-rx, y2-ry);
  return 0;
}

PVector mouseDiscPosition() {
  PVector m = new PVector(mouseX, mouseY);
  PVector d = disc.copy();
  
  d.z -= dotsz;
  return discClamp(d, m);
}

// uses disc center and radius in the calculation of distance.
float acoshHyperbolicDistance(PVector c, PVector p, PVector q) {
  float pq2 = distanceSquared(p, q);
  float op2 = distanceSquared(c, p);
  float oq2 = distanceSquared(c, q);
  float r2 = c.z*c.z;
  return acosh(1 + (2*pq2*r2)/((r2 - op2)*(r2 - oq2)));
}

// uses the points of intersection of the disc and the circle the hyperbolic line is on
// in the calculation of distance.
float logHyperbolicDistance(PVector c, PVector p, PVector q) {
  if (p.x == q.x && p.y == q.y)
    return 0;

  PVector C = complement(c, p, q);
  PVector[] r = new PVector[]{new PVector(), new PVector()};
  circleVsCircle(c, C, r);

  float aq = PVector.dist(r[0], q);
  float ap = PVector.dist(r[0], p);
  float pb = PVector.dist(p, r[1]);
  float qb = PVector.dist(q, r[1]);
  float nd = aq*pb;
  float dn = ap*qb;
  float d = (nd < dn) ? (dn/nd) : (nd/dn);
  return log(d);
}

float acosh(float x) {
  return log(x + sqrt(x*x - 1.0));
}
