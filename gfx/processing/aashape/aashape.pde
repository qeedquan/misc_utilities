float theta = 100;
float radius = 200;
int life;
int lifeforce;

void settings() {
  size(1024, 768);
  
  // can only be set on startup
  // 0 - no antialiasing
  // 2 - 2x antialiasing
  // 3 - 3x antialiasing
  // 4 - 4x antialiasing
  // etc
  int aa = 3;
  smooth(aa);
  
  lifeforce = 1;
  life = 255;
}

void keyPressed() {
  if (key == ' ')
    radius += 15;
}

void draw() {
  if (mousePressed) {
    theta = (theta + 5) % 360;
  }

  background(255);
  drawLines();
  drawCircles();
}

void drawLines() {
  for (float t = theta; t < 360+theta; t += 15) {
    float r = radius;
    float p = t / (360+theta);
    float x = cos(radians(t));
    float y = sin(radians(t));
    float x0 = mouseX;
    float y0 = mouseY;
    float x1 = x0 + x*r*(1+p);
    float y1 = y0 + y*r*(1+p);
    line(x0, y0, x1, y1);
  }
}

void drawCircles() {
  push();
  fill(127, 255);
  for (int x = 1; x < 10; x++) {
    strokeWeight(x);
    circle(100 + x*100, 100, 40);
    ellipse(100 + x*100, 200, 30, 40);
  }
  strokeWeight(1);
  pop();
  
  if (life == 0 || life == 255)
    lifeforce = -lifeforce;
  life += lifeforce;
  
  push();
  fill(127, 13, 45, life);
  stroke(127, life);
  circle(512, 512, 100);
  fill(200, 31, 5, 255-life);
  circle(550, 512, 100);
  pop();
}
