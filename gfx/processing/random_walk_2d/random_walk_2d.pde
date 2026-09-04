PVector pos;

void setup() {
  size(1200, 800);
  reset();
}

void reset() {
  pos = new PVector(width/2, height/2);
  background(150);
}

void draw() {
  pos.x += random(-1, 1);
  pos.y += random(-1, 1);
  point(pos.x, pos.y);
}

void keyPressed() {
  if (key == ' ')
    reset();
  else if (key == 'r') {
    pos.x = random(width);
    pos.y = random(height);
  }
}
