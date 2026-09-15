/*

https://en.wikipedia.org/wiki/B%C3%A9zier_curve
https://www.geeksforgeeks.org/convex-hull-set-1-jarviss-algorithm-or-wrapping/
https://ocw.mit.edu/courses/electrical-engineering-and-computer-science/6-837-computer-graphics-fall-2012/lecture-notes/MIT6_837F12_Lec01.pdf
https://pomax.github.io/bezierinfo/

*/

PVector[] points;
PVector[] bbox;
int fontsz;
int select;
boolean showpts;

void setup() {
  fontsz = 16;
  size(1024, 768);
  strokeWeight(2);
  textSize(fontsz);

  points = new PVector[4];
  reset();
}

void reset() {
  for (int i = 0; i < points.length; i++) {
    float r = 20;
    points[i] = new PVector(width/2 + (i-points.length/2)*4*r, height/2, r);
  }
  bbox = findBbox(points);
  select = -1;
  showpts = true;
}

void draw() {
  background(150, 150, 150);
  drawBezier(points);
  
  if (mousePressed) {
    if (mouseButton == LEFT) {
      if (select != -1) {
        points[select].x = mouseX;
        points[select].y = mouseY;
        bbox = findBbox(points);
      }
    } else if (mouseButton == RIGHT) {
      if (bbox[0].x <= mouseX && mouseX <= bbox[1].x)
        drawBezierParameters(points, mouseX);
    }
  } else if (!mousePressed) {
    select = -1;
  }
}

/*

The tangent vector is the derivative of the bezier curve and the normal
is the vector perpendicular to the tangent and in 2D we can calculate
that by rotating the tangent vector by 90 degrees

*/
void drawBezierTangentAndNormals(float t, float x0, float y0, PVector[] p) {
  float Tx = cubicBezierDerivative(t, p[0].x, p[1].x, p[2].x, p[3].x);
  float Ty = cubicBezierDerivative(t, p[0].y, p[1].y, p[2].y, p[3].y);
  PVector T = new PVector(Tx, Ty).normalize();
  PVector N = new PVector(-T.y, T.x);
  T.mult(50);
  N.mult(50);
  
  stroke(255, 50, 60);
  line(x0-T.x, y0-T.y, x0+T.x, y0+T.y);

  stroke(150, 255, 30);
  line(x0-N.x, y0-N.y, x0+N.x, y0+N.y);
  
  textAlign(LEFT);
  text(String.format("T: (%.3f %.3f)", T.x, T.y), 16, 4*fontsz);
  text(String.format("N: (%.3f %.3f)", N.x, N.y), 16, 5*fontsz);
}

void drawBezierParameters(PVector[] p, float x) {
  // for a given point inside the bezier curve, there can be
  // actually more than one t that can evaluate to the same
  // x coordinate (but different y coordinate), this happens
  // when the curve is on top of itself, we just pick the first
  // t that matches x here
  float t = unbezier(x, p[0].x, p[1].x, p[2].x, p[3].x);
  float px = cubicBezier(t, p[0].x, p[1].x, p[2].x, p[3].x);
  float py = cubicBezier(t, p[0].y, p[1].y, p[2].y, p[3].y);
  if (t < 0)
    return;

  drawBezierTangentAndNormals(t, px, py, p);
  
  PVector[] h = convexHull(p);
  
  textAlign(LEFT);
  text(String.format("t: %.3f", t), 16, fontsz);
  text(String.format("p: (%.3f %.3f)", px, py), 16, 2*fontsz);
  text(String.format("h: %d", h.length), 16, 3*fontsz);
  fill(80, 80, 200);
  ellipse(px, py, 10, 10);
}

void drawBezierPoints(PVector[] p) {
  int steps = 50;
  for (int i = 0; i <= steps; i++) {
    fill(i*5, i*5, i*5);
    float t = i / float(steps);
    float x = bezierPoint(p[0].x, p[1].x, p[2].x, p[3].x, t);
    float y = bezierPoint(p[0].y, p[1].y, p[2].y, p[3].y, t);
    ellipse(x, y, 10, 10);
  }
}

