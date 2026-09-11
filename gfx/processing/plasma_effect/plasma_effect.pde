// Based on https://www.bidouille.org/prog/plasma
int pc;
int cc;

void setup() {
  size(640, 480);
  colorMode(RGB, 1);
  loadPixels();
  cc = pc = 0;
}

void draw() {
  float t = millis() / 1e3;
  int i = 0;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      float tx = map(x, 0, width, -0.5, 0.5);
      float ty = map(y, 0, height, -0.5, 0.5);
      color col = plasma(pc, cc, t, tx, ty);
      pixels[i++] = col;
    }
  }
  updatePixels();
}

color plasma(int pc, int cc, float t, float x, float y) {
  float r, g, b, v;
  float cx, cy;
  
  r = g = b = v = 0;
  if (pc == 0)
    v = sin(x*10 + t);
  else if (pc == 1)
    v = sin(10*(x*sin(t/2) + y*cos(t/3)) + t);
  else if (pc == 2) {
    cx = x + 0.5*sin(t/5);
    cy = y + 0.5*cos(t/3);
    v = sin(sqrt(100*(cx*cx + cy*cy) + 1) + t);
  } else if (pc == 3) {
    v += sin(x*10 + t);
    v += sin(10*(x*sin(t/2) + y*cos(t/3)) + t);
    
    cx = x + 0.5*sin(t/5);
    cy = y + 0.5*cos(t/3);
    v += sin(sqrt(100*(cx*cx + cy*cy) + 1) + t);
  } else if (pc == 4) {
    v = tan(x*y*10 * sin(t)) + cos(t);
  } else if (pc == 5) {
    v = x*sin(5*t) + y/cos(4*t) + x/5*cos(t);
  } else if (pc == 6) {
    v = 1/(0.01+tan(x+y*t));
  } else if (pc == 7) {
    v = abs(sin(x*y*t));
  }

  if (cc == 0) {
    r = sin(v*PI);
    g = cos(v*PI);
    b = 0;
  } else if (cc == 1) {
    r = 1;
    g = cos(v*PI);
    b = sin(v*PI);
  } else if (cc == 2) {
    r = sin(v*PI);
    g = sin(v*PI + 2*PI/3);
    b = sin(v*PI + 4*PI/3);
  } else if (cc == 3) {
    r = g = b = sin(v*5*PI);
  }
  
  r = map(r, -1, 1, 0, 1);
  g = map(g, -1, 1, 0, 1);
  b = map(b, -1, 1, 0, 1);
  return color(r, g, b);
}

void keyPressed() {
  if (keyCode == LEFT)
    pc--;
  else if (keyCode == RIGHT)
    pc++;
  else if (keyCode == UP)
    cc--;
  else if (keyCode == DOWN)
    cc++;
  pc = wrap(pc, 0, 7);
  cc = wrap(cc, 0, 3);
}

int wrap(int x, int a, int b) {
  if (x < a)
    x = b;
  else if (x > b)
    x = a;
  return x;
}
