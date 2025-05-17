int slices = 20;

void setup() {
  size(1024, 768);
  textSize(16);
  render();
}

void draw() {
}

void render() {
  background(50, 50, 100);
  drawGrid(slices);
  fill(255, 255, 255);
  text("Slices: " + slices, 16, 16);
}

void drawGrid(int n) {
  for (int i = -1; i <= n; i++) {
    float y = -1.0 + 2.0*i/n;
    for (int j = -1; j <= n; j++) {
      float x = -1.0 + 2.0*j/n;
      float d = n*sqrt(x*x + y*y);
      float a = atan2(y, x);
      float px = map(x, -1, 1, 0, width);
      float py = map(y, -1, 1, 0, height);
      a = map(a, -PI/2, PI/2, 0, 255);
      fill(a, a, a);
      circle(px, py, d);
    }
  }
}

void keyPressed() {
  if (key == CODED) {
    if (keyCode == RIGHT) {
      slices++;
      render();
    }
    if (keyCode == LEFT) {
      slices = (slices < 1) ? 1 : slices-1;
      render();
    }
  }
}
