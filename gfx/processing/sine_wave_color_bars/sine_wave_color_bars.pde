float phase;

void setup() {
  size(1024, 768);
}

void draw() {
  for (int i = 0; i < width; i++) {
    float r = map(i, 0, width, 0, TWO_PI);
    stroke(cos(r+2*phase) * 255, sin(r+phase) * 255, cos(r*r*phase)*sin(r*phase)*255);
    line(i, 0, i, height);
  }
  phase += 0.1;
}
