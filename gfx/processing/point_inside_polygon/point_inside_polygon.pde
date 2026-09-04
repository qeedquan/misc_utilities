/*

https://doc.qt.io/qt-5/qpaintengine.html#PolygonDrawMode-enum
https://en.wikipedia.org/wiki/Even%E2%80%93odd_rule
https://en.wikipedia.org/wiki/Nonzero-rule
https://en.wikipedia.org/wiki/Point_in_polygon
http://geomalgorithms.com/a03-_inclusion.html
https://jeffe.cs.illinois.edu/teaching/comptop/2009/notes/jordan-polygon-theorem.pdf

*/

Polygon poly;
int fontsz;

void setup() {
  size(1200, 800);
  poly = new Polygon();
  fontsz = 16;
  textSize(fontsz);
}

void draw() {
  background(200);
  poly.draw();
  
  text(String.format("Fill Mode: %s", poly.fillMode()), 16, 32 + 0*fontsz);
}

void keyPressed() {
  if (key == ' ')
    poly.randomize(50);
  else if (keyCode == BACKSPACE)
    poly.clear();
  else if (keyCode == LEFT)
    poly.mode--;
  else if (keyCode == RIGHT)
    poly.mode++;
  poly.mode = wrap(poly.mode, 0, 2);
}

void mousePressed() {
  if (mouseButton == LEFT)
    poly.add(new PVector(mouseX, mouseY));
}

int wrap(int x, int a, int b) {
  if (x < a)
    x = b;
  else if (x > b)
    x = a;
  return x;
}

class Rect {
  PVector min, max;
  
  Rect() {
    min = new PVector();
    max = new PVector();
  }
  
  Rect(PVector min, PVector max) {
    this.min = min.copy();
    this.max = max.copy();
  }
  
  float width() {
    return max.x - min.x;
  }
  
  float height() {
    return max.y - min.y;
  }
}

/*

The two common ways of figuring out whether or not a point is inside a polygon
is using the boundary crossing method or angle summation method.

The boundary crossing method calculates the crossing number and
the angle summation method calculates the winding number.

They are also known as even-odd rule and non-zero winding rule respectively.
Many drawing engines implement both of these rules and allow the user to pick for filling polygons, though
they have specialized implementation that do not do a point by point test to fill in the polygon.
We implement both of these rules and also the polyline rule (where only the outline is drawn).

Both of the methods give the same answer for a simple/convex polygon (no holes or intersections).
For non-simple polygons the boundary crossing method does not consider a point inside the
intersection area to be inside the polygon while the angle summation method does.

The crossing number is calculated as follows:
1) Shoot a horizontal ray of where the test point is against a polygon.
2) For every time the ray crosses a polygon boundary, the crossing number is incremented.
3) If the crossing number at the end is even, then it is outside. Otherwise it is considered to be inside.

There are special cases that have to be handled which is covered by the edge crossing rules.

Edge Crossing Rules:
1) A upward edge includes its starting endpoint, and excludes its final endpoint.
2) A downward edge excludes its starting endpoint, and includes its final endpoint.
3) Horizontal edges are excluded
4) The edge-ray intersection point must be strictly right of the point P.

The winding number counts how many times a polygon winds around a point.
A point is considered outside when the winding number is 0.
wn(P, C): 1/(2pi) * sum(0, n-1)[angle_i]

More generally, we can define the winding number for any closed continuous curve around a point P in the 2D plane.
Let the curve be defined as C(x(t), y(t)), for 0 <= t <= 1 with C(0) = C(1).
Let P be a point not in C, then the vector c(P, t) = C(t)-P.
Define another vector w(P, t) which is the normalized vector c(P, t).
This gives us a continuous mapping to a unit circle, hence we can represents w(P, t) as (cos(a(t)), sin(a(t)).
The winding number is equal to the integer number of times w(P) wraps the curve C around the circle S1.

This formula is clearly not very efficient since it uses a computationally expensive arccos() trig function.
But, a simple observation lets us replace this formula by a more efficient one.
Pick any point Q on S1. Then, as the curve W(P) wraps around S1, it passes Q a certain number of times.
If we count (+1) when it passes Q counterclockwise, and (–1) when it passes clockwise, then the accumulated sum is exactly the total number of times
that W(P) wraps around S1, and is equal to the winding number.

Further, if we take an infinite ray R starting at P and extending in the direction of the vector Q,
then intersections where R crosses the curve C correspond to the points where W(P) passes Q.
To do the math, we have to distinguish between positive and negative crossings where C crosses R from right-to-left or left-to-right.
This can be determined by the sign of the dot product between a normal vector to C and the direction vector q = Q [Foley et al, 1996, p-965],
and when the curve C is a polygon, one just needs to make this determination once for each edge.
For a horizontal ray R from P, testing whether an edge's endpoints are above and below the ray suffices.
If the edge crosses the positive ray from below to above, the crossing is positive (+1);
but if it crosses from above to below, the crossing is negative (–1). One then simply adds all crossing values to get wn(P, C).

*/

