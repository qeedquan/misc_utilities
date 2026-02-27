PVector eye;
PVector at;
PVector proj;
int state;

void setup() {
  size(1024, 768);
  strokeWeight(2);
  eye = new PVector(width/2, height-20, 20);
  at = new PVector(eye.x, eye.y-200, eye.z);
  proj = new PVector(0, at.y-300, eye.z);
  state = 0x0;
}

void hline(int state, float y, float r, float g, float b) {
  if (state != 0)
    stroke(40, 100, 150);
  else
    stroke(r, g, b);
  line(0, y, width, y);
}

void draw() {
  background(150, 160, 180);
  
  line(eye.x, eye.y, at.x, at.y);  
  
  pushStyle();
  hline(state&0x1, eye.y, 0, 0, 0);
  fill(255, 255, 255);
  circle(eye.x, eye.y, eye.z);
  popStyle();
  
  pushStyle();
  hline(state&0x2, at.y, 130, 70, 200);
  fill(200, 50, 100);
  circle(at.x, at.y, at.z);
  popStyle();
  
  pushStyle();
  hline(state&0x4, proj.y, 200, 250, 200);
  popStyle();
  
  pushStyle();
  PVector ip = calcIntersect(eye, at, proj);
  line(at.x, at.y, ip.x, ip.y);
  fill(150, 35, 200);
  circle(ip.x, ip.y, ip.z);
  popStyle();
  
  state &= ~0x7;
  if (mousePressed)
    mousePressed();  
  applyClamping();
}

void mousePressed() {
  if (mouseButton == LEFT) {
    eye.x = mouseX;
    state |= 0x1;
  } else if (mouseButton == RIGHT) {
    at.x = mouseX;
    state |= 0x2;
  } else if (mouseButton == CENTER) {
    if (keyPressed && key == CODED && keyCode == SHIFT) {
      proj.y = mouseY;
      state |= 0x4;
    } else {
      at.y = mouseY;
      state |= 0x2;
    }
  }
}

void applyClamping() {
  eye.x = clamp(eye.x, eye.z, width-eye.z);
  proj.y = clamp(proj.y, 0, eye.y);
  at.x = clamp(at.x, at.z, width-at.z);
  at.y = clamp(at.y, proj.y, height-at.z);
}

float clamp(float x, float a, float b) {
  return min(max(x, a), b);
}

/*

p = at
t = normalize(at-eye)
p + s*t = plane

p.x + s*t.x = plane.x
p.y + s*t.y = plane.y

We have where plane.y is, we want to find x
s = (plane.y-p.y)/t.y

*/
PVector calcIntersect(PVector eye, PVector at, PVector plane) {
  PVector dir = PVector.sub(at, eye).normalize();
  float s = (plane.y-at.y)/dir.y;
  float x = at.x + s*dir.x;
  
  return new PVector(x, plane.y, eye.z);
}
