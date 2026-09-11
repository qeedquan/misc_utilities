float scale;
int mode;

void setup() {
  size(1024, 768);
  colorMode(RGB, 1);
  loadPixels();
  scale = 0.02;
  mode = 0;
}

void draw() {
  drawNoise(mode, millis(), scale, 0, 0, width, height);
  updatePixels();
}

void drawNoise(int mode, float t, float s, float x0, float y0, float w, float h) {
  int i = 0;
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      float g = gen(mode, t, (x+x0)*s, (y+y0)*s);
      pixels[i++] = color(g);
    }
  }
}

float gen(int mode, float t, float x, float y) {
  float v = 0;
  switch (mode) {
  case 0:
    v = noise(x, y);
    break;
  case 1:
    v = noise(x+random(0.5), y+random(0.5));
    break;
  case 2:
    v = noise(x*cos(t), y*sin(t));
    break;
  case 3:
    v = noise(sin(x*t), cos(y*x*t));
    break;
  }
  return v;
}

void keyPressed() {
  if (keyCode == LEFT)
    scale -= 0.01;
  else if (keyCode == RIGHT)
    scale += 0.01;
  else if (keyCode == UP)
    mode += 1;
  else if (keyCode == DOWN)
    mode -= 1;
  mode = wrap(mode, 0, 3);
  
  println("Mode:", mode);
  println("Scale:", scale);
  println();
}

int wrap(int x, int a, int b) {
  if (x < a)
    x = b;
  else if (x > b)
    x = a;
  return x;
}
