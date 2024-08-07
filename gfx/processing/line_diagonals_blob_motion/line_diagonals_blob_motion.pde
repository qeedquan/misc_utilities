/*

https://www.generativehut.com/post/using-noise-to-create-looping-gifs-on-processing

*/

int ndiags;
int radius;
float ticks;
boolean animating;

void setup() {
  size(1024, 768);
  ndiags = 60;
  radius = 100;
  animating = false;
}

void draw() {
  background(150);
  diagonals(ticks, 0, 0, width, height, radius, (width*2)/ndiags, ndiags);
  if (animating)
    ticks += 0.01;
}

void diagonals(float t, float x0, float y0, float w, float h, float r0, float s, int n) {
  for (int y = 0; y < n; y++) {
    for (int x = 0; x < n; x++) {
      float r = r0*noise(x*s, y*s, cos(2*PI*t)) + r0*abs(sin(2*PI*t));
      float xc = w + w/4 * cos(2*PI*t)*sin(PI*t + random(0.2));
      float yc = h + h/4 * sin(2*PI*t)*cos(0.5*t + random(0.1));

      if (dist(x*s, y*s, (xc-w/2), (yc-h/2)) < r)
        line(x0 + x*s, y*s, (x+1)*s, (y+1)*s);
      else
        line(x0 + (x+1)*s, y0 + y*s, x0 + (x-1)*s, y0 + (y+1)*s);
    }
  }
}

void keyPressed() {
  if (key == ' ')
    animating = !animating;
    
  switch (keyCode) {
  case LEFT:
    ndiags--;
    break;
  case RIGHT:
    ndiags++;
    break;
  case UP:
    radius -= 10;
    break;
  case DOWN:
    radius += 10;
    break;
  case ENTER:
    println("Diagonals:", ndiags);
    println("Radius:", radius);
    println("Animating:", animating);
    break;
  }
}