class Polygon {
  ArrayList<PVector> vertices;
  Rect bounds;
  int mode;
  
  Polygon() {
    randomize(50);
    mode = 1;
  }
  
  void randomize(int n) {
    vertices = gen(n);
    bounds = boundary();
  }
  
  void clear() {
    vertices.clear();
    bounds = new Rect();
  }
  
  void add(PVector p) {
    vertices.add(p);
    bounds = boundary();
  }
  
  ArrayList<PVector> gen(int n) {
    ArrayList<PVector> v = new ArrayList<PVector>();
    int l = round(max(3, random(n)));
    for (int i = 0; i <= l; i++)
      v.add(new PVector(random(width), random(height)));
    return v;
  }
  
  Rect boundary() {
    Rect r = new Rect(new PVector(1e6, 1e6), new PVector(-1e6, -1e6));
    
    int n = vertices.size();
    for (int i = 0; i < n; i++) {
      PVector v = vertices.get(i);
      r.min.x = min(r.min.x, v.x);
      r.min.y = min(r.min.y, v.y);
      r.max.x = max(r.max.x, v.x);
      r.max.y = max(r.max.y, v.y);
    }

    return r;
  }
  
  void draw() {
    if (mode != 0)
      drawFilled();
    drawOutline();
    drawBoundary();
  }
  
  void drawOutline() {
    stroke(0);
    int n = vertices.size();
    for (int i = 0; i < n; i++) {
      PVector v = vertices.get(i);
      PVector u = vertices.get((i+1) % n);
      line(v.x, v.y, u.x, u.y);
    }
    for (int i = 0; i < n; i++) {
      PVector v = vertices.get(i);
      fill(255);
      circle(v.x, v.y, 10);
    }
  }
  
  void drawBoundary() {
    noFill();
    strokeWeight(2);
    stroke(80, 190, 200);
    rect(bounds.min.x, bounds.min.y, bounds.width(), bounds.height());
  }
  
  void drawFilled() {
    loadPixels();
    Rect r = bounds;
    PVector p = new PVector();
    for (p.y = r.min.y; p.y <= r.max.y; p.y++) {
      for (p.x = r.min.x; p.x <= r.max.x; p.x++) {
        boolean in = false;
        if (mode == 1)
          in = (crossingNumber(p)&1) != 0;
        else if (mode == 2)
          in = windingNumber(p) != 0;

        if (in)
          pixels[round(p.y)*width + round(p.x)] = color(200, 150, 180);
      }
    }
    updatePixels();
  }
  
  float orientation(PVector p0, PVector p1, PVector p2) {
    return (p1.x - p0.x)*(p2.y - p0.y) - (p2.x - p0.x)*(p1.y - p0.y);
  }
  
  int windingNumber(PVector p) {
    int wn = 0;
    int n = vertices.size();
    for (int i = 0; i < n; i++) {
      PVector v = vertices.get(i);
      PVector u = vertices.get((i+1)%n);
      if (v.y <= p.y && u.y > p.y && orientation(v, u, p) > 0)
        wn++;
      else if (v.y > p.y && u.y <= p.y && orientation(v, u, p) < 0)
        wn--;
    }
    return wn;
  }
  
  int crossingNumber(PVector p) {
    int cn = 0;
    int n = vertices.size();
    for (int i = 0; i < n; i++) {
      PVector v = vertices.get(i);
      PVector u = vertices.get((i+1)%n);
      if (((v.y <= p.y) && (u.y > p.y)) || ((v.y > p.y) && (u.y <= p.y))) {
        float vt = (p.y - v.y) / (u.y - v.y);
        if (p.x <  v.x + vt * (u.x - v.x))
          cn++;
      }
    }
    return cn;
  }
  
  String fillMode() {
    if (mode == 0)
      return "None";
    if (mode == 1)
      return "Crossing";
    return "Winding";
  }
}
