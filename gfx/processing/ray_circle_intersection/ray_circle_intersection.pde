/*

https://raytracing.github.io/books/RayTracingInOneWeekend.html

*/

ArrayList<Disc> discs;
ArrayList<Ray>  rays;
Disc            workDisc;
Ray             workRay;
int             state;
int             prevt;

void setup() {
  size(1200, 800);

  reset(15);
}

void reset(int n) {
  discs = randomCircles(n, false);
  rays = new ArrayList<Ray>();
  state = 0;
  prevt = millis();
}

void draw() {
  background(150);
  strokeWeight(2);

  for (Disc d : discs)
    d.draw();
  for (Ray r : rays)
    r.draw();
  if (workDisc != null)
    workDisc.draw();
  if (workRay != null)
    workRay.draw();
  drawIntersections();
  
  int t = millis();
  float dt = abs(t-prevt) / 1e3;
  update(dt);
  prevt = t;
  mouseMoved();
}

void drawIntersections() {
  float[] t = new float[2];
  for (Ray r : rays) {
    for (Disc d : discs) {
      if (!rayVsCircle(r, d, t))
        continue;

      for (int i = 0; i < 2; i++) {
        if (0 <= t[i] && t[i] <= r.tick) {
          PVector p = r.at(t[i]);
          fill(30);
          circle(p.x, p.y, 10);
        }
      }
    }
  }
}

void update(float dt) {
  for (int i = 0; i < rays.size(); ) {
    Ray r = rays.get(i);
    r.tick += dt;

    PVector p0 = r.at(0);
    PVector p1 = r.at(r.tick);
    if (!(0 <= p1.x && p1.x < width) || !(0 <= p1.y && p1.y < height))
      r.origin.set(p0.x + r.direction.x*dt, p0.y + r.direction.y*dt);

    if (!(0 <= p0.x && p0.x < width) && !(0 <= p0.y && p0.y < height))
      rays.remove(i);
    else
      i++;
  }
}

void keyPressed() {
  if (key == ' ')
    reset(20);
  else if (keyCode == BACKSPACE)
    reset(0);
}

int mouseButtonMask(int button) {
  switch (button) {
  case LEFT:
    return 0x1;
  case CENTER:
    return 0x2;
  case RIGHT:
    return 0x4;
  }
  return 0;
}

void mouseMoved() {
  switch (state&0x7) {
  case 0x1:
    workRay.direction.set(mouseX-workRay.origin.x, mouseY-workRay.origin.y);
    break;

  case 0x4:
    workDisc.radius = dist(mouseX, mouseY, workDisc.center.x, workDisc.center.y);
    break;
  }
}

void mousePressed() {
  if ((state&0x7) != 0)
    return;

  state |= mouseButtonMask(mouseButton);
  switch (mouseButton) {
  case LEFT:
    workRay = new Ray(new PVector(mouseX, mouseY), new PVector(), 1, 1, color(0));
    break;

  case RIGHT:
    workDisc = new Disc(new PVector(mouseX, mouseY), 0, randomColor());
    break;
  }
}

void mouseReleased() {
  switch (state&0x7) {
  case 0x1:
    Ray r = new Ray(workRay.origin, workRay.direction, 0, 0, randomColor());
    rays.add(r);
    workRay = null;
    break;

  case 0x4:
    discs.add(workDisc);
    workDisc = null;
    break;
  }

  state &= ~mouseButtonMask(mouseButton);
}

color randomColor() {
  return color(random(255), random(255), random(255));
}

PVector randomPoint() {
  return new PVector(random(width), random(height));
}

ArrayList<Disc> randomCircles(int len, boolean overlap) {
  ArrayList<Disc> l = new ArrayList<Disc>();
  PVector[] r = new PVector[]{new PVector(), new PVector()};

  int tries = 0;
  for (int i = 0; i < len; ) {
    Disc d = new Disc(randomPoint(), random(30, 100), randomColor());

    boolean collided = false;
    if (!overlap) {
      for (Disc p : l) {
        if (circleVsCircle(p.center, d.center, p.radius, d.radius, r) >= 0 || pointInCircle(p.center, p.radius, d.center)) {
          collided = true;
          tries++;
          break;
        }
      }
    }

    if (overlap || !collided) {
      l.add(d);
      i++;
      tries = 0;
    } else if (tries >= len*len)
      break;
  }

  return l;
}

