/*

https://www.masswerk.at/minskytron/
https://nbickford.wordpress.com/2011/04/03/the-minsky-circle-algorithm/

*/

MinskyCircle mcs[];
int mcslen;

void setup() {
  size(800, 600);
  
  mcslen = 10;
  alloc(mcslen);
  reset();
}

void alloc(int n) {
  mcs = new MinskyCircle[n];
  for (int i = 0; i < mcs.length; i++)
    mcs[i] = new MinskyCircle();
}

void reset() {
  background(0);
  for (MinskyCircle mc : mcs)
    mc.reset();
}

void keyPressed() {
  if (keyCode == UP)
    mcslen++;
  else if (keyCode == DOWN)
    mcslen--;
  mcslen = clamp(mcslen, 1, mcslen);
  
  for (MinskyCircle mc : mcs) {
    if (keyCode == LEFT)
      mc.mode--;
    else if (keyCode == RIGHT)
      mc.mode++;
    mc.mode = clamp(mc.mode, 0, 2);
  }
  
  if (keyCode == UP || keyCode == DOWN)
    alloc(mcslen);
  if (keyCode == ' ' || keyCode == LEFT || keyCode == RIGHT || keyCode == UP || keyCode == DOWN)
    reset();

  println("Mode:", mcs[0].mode);
  println("Length: ", mcslen);
  println();
}

void draw() {
  for (MinskyCircle mc : mcs) {
    mc.step();
    mc.draw();
  }
}

int clamp(int x, int a, int b) {
  if (x < a)
    x = a;
  else if (x > b)
    x = b;
  return x;
}

class MinskyCircle {
  int px, py;
  double c1, c2;
  double d1, d2;
  int mode;
  
  MinskyCircle() {
    mode = 2;
    reset();
  }
  
  void reset() {
    px = (int)random(5, 50);
    py = 0;
    c1 = random(-1, 1);
    c2 = random(-1, 1);
  }
  
  void step() {
    px = px - (int)(c1 * py);
    py = py + (int)(c2 * px);

    c1 += d1;
    c2 += d2;
    
    d1 = random(-0.1, 0.1);
    d2 = random(-0.1, 0.1);
  }
  
  void draw() {
    push();
    translate(width/2, height/2);
    stroke(0, 255, 0);
    strokeWeight(2);
    switch (mode) {
    case 0:
      point(px, py);
      break;
    case 1:
      disp2(px, py);
      break;
    case 2:
      disp8(px, py);
      break;
    }
    pop();
  }
  
  void disp2(int x, int y) {
    point(x, y);
    point(y, x);
  }
  
  void disp8(int x, int y) {
    point(x, y);
    point(-x, y);
    point(y, -x);
    point(-y, -x);
    point(-x, -y);
    point(x, -y);
    point(-y, x);
    point(y, x);
  }
}