void drawBezier(PVector[] p) {  
  stroke(10, 30, 50);
  fill(50, 60, 100);
  bezier(p[0].x, p[0].y, p[1].x, p[1].y, p[2].x, p[2].y, p[3].x, p[3].y);
  
  drawConvexHull(p);
  if (showpts)
    drawBezierPoints(p);
  
  PVector m = new PVector(mouseX, mouseY, p[0].z);
  for (int i = 0; i < p.length; i++) {
    stroke(0, 0, 0);
    if (inCircle(p[i], m)) {
      fill(200, 40, 80);
      if (select == -1)
        select = i;
    } else
      fill(50, 80, 60 + i*50);
    circle(p[i].x, p[i].y, 2*p[i].z);
    
    fill(230, 248, 250);
    textAlign(CENTER);
    text("P"+i, p[i].x, p[i].y+fontsz/2);
  }
}

void drawConvexHull(PVector[] p) {
  PVector[] h = convexHull(p);
  for (int i = 0; i < h.length; i++) {
    int j = (i+1) % h.length;
    stroke(0, 0, 0);
    line(h[i].x, h[i].y, h[j].x, h[j].y);
  }
}

void keyPressed() {
  if (key == ' ')
    showpts = !showpts;
  else if (key == ENTER)
    reset();
}

/*

The control points create a convex hull for the curve generated.
However, not all points will be used for the convex hull, if one
of the control points is inside the hull where the other control points can bound it,
we can have a situation where the convex hull will be a triangle or a line rather
than a quadrilateral

The convex hull stems from the fact that the basis functions are the bernstein polynomials
which have the property that 0 <= t <= 1 will be B(t) >= 0 and the sum of all of the B(t)
basis functions will be 1.

*/
PVector[] convexHull(PVector[] p) {
  PVector[] h = p.clone();
  int n = h.length;

  int l = 0;
  for (int i = 1; i < n; i++) {
    if (p[i].x < p[l].x || (p[i].x == p[l].x && p[i].y < p[l].y))
      l = i;
  }
  
  int a = l;
  int s = 0;
  do {
    h[s++] = p[a];
    int b = (a+1) % n;
    for (int i = 0; i < n; i++) {
      if (orientation(p[a], p[i], p[b]) == 2)
        b = i;
    }
    a = b;
  } while (a != l);
  
  return h;
}

int orientation(PVector p, PVector q, PVector r) {
  float v = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
  if (v == 0)
    return 0;
  return (v > 0) ? 1 : 2;
}

boolean inCircle(PVector c, PVector p) {
  float dx = p.x - c.x;
  float dy = p.y - c.y;
  float r = c.z;
  return dx*dx + dy*dy < r*r;
}

PVector[] findBbox(PVector[] p) {
  if (p == null)
    return null;
    
  PVector[] b = new PVector[2];
  b[0] = p[0].copy();
  b[1] = p[0].copy();
  for (int i = 1; i < p.length; i++) {
    b[0].x = min(b[0].x, p[i].x);
    b[0].y = min(b[0].y, p[i].y);
    b[1].x = max(b[1].x, p[i].x);
    b[1].y = max(b[1].y, p[i].y);
  }
  return b;
}

/*

Given the control points and parameter t, we can find a generate a point
on a cubic bezier curve evaluating the bernstein polynomials at t, we can write it as
a matrix multiplicaton

T = [1 t t^2 t^3]
B = [[1 0 0 0][-3 3 0 0] [3 -6 3 0] [-1 3 -3 1]]
p = control points
T*B*p
Transpose to make it the right dimension as needed

*/
float cubicBezier(float t, float pa, float pb, float pc, float pd) {
  float a = -(t*t*t) + 3*t*t - 3*t + 1;
  float b = 3*t*t*t - 6*t*t + 3*t;
  float c = -3*t*t*t + 3*t*t;
  float d = t*t*t;
  return a*pa + b*pb + c*pc + d*pd;
}

