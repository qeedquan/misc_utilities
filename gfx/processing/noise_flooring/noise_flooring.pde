float noisescale;
float jitter;
float floorheight;

void setup() {
  size(1024, 768);
  noisescale = 0.02;
  jitter = 0.05;
  floorheight = width/2;
}

void draw() {
  background(200);

  for (int x = 0; x < width; x++) {
    float n = noise((mouseX+x)*noisescale, random(jitter)*mouseY*noisescale);
    stroke(n*255);
    line(x, mouseY + n*80, x, height);
  }
}

void keyPressed() {
  if (keyCode == LEFT)
    noisescale -= 0.01;
  else if (keyCode == RIGHT)
    noisescale += 0.01;
  else if (keyCode == UP)
    jitter += 0.05;
  else if (keyCode == DOWN)
    jitter -= 0.05;
  else if (key == 'w')
    floorheight -= 10;
  else if (key == 's')
    floorheight += 10;

  println("Noise Scale:", noisescale);
  println("Jitter:", jitter);
  println("Floor Height:", floorheight);
  println();
}