boolean pointInCircle(PVector c, float r, PVector p) {
  float dx = p.x - c.x;
  float dy = p.y - c.y;
  return dx*dx + dy*dy < r*r;
}

int circleVsCircle(PVector c1, PVector c2, float r1, float r2, PVector[] r) {
  float dx = c2.x - c1.x;
  float dy = c2.y - c1.y;
  float d = sqrt(dx*dx + dy*dy);
  if (d > r1+r2 || d < abs(r1-r2))
    return -1;
  if (d == 0 && r1 == r2)
    return 1;
  
  float a = (r1*r1 - r2*r2 + d*d) / (2*d);
  float h = sqrt(r1*r1 - a*a);
  float x2 = c1.x + (dx*a)/d;
  float y2 = c1.y + (dy*a)/d;
  float rx = -dy*(h/d);
  float ry = dx*(h/d);
  
  r[0].set(x2+rx, y2+ry);
  r[1].set(x2-rx, y2-ry);
  return 0;
}

/*

The equation of a sphere (also applies to a circle) is

(x - Cx)^2 + (y - Cy)^2 + (z - Cz)^2 = r^2

We can rewrite this using dot product of a test point and the sphere center point and make it an implicit equation 
(P - C).(P - C) - r^2 = 0

The parametric equation of a line is
P(t) = A + tB

Substitute in the equation above and we get
(A + tB - C).(A + tB - C) - r^2 = 0

We want the equation to be in the form
ax^2 + bx + c = 0
to be able to use the quadratic formula.

We can do this by rewriting the above formula into
(Xt + Y)^2
X=t*B
Y=A-C

Expand the above out and resubstitute the original variables back in to get
t^2*B.B + t*2*b.(A - C) + (A - C).(A - C) - r^2 = 0

*/

boolean rayVsCircle(Ray r, Disc c, float[] t) {
  PVector oc = PVector.sub(r.origin, c.center);
  float pa = PVector.dot(r.direction, r.direction);
  float hb = PVector.dot(oc, r.direction);
  float pc = PVector.dot(oc, oc) - c.radius*c.radius;
  float d = hb*hb - pa*pc;
  if (d < 0)
    return false;
  float sd = sqrt(d);

  t[0] = (-hb - sd) / pa;
  t[1] = (-hb + sd) / pa;
  return true;
}

void dashline(float x1, float y1, float x2, float y2, int n) {
  PVector p = new PVector(x1, y1);
  PVector q = new PVector();
  for (int i = 1; i < 2*n; i++) {
    float t = i*0.5/n;

    q.set(x1 + (x2-x1)*t, y1 + (y2-y1)*t);
    if ((i&1) != 0)
      line(round(p.x), round(p.y), round(q.x), round(q.y));
    p.set(q);
  }
}

class Disc {
  PVector center;
  float radius;
  color chroma;
  
  Disc(PVector center, float radius, color chroma) {
    this.center = center;
    this.radius = radius;
    this.chroma = chroma;
  }
  
  void draw() {
    push();
    fill(chroma);
    circle(center.x, center.y, 2*radius);
    pop();
  }
}

class Ray {
  PVector origin;
  PVector direction;
  float tick;
  int mode;
  color chroma;

  Ray(PVector origin, PVector direction, float tick, int mode, color chroma) {
    this.origin = origin;
    this.direction = direction;
    this.tick = tick;
    this.mode = mode;
    this.chroma = chroma;
  }
  
  PVector at(float t) {
    return new PVector(origin.x + direction.x*t, origin.y + direction.y*t);
  }

  void draw() {
    push();
    stroke(chroma);
    PVector p0 = at(0);
    PVector p1 = at(tick);
    if (mode == 1)
      dashline(p0.x, p0.y, p1.x, p1.y, 20);
    else
      line(p0.x, p0.y, p1.x, p1.y);
    pop();
  }
}
