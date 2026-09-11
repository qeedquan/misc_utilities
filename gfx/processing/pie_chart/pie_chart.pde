import java.util.*;

Pie pie;

void setup() {
  size(1024, 768);
  pie = new Pie();
}

void draw() {
  background(140, 150, 100);
  pie.draw();
}

void keyPressed() {
  if (key == ' ')
    pie.gen();
}

void mousePressed() {
  pie.mousePressed();
}

void mouseWheel(MouseEvent ev) {
  pie.mouseWheel(ev);
}

PVector randrgb() {
  return new PVector(random(255), random(255), random(255));
}

float[] percents() {
  float   p = 1;
  float[] v = new float[50];
  int     i = 0;
  for (; i < v.length; i++) {
    float x = map(randomGaussian(), -2.5, 2.5, 0.1, random(v.length/(i+1))*1.0/v.length);
    v[i] = min(x, p);
    p -= v[i];
    if (p <= 0)
      break;
  }
  if (i > 0)
    v[i] += p;

  return Arrays.copyOfRange(v, 0, i+1);
}

class Pie {
  PVector   disc;
  float[]   values;
  PVector[] colors;
  float     startAngle;
  
  Pie() {
    disc = new PVector(width/2, height/2, 200);
    gen();
  }
  
  void gen() {
    values = percents();
    colors = new PVector[values.length];
    for (int i = 0; i < colors.length; i++)
      colors[i] = randrgb();
    startAngle = random(TWO_PI);
  }
  
  void draw() {
    strokeWeight(2);
    float t = startAngle;
    float r = disc.z;
    float x = disc.x;
    float y = disc.y;
    for (int i = 0; i < values.length; i++) {
      float dt = map(values[i], 0, 1, 0, TWO_PI);
      fill(colors[i].x, colors[i].y, colors[i].z);
      arc(x, y, 2*r, 2*r, t, t+dt);

      float c1 = r*cos(t);
      float s1 = r*sin(t);
      float c2 = r*cos(t+dt);
      float s2 = r*sin(t+dt);
      stroke(30);
      line(x, y, x + c1, y + s1);
      line(x, y, x + c2, y + s2);
      
      t += dt;
    }
  }
  
  void mousePressed() {
    disc.x = mouseX;
    disc.y = mouseY;
  }
  
  void mouseWheel(MouseEvent ev) {
    disc.z *= exp(0.05*ev.getCount());
  }
}
