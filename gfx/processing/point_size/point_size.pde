void setup() {
  size(1440, 900);
}

void draw() {
  background(255);
  dots(0, 0, 80);
}

void dots(int x0, int y0, int n) {
  int x = x0 + n/2;
  int y = y0 + n/2;
  for (int i = 0; i < n; i++) {
    stroke(0, 0, i);
    strokeWeight(i);
    point(x, y);
    x += 2*n;
    if (x + 2*n >= width) {
      x = x0 + n/2;
      y = y + n;
    }
  }
}
