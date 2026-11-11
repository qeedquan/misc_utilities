// https://en.wikipedia.org/wiki/Archimedean_spiral

class Spiral {
  float x0, y0;
  float x1, y1;
  float r, v;
  float t;
  
  Spiral() {
    r = 3;
    v = 0;
    reset();
  }
  
  void update(float dt) {
    t += dt;
    x0 = x1;
    y0 = y1;
    x1 = (r*t + v)*cos(t);
    y1 = (r*t + v)*sin(t);
  }
  
  void reset() {
    x0 = y0 = x1 = y1 = t = 0;
  }
}

color colbg;
Spiral spiral;

void setup() {
  colbg = color(50, 60, 100);
  spiral = new Spiral();
  
  size(1024, 768);
  background(colbg);
}

void draw() {
  translate(width/2, height/2);
  spiral.update(0.2);
  stroke(255, 255, 255);
  line(spiral.x0, spiral.y0, spiral.x1, spiral.y1);
}

void keyPressed() {
  if (key == ' ') {
    background(colbg);
    spiral.reset();
  } else if (keyCode == LEFT) {
    spiral.v -= 10;
  } else if (keyCode == RIGHT) {
    spiral.v += 10;
  }
}
