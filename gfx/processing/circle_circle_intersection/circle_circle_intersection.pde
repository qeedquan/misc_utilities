/*

http://paulbourke.net/geometry/circlesphere/
https://mathworld.wolfram.com/Circle-CircleIntersection.html
http://jwilson.coe.uga.edu/EMAT6680Su09/Park/As7dspark/As7dspark.html

*/

ArrayList<PVector> circles;
ArrayList<PVector> palettes;
int select;

void setup() {
  size(1024, 768);
  circles = new ArrayList<PVector>();
  circles.add(new PVector(width/2, height/2, 100));
  circles.add(new PVector(width/2+100, height/2, 100));
  circles.add(new PVector(width/2+50, height/2+100, 100));
  palettes = new ArrayList<PVector>();
  for (int i = 0; i < circles.size(); i++)
    palettes.add(randColor());
  select = -1;
}

void draw() {
  background(210);
  
  strokeWeight(2);
  drawCircles(circles, palettes);

  if (!mousePressed)
    select = -1;
  else if (mouseButton == RIGHT) {
    PVector c = circles.get(circles.size()-1);
    c.z = dist(mouseX, mouseY, c.x, c.y);
  } else if (select != -1) {
    PVector c = circles.get(select);
    c.set(mouseX, mouseY, c.z);
    if (keyPressed) {
      if (key == 'd') {
        circles.remove(select);
        palettes.remove(select);
        select = -1;
      }
    }
  }
}

void mousePressed() {
  PVector m = new PVector(mouseX, mouseY);
  if (mouseButton == LEFT) {
    select = -1;
    for (int i = 0; i < circles.size(); i++) {
      if (pointInCircle(circles.get(i), m)) {
        select = i;
        break;
      }
    }
  } else if (mouseButton == RIGHT) {
    circles.add(new PVector(mouseX, mouseY, 10));
    palettes.add(randColor());
  }
}

void drawCircles(ArrayList<PVector> cir, ArrayList<PVector> pal) {
  ArrayList<PVector> rl = new ArrayList<PVector>();

  // area within the circle intersection
  fill(20, 20, 20);
  PVector[] r = {new PVector(), new PVector()};
  int n = cir.size();
  for (int i = 0; i < n; i++) {
    PVector a = cir.get(i);
    for (int j = i+1; j < n; j++) {
      PVector b = cir.get(j);
      PVector c = averageColor(pal.get(i), pal.get(j));
      int d = circleVsCircle(a, b, r);
      if (d == 0) {
        fill(c.x, c.y, c.z);
        stroke(c.x, c.y, c.z);
        drawCircleIntersection(a, b, r);
        rl.add(r[0].copy());
        rl.add(r[1].copy());
      } else if (d == 1) {
        fill(c.x, c.y, c.z);
        circle(a.x, a.y, a.z*2);
      }
    }
  }
  
  // point of intersection
  stroke(100);
  fill(100);
  for (PVector p : rl)
    circle(p.x, p.y, 10);

  // circle boundary
  noFill();
  for (int i = 0; i < circles.size(); i++) {
    PVector c = circles.get(i);

    if (i == select)
      stroke(0, 0, 255);
    else
      stroke(0);
    circle(c.x, c.y, c.z*2);
  }
}

/*

Simplest way to draw the area of intersection, loop through the rectangle that
encloses the two circle and do a point intersection test against both circles,
if both of them are satisfied then the point is in the area of intersection

*/
void drawCircleIntersection(PVector c1, PVector c2, PVector[] r) {
  int x1, y1;
  int x2, y2;
  
  x1 = round(min(c1.x-c1.z, c2.x-c2.z));
  y1 = round(min(c1.y-c1.z, c2.y-c2.z));
  x2 = round(max(c1.x+c1.z, c2.x+c2.z));
  y2 = round(max(c1.y+c1.z, c2.y+c2.z));

  PVector p = new PVector();
  for (int y = y1; y <= y2; y++) {
    int px1 = -1;
    int px2 = -1;
    for (int x = x1; x <= x2; x++) {
      p.set(x, y);
      if (pointInCircle(c1, p) && pointInCircle(c2, p)) {
        px1 = (px1 < 0) ? x : min(px1, x);
        px2 = (px2 < 0) ? x : max(px2, x);
      }
    }
    if (px1 >= 0)
      line(px1, y, px2, y);
  }
}

/*

A circle can intersect another circle in the following ways:
no intersection           (completely inside or outside, another way to think about this is that they intersected in the imaginary plane but not the real plane)
one point                 (when two circles are tangent to each other, they share a common tangent line and tangent point at the intersection point)
two points                (the normal case if we are expecting an intersection)
infinite number of points (intersected the same circle)

*/

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

boolean pointInCircle(PVector c, PVector p) {
  float dx = p.x - c.x;
  float dy = p.y - c.y;
  float r = c.z;
  return dx*dx + dy*dy < r*r;
}

PVector averageColor(PVector c1, PVector c2) {
  return new PVector((c1.x+c2.x)/2, (c1.y+c2.y)/2, (c1.z+c2.z)/2);
}

PVector randColor() {
  return new PVector(random(255), random(255), random(255));
}
