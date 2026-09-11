float radius;
int state;

void setup() {
  size(1024, 768);
  background(150, 150, 150);
  radius = 1;
  state = 0;
}

void draw() {
  switch (state) {
  case 0:
    fill(10, 10, 10);
    circle(mouseX, mouseY, radius);
    if (mousePressed && mouseButton == LEFT) {
      fill(255, 0, 0);
      circle(mouseX, mouseY, radius*10);
      state = 1;
    }
    break;

  case 1:
    if (!mousePressed) {
      fill(0, 0, 255);
      circle(mouseX, mouseY, radius*10);
      state = 0;
    }
    break;
  }
}

void keyPressed() {
  if (key == ' ') {
    background(150, 150, 150);
    return;
  }
    
  if (key != CODED)
    return;

  float newRadius = radius;
  if (keyCode == LEFT)
    newRadius = max(radius-1, 1);
  else if (keyCode == RIGHT)
    newRadius = radius+1;

  if (newRadius != radius) {
    radius = newRadius;
    println("Radius: " + radius);
  }
}
