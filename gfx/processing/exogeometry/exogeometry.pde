// http://blog.kenperlin.com/?p=5641

PVector exo;
PVector offset;
float angle;
int style;
int fontsz;
int iterations;
boolean pause;
boolean rotating;

void setup() {
  size(1200, 800);
  strokeWeight(2);
  colorMode(RGB, 1);
  fontsz = 16;
  textSize(fontsz);
  
  exo = new PVector();
  offset = new PVector();
  style = 0;
  iterations = 8000;
  pause = false;
  rotating = true;
}

void draw() {
  background(0.7);
  
  push();
  translate(width/2, height/2);
  rotate(angle);
  translate(-width/2, -height/2);
  if (rotating) {
      angle += radians(1);
      angle %= 2*PI;
  }
  iterate(exo.x, exo.y, iterations, style);
  pop();
  
  text(String.format("Style: %d Iterations: %d Offset: %s", style, iterations, offset), 16, 16);
  text(String.format("Pause: %b Rotating: %b", pause, rotating), 16, 16 + 1*fontsz);
  text(String.format("P: %.6f Q: %.6f", exo.x, exo.y), 16, 16 + 2*fontsz);
}

void iterate(float p, float q, int n, int style) {
  push();

  float a = 0;
  float x = 0;
  float y = 0;
  for (int i = 0; i < n; i++) {   
    float x1 = x + cos(a) / 10;
    float y1 = y + sin(a) / 10;

    stroke(x, y, x*y);
    switch (style) {
    case 0:
      line(x2p(x), y2p(y), x2p(x1), y2p(y1));
      break;
    case 1:
      circle(x2p(x), y2p(y), 5);
      break;
    case 2:
      triangle(x2p(x), y2p(y), x2p(x1), y2p(y1), x2p((x+x1)/2), y2p((y+y1)/2));
      break;
    }
    
    x = x1;
    y = y1;
    a += 0.05 * (pow(x * x * q, p) + pow(y * y / q, p));
  }

  pop();
}

int x2p(float x) {
  return round(width/2.0 + width/8.0 * x);
}

int y2p(float y) {
  return round(height/2.0 - height/8.0 * y);
}

void keyPressed() {
  if (keyCode == BACKSPACE)
    rotating = !rotating;
  else if (keyCode == ENTER)
    pause = !pause;
  else if (key == ' ')
    style = (style + 1) % 3;
  else if (keyCode == LEFT)
    iterations -= 100;
  else if (keyCode == RIGHT)
    iterations += 100;
  else if (key == 'a')
    offset.x -= 0.1;
  else if (key == 'd')
    offset.x += 0.1;
  else if (key == 'w')
    offset.y += 0.1;
  else if (key == 's')
    offset.y -= 0.1;
  iterations = max(iterations, 0);
}

void mouseMoved() {
  if (pause)
    return;
  exo.x = 0.6*mouseX/width + 0.6;
  exo.y = pow(2, 2.0*mouseY/height-1);
  exo.x += offset.x;
  exo.y += offset.y;
}
