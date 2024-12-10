// http://paulbourke.net/geometry/fowler/

PVector p1 = new PVector();
PVector p2 = new PVector();
PVector pc = new PVector();

void setup() {
  size(1024, 768);
  textSize(16);
  
  pc = new PVector(width/2, height/2);
  p1 = new PVector(pc.x + 100, pc.y);
  p2 = new PVector(pc.x, pc.y + 100);
}

void draw() {
  background(100);
  
  strokeWeight(5);
  stroke(100, 0, 0);
  line(pc.x, pc.y, p1.x, p1.y);
  
  stroke(0, 0, 100);
  line(pc.x, pc.y, p2.x, p2.y);
  
  fill(20, 80, 40);
  circle(p1.x, p1.y, 20);
  circle(p2.x, p2.y, 20);
  
  fill(255, 255, 255);
  PVector v1 = PVector.sub(p1, pc);
  PVector v2 = PVector.sub(p2, pc);
  float ang = PVector.angleBetween(v1, v2);
  float fang = fowlerAngle(p2.x-p1.x, p2.y-p1.y);
  text("Regular Angle: " + degrees(ang), 16, 16);
  text("Fowler Angle: " + fang, 16, 32);
  
  mouseEvent();
}

void mouseEvent() {
  if (!mousePressed)
    return;
    
  if (mouseButton == LEFT) {
    p1 = new PVector(mouseX, mouseY);
  } else if (mouseButton == RIGHT) {
    p2 = new PVector(mouseX, mouseY);
  }
}


/*
   This function is due to Rob Fowler.  Given dy and dx between 2 points
   A and B, we calculate a number in [0.0, 8.0) which is a monotonic
   function of the direction from A to B. 

   (0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0) correspond to
   (  0,  45,  90, 135, 180, 225, 270, 315, 360) degrees, measured
   counter-clockwise from the positive x axis.
*/
float fowlerAngle(float dx, float dy) {
  float adx, ady;
  int code;
  
  adx = abs(dx);
  ady = abs(dy);
  
  code = (adx < ady) ? 1 : 0;
  if (dx < 0)
    code |= 2;
  if (dy < 0)
    code |= 4;
  
  switch (code) {
  case 0:
    return (dx == 0) ? 0 : ady/adx;
  case 1:
    return (2.0 - (adx/ady));
  case 3:
    return (2.0 + (adx/ady));
  case 2:
    return (4.0 - (ady/adx));
  case 6:
    return (4.0 + (ady/adx));
  case 7:
    return (6.0 - (adx/ady));
  case 5:
    return (6.0 + (adx/ady));
  case 4:
    return (8.0 - (ady/adx));
  }
  return 0.0;
}
