/*

https://en.wikipedia.org/wiki/Inversive_geometry
https://artofproblemsolving.com/wiki/index.php/Circular_Inversion
https://mathworld.wolfram.com/Inversion.html

*/

ArrayList<PVector> points;
ArrayList<PVector> circles;
PVector workingCircle;

void setup() {
  size(1024, 768);
  
  workingCircle = new PVector();
  circles = new ArrayList<PVector>();
  points = new ArrayList<PVector>();
}

void draw() {
  background(200, 200, 200);

  noFill();
  strokeWeight(3);
  stroke(100, 30, 40);
  circle(workingCircle.x, workingCircle.y, 2*workingCircle.z);
  for (int i = 0; i < circles.size(); i++) {
    PVector c = circles.get(i);
    noFill();
    stroke(0, 0, 0);
    // we need to multiply by 2 here because the circle api takes a diameter and not radius
    circle(c.x, c.y, 2*c.z);
    
    for (int j = 0; j < points.size(); j++) {
      PVector p = points.get(j);
      drawInversion(c, p);
    }
    drawInversion(c, new PVector(mouseX, mouseY));
  }
}

/*

Given a point p1 (x, y), we can calculate an inverted point p2 (x', y')
of a circle by this relationship

length(p1)*length(p2) = r^2
where the length() returns the length from the center of the circle (cx, cy)
to that point p, so we can solve for the length of p2 by dividing:
length(p2) = r^2/length(p1)

With the length of p2 and knowing the direction (it is in the same direction as p1)
we can then draw that line

The point at the origin does not have an inverse (an ideal point at infinity)
So as we move torwards the origin, the inverted point goes off to infinity

*/

void drawInversion(PVector circle, PVector point) {
  float dx = point.x - circle.x;
  float dy = point.y - circle.y;
  float len = sqrt(dx*dx + dy*dy);
  if (len == 0)
    return;

  PVector dir = new PVector(dx, dy).normalize();
  PVector invp = new PVector();
  float invplen = (circle.z*circle.z) / len;
  dir.mult(invplen);
  invp.x = circle.x + dir.x;
  invp.y = circle.y + dir.y;
  
  stroke(10, 30, 50);
  line(circle.x, circle.y, point.x, point.y);
  line(point.x, point.y, invp.x, invp.y);
  
  fill(150, 50, 100);
  circle(point.x, point.y, 20);
  fill(80, 100, 150);
  circle(invp.x, invp.y, 20);
}

void mouseDragged() {
  if (mouseButton == RIGHT) {
    float dx = mouseX - workingCircle.x;
    float dy = mouseY - workingCircle.y;
    workingCircle.z = sqrt(dx*dx + dy*dy);
  }
}

void mousePressed() {
  if (mouseButton == RIGHT) {
    workingCircle = new PVector(mouseX, mouseY, 0);
  } else if (mouseButton == LEFT) {
    points.add(new PVector(mouseX, mouseY));
  }
}

void mouseReleased() {
  if (mouseButton == RIGHT) {
    if (workingCircle.z != 0) {
      circles.add(workingCircle);
      workingCircle = new PVector();
    }
  }
}

void keyPressed() {
  if (key == ' ') {
    circles.clear();
    points.clear();
  }
}
