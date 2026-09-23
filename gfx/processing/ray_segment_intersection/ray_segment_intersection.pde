// ported from https://ncase.me/sight-and-light/

int demo;

void setup() {
  size(640, 360);
  noCursor();
  setDemo(3);
}

void setDemo(int value) {
  demo = value;
  println("Demo: " + demo);
}

void keyPressed() {
  if ('0' <= keyCode && keyCode <= '3') {
    setDemo(keyCode - '0');
  }
}

void draw() {
  background(200);
  drawSegments();
  switch (demo) {
  case 0:
    draw0();
    break;
  case 1:
    draw1();
    break;
  case 2:
    draw2();
    break;
  case 3:
    draw3();
    break;
  }
}

void draw0() {    
  fill(0xdd, 0x38, 0x38);
  circle(mouseX, mouseY, 8);
}

void draw1() {
  PVector[] ray = {new PVector(width/2, height/2), new PVector(mouseX, mouseY)};
  PVector point = closest(ray);
  if (point != null) {
    stroke(0xdd, 0x38, 0x38);
    fill(0xdd, 0x38, 0x38);

    line(ray[0].x, ray[0].y, point.x, point.y);
    circle(point.x, point.y, 8);
  }
}

void draw2() {
  stroke(0xdd, 0x38, 0x38);
  fill(0xdd, 0x38, 0x38);
  
  final float numRays = 50;
  for (float t = 0; t < 2*PI; t += 2*PI/numRays) {
    float dx = cos(t);
    float dy = sin(t);
    
    PVector[] ray = {new PVector(mouseX, mouseY), new PVector(mouseX+dx, mouseY+dy)};
    PVector point = closest(ray);
    if (point == null)
      continue;
    
    line(mouseX, mouseY, point.x, point.y);
    circle(point.x, point.y, 8);
  }
}

void draw3() {
  ArrayList<PVector> points = new ArrayList<PVector>();
  final int numRays = 50;
  for (float t = 0; t < 2*PI; t += 2*PI/numRays) {
    float dx = cos(t);
    float dy = sin(t);
    
    PVector[] ray = {new PVector(mouseX, mouseY), new PVector(mouseX+dx, mouseY+dy)};
    PVector point = closest(ray);
    if (point == null)
      continue;

    points.add(point);
  }
  
  beginShape();
  fill(0xdd, 0x38, 0x38);
  for (int i = 0; i < points.size(); i++) {
    PVector point = points.get(i);
    vertex(point.x, point.y);
  }
  endShape();

  stroke(0xf5, 0x55, 0x55);
  for (int i = 0; i < points.size(); i++) {
    PVector point = points.get(i);
    line(mouseX, mouseY, point.x, point.y);
  }
}

void drawSegments() {
  stroke(9);
  for (int i = 0; i < segments.length; i++) {
    PVector[] seg = segments[i];
    line(seg[0].x, seg[0].y, seg[1].x, seg[1].y);
  }
}

PVector closest(PVector[] ray) {
  PVector point = null;
  for (int i = 0; i < segments.length; i++) {
    PVector hit = intersection(ray, segments[i]);
    if (hit == null)
      continue;

    if (point == null || hit.z < point.z)
      point = hit;
  }
  return point;
}

PVector intersection(PVector[] ray, PVector[] segment) {
  // RAY in parametric: Point + Direction*T1
  float r_px = ray[0].x;
  float r_py = ray[0].y;
  float r_dx = ray[1].x - ray[0].x;
  float r_dy = ray[1].y - ray[0].y;

  // SEGMENT in parametric: Point + Direction*T2
  float s_px = segment[0].x;
  float s_py = segment[0].y;
  float s_dx = segment[1].x - segment[0].x;
  float s_dy = segment[1].y - segment[0].y;

  // Are they parallel? If so, no intersect
  float r_mag = sqrt(r_dx*r_dx + r_dy*r_dy);
  float s_mag = sqrt(s_dx*s_dx + s_dy*s_dy);
  // Direction are the same
  if (r_dx/r_mag == s_dx/s_mag && r_dy/r_mag == s_dy/s_mag)
    return null;

  // SOLVE FOR T1 & T2
  // r_px+r_dx*T1 = s_px+s_dx*T2 && r_py+r_dy*T1 = s_py+s_dy*T2
  // ==> T1 = (s_px+s_dx*T2-r_px)/r_dx = (s_py+s_dy*T2-r_py)/r_dy
  // ==> s_px*r_dy + s_dx*T2*r_dy - r_px*r_dy = s_py*r_dx + s_dy*T2*r_dx - r_py*r_dx
  // ==> T2 = (r_dx*(s_py-r_py) + r_dy*(r_px-s_px))/(s_dx*r_dy - s_dy*r_dx)
  float T2 = (r_dx*(s_py - r_py) + r_dy*(r_px - s_px)) / (s_dx*r_dy - s_dy*r_dx);
  float T1 = (s_px+s_dx * T2-r_px) / r_dx;

  // Must be within parametic whatevers for RAY/SEGMENT
  if (T1 < 0)
    return null;
  if (T2 < 0 || T2 > 1)
    return null;

  // Return the POINT OF INTERSECTION
  return new PVector(r_px + r_dx*T1, r_py + r_dy*T1, T1);
}

PVector[][] segments = {
  // border
  {new PVector(0, 0), new PVector(640, 0)},
  {new PVector(640, 0), new PVector(640, 360)},
  {new PVector(640, 360), new PVector(0, 360)},
  {new PVector(0, 360), new PVector(0, 0)},
  
  // polygon #1
  {new PVector(100, 150), new PVector(120, 50)},
  {new PVector(120, 50), new PVector(200, 80)},
  {new PVector(200, 80), new PVector(140, 210)},
  {new PVector(140, 210), new PVector(100, 150)},
  
  // polygon #2
  {new PVector(100, 200), new PVector(120, 250)},
  {new PVector(120, 250), new PVector(60, 300)},
  {new PVector(60, 300), new PVector(100, 200)},
  
  // polygon #3
  {new PVector(200, 260), new PVector(220, 150)},
  {new PVector(220, 150), new PVector(300, 200)},
  {new PVector(300, 200), new PVector(350, 320)},
  {new PVector(350, 320), new PVector(200, 260)},
  
  // polygon #4
  {new PVector(340, 60), new PVector(360, 40)},
  {new PVector(360, 40), new PVector(370, 70)},
  {new PVector(370, 70), new PVector(340, 60)},
  
  // polygon #5
  {new PVector(450, 190), new PVector(560, 170)},
  {new PVector(560, 170), new PVector(540, 270)},
  {new PVector(540, 270), new PVector(430, 290)},
  {new PVector(430, 290), new PVector(450, 190)},
  
  // polygon #6
  {new PVector(400, 95), new PVector(580, 50)},
  {new PVector(580, 50), new PVector(480, 150)},
  {new PVector(480, 150), new PVector(400, 95)},  
};
