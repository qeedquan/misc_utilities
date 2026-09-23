/*

http://www2.clarku.edu/faculty/djoyce/trig/tangents.html
https://www.math-only-math.com/slope-of-a-line.html

*/

float angle;
int fontsz;
boolean animating;

void setup() {
  size(1024, 768);
  fontsz = 16;
  textSize(fontsz);
  angle = radians(45);
  animating = false;
}

void draw() {
  background(200);
  push();
  strokeWeight(2);
  line(width/2, 0, width/2, height);
  line(0, height/2, width, height/2);
  
  stroke(100, 180, 30);
  for (int i = 0; i < 360; i += 30)
    drawSlope(width/2, height/2, angle+radians(i), 200, false);
  
  stroke(255, 100, 25);
  drawSlope(width/2, height/2, angle, 200, true);
  
  pop();
  
  if (mousePressed)
    drawCircular(mouseX, mouseY);
  
  if (animating)
    angle += radians(1);
  angle %= 2*PI;
}

/*

Given a point on the plane and a circle center
(x, y)   point
(cx, cy) circle center

We can find the angle between them with
a = atan2(y-cy, x-cx)

We can use that angle to find the point on the circular arc that is closest to the point.
The point on the circle arc will be on the same line made by connecting (x, y) and (cx, cy).

*/

void drawCircular(float x, float y) {
  float x0 = width/2;
  float y0 = height/2;
  float t = atan2(y-y0, x-x0);
  float r = width/4;
  float c = r*cos(t);
  float s = r*sin(t);
  
  push();
  noFill();
  circle(x0, y0, 2*r);
  fill(120, 30, 150);
  circle(x0+c, y0+s, 10);
  fill(50, 100, 180);
  circle(x, y, 10);
  
  fill(255);
  text(String.format("Circular Angle: %.3f", degrees(t)), 16, 32 + 2*fontsz);

  pop();
}

/*

Given an angle with the x-axis and the line,
we can use the tangent of the angle as a slope

m = tan(theta)
y = mx + b

If the y coordinate system is top-bottom (standard graphics convention) and we
want to convert it to bottom-top (standard math convention), negate the sign 
m = tan(-theta)

This formulation of a line preserve directions but the length changes along a direction

*/

void drawSlope(float x, float y, float angle, float dx, boolean showText) {
  float m = tan(-angle);
  float b = y - m*x;
  
  float x1 = x - dx;
  float y1 = m*x1 + b;

  float x2 = x + dx;
  float y2 = m*x2 + b;
  line(x1, y1, x2, y2);
  
  if (showText) {
    fill(255);
    text(String.format("Angle:  %.3f", degrees(angle)), 16, 32);
    text(String.format("Length: %.3f", lineLength(x1, y1, x2, y2)), 16, 32 + fontsz);
  }
}

float lineLength(float x1, float y1, float x2, float y2) {
  float dx = x2 - x1;
  float dy = y2 - y1;
  return sqrt(dx*dx + dy*dy);
}

void keyPressed() {
  if (keyCode == ' ')
    animating = !animating;
  else if (keyCode == LEFT)
    angle -= radians(1);
  else if (keyCode == RIGHT)
    angle += radians(1);
}