/*

A derivative of a nth order bezier curve is another bezier curve of order n-1
and the general formula for it is

n * sum(bezier(i, n-1, t) * (p[i+1]-p[i]))

So it is the bezier curve of order n-1 with weights of the delta of the control
points of the original curve

*/
float cubicBezierDerivative(float t, float pa, float pb, float pc, float pd) {
  return 3*quadraticBezier(t, pb-pa, pc-pb, pd-pc);
}

/*

The quadratic bezier can be written as a matrix multiplication
just like a cubic bezier except it gives rise to a quadratic
polynomial rather than a cubic one

T = [1 t t^2]
B = [[1 0 0] [-2 2 0] [1 -2 1]]
p = control points
T*B*p
Transpose to make it the right dimension as needed

*/
float quadraticBezier(float t, float pa, float pb, float pc) {
  float a = t*t - 2*t + 1;
  float b = -2*t*t + 2*t;
  float c = t*t;
  return a*pa + b*pb + c*pc;
}

/*

Given a point on the bezier curve defined by the control point coefficients
we want to find the parameter t that generated it, it involves
solving the bezier curve cubic equation for a real root, that
root will be t.

The polynomial equation is:
(-a + 3b - 3c + d)t^3 + (3a - 6b + 3c)t^2 + (-3a + 3b)t + (a-x) = 0

We get this equation after subtracting x from all of our control point
coefficients to normalize them

*/
float unbezier(float x, float a, float b, float c, float d) {
  float[] r = cubicRealRoots(a-x, b-x, c-x, d-x);
  if (r == null)
    return -1;

  for (int i = 0; i < r.length; i++) {
    if (0 <= r[i] && r[i] <= 1)
      return r[i];
  }
  return -1;
}

boolean approx(float a, float b) {
  return abs(a-b) < 1e-6;
}

float cbrt(float v) {
  if (v < 0)
    return -pow(-v, 1.0/3.0);
  return pow(v, 1.0/3.0);
}

float[] cubicRealRoots(float pa, float pb, float pc, float pd) {
  float a = 3*pa - 6*pb + 3*pc;
  float b = -3*pa + 3*pb;
  float c = pa;
  float d = -pa + 3*pb - 3*pc + pd;
  if (approx(d, 0)) {
    if (approx(a, 0)) {
      if (approx(b, 0))
        return null;
      return new float[]{-c/b};
    }
    float q = sqrt(b*b - 4*a*c);
    return new float[]{(q-b)/(2*a), (-b-q)/(2*a)};
  }
  
  a /= d;
  b /= d;
  c /= d;
  
  float p = (3*b - a*a)/3;
  float p3 = p/3;
  float q = (2*a*a*a - 9*a*b + 27*c)/27;
  float q2 = q/2;
  float d0 = q2*q2 + p3*p3*p3;
  if (d0 < 0) {
    float mp3 = -p/3;
    float mp33 = mp3*mp3*mp3;
    float r = sqrt(mp33);
    float t = -q / (2*r);
    float cosphi = (t < -1) ? -1 : (t > 1) ? 1 : t;
    float phi = acos(cosphi);
    float crtr = cbrt(r);
    float t1 = 2*crtr;
    float r1 = t1 * cos(phi/3) - a/3;
    float r2 = t1 * cos((phi+2*PI)/3) - a/3;
    float r3 = t1 * cos((phi+4*PI)/3) - a/3;
    return new float[]{r1, r2, r3};
  }
  
  if (d0 == 0) {
    float u1 = q2 < 0 ? cbrt(-q2) : -cbrt(q2);
    float r1 = 2*u1 - a/3;
    float r2 = -u1 - a/3;
    return new float[]{r1, r2};
  }
  
  float sd = sqrt(d0);
  float u1 = (float)cbrt(sd - q2);
  float v1 = (float)cbrt(sd + q2);
  return new float[]{u1 - v1 - a/3};
}
