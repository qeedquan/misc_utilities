PVector point;
PVector[] direction;

PMatrix2D rotation;
boolean rotating;

void setup() {
  size(1024, 768);
  
  point = new PVector(width/2, height/2);
  direction = new PVector[]{
    new PVector(150, 0),
    new PVector(0, -150),
  };
  rotation = new PMatrix2D();
  rotation.rotate(radians(1));
  rotating = false;
}

void draw() {
  background(200, 200, 200);
  drawParallelogram(point, direction[0], direction[1], 15);
  if (rotating)
    rotation.mult(direction[0], direction[0]);
}

/*

A parallelogram can be written in parametric form as
p = p0 + dirx*t + diry*s where [t, s] are parameterization from [0, 1]

p0 is the origin point
dirx is the x direction vector
diry is the y direction vector

*/

void drawParallelogram(PVector origin, PVector dirx, PVector diry, float tr) {
  strokeWeight(2);
  stroke(41, 122, 245);

  float dy = 1.0/diry.mag();
  PVector u0 = PVector.mult(dirx, 0);
  PVector u1 = PVector.mult(dirx, 1);
  PVector q0 = PVector.add(origin, u0);
  PVector q1 = PVector.add(origin, u1);
    
  for (float y = 0; y <= 1; y += dy) {
    PVector v = PVector.mult(diry, y);
    PVector p0 = PVector.add(q0, v);
    PVector p1 = PVector.add(q1, v);
    line(p0.x, p0.y, p1.x, p1.y);
  }
  
  strokeWeight(5);
  fill(0, 0, 0);
  stroke(200, 60, 8);
  line(origin.x, origin.y, origin.x+dirx.x, origin.y+dirx.y);
  circle(origin.x+dirx.x, origin.y+dirx.y, tr);
  stroke(50, 30, 200);
  line(origin.x, origin.y, origin.x+diry.x, origin.y+diry.y);
  circle(origin.x+diry.x, origin.y+diry.y, tr);
}

void keyPressed() {
  if (key == ' ')
    rotating = !rotating;
}

void mousePressed() {
  PVector dir = new PVector(mouseX-point.x, mouseY-point.y);
  if (mouseButton == LEFT)
    direction[0] = dir;
  else if (mouseButton == RIGHT)
    direction[1] = dir;
  else if (mouseButton == CENTER)
    point = new PVector(mouseX, mouseY);
}
