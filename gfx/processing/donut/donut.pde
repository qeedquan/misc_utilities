void setup() {
  size(640, 350);
}

void draw() {
  background(200);
  noFill();
  for (int angle = 0; angle < 360; angle += 3) {
    float t = radians(angle);
    float x = cos(t) * 96;
    float y = sin(t) * 70;
    circle(x + width/2, y + height/2, 60);
  }
}
