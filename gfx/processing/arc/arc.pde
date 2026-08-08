void setup() {
  size(1024, 768);
}

void draw() {
  background(200);
  
  float tz = 10;
  float ang = angle(width/2, height/2, mouseX, mouseY);
  float len = length(width/2, height/2, mouseX, mouseY);
  arrow(0, height/2, width-tz-5, height/2, tz);
  arrow(width/2, height, width/2, tz+5, tz);
  arrow(width/2, height/2, mouseX, mouseY, tz);
  text("Angle: " + degrees(ang), 10, 16);
  text("Length: " + len, 10, 36);

  pushMatrix();
  fill(50, 50, 150);
  translate(width/2, height/2);
  scale(1, -1);
  arc(0, 0, 50, 50, 0, ang);
  popMatrix();
}

void arrow(float x1, float y1, float x2, float y2, float tz) {
  strokeWeight(2);
  stroke(0, 0, 0);
  fill(0, 0, 0);
  line(x1, y1, x2, y2);
  
  float ang = atan2(y2-y1, x2-x1) + radians(270);
  pushMatrix();
  translate(x2, y2);
  rotate(ang);
  triangle(-tz, 0, tz, 0, 0, tz);
  popMatrix();
}

float angle(float x1, float y1, float x2, float y2) {
  float x = atan2(y2-y1, x2-x1);
  if (x < 0)
    x = abs(x);
  else
    x = radians(360) - x;
  return x;
}

float length(float x1, float y1, float x2, float y2) {
  float dx = x2 - x1;
  float dy = y2 - y1;
  return sqrt(dx*dx + dy*dy);
}
