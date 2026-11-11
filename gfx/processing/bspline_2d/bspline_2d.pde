/*

https://pomax.github.io/bezierinfo/
https://en.wikipedia.org/wiki/B-spline
https://en.wikipedia.org/wiki/De_Boor%27s_algorithm
https://mathworld.wolfram.com/B-Spline.html

*/

BSpline spl;
int fontsz;
int mouseState;

void setup() {
  size(1024, 768);
  strokeWeight(2);

  fontsz = 16;
  textSize(fontsz);
  
  spl = new BSpline();
  mouseState = 0;
}

void draw() {
  background(200);
  spl.draw();
  
  if (keyPressed && key == 'd')
    keyPressed();
  if (mousePressed)
    mousePressed();
  else if (!mousePressed) {
    mouseState = 0;
    spl.select = -1;
  }
}

void keyPressed() {
  if (keyCode == ' ')
    spl.randomCurve(spl.degree + 1);
  else if (keyCode == LEFT)
    spl.adjustDegree(-1);
  else if (keyCode == RIGHT)
    spl.adjustDegree(1);
  else if (key == 'd') {
    if (spl.select != -1)
      spl.deletePoint(spl.select);
  } else if (keyCode == ENTER)
    spl.dump();
}

void mousePressed() {
  if (mouseButton == LEFT && (mouseState&0x1) == 0) {
    spl.addPoint(mouseX, mouseY);
    mouseState |= 0x1;
  } else if (mouseButton == RIGHT) {
    if (spl.select != -1) {
      spl.points.get(spl.select).set(mouseX, mouseY);
    }
  }
}

boolean inCircle(PVector c, PVector p) {
  float dx = p.x - c.x;
  float dy = p.y - c.y;
  float r = c.z/2;
  return dx*dx + dy*dy < r*r;
}

class BSpline {
  ArrayList<PVector> points;
  ArrayList<Float> knots;
  ArrayList<Float> weights;
  ArrayList<PVector> curve;
  ArrayList<PVector> colors;
  int degree;
  int select;
  float steps;
  
  BSpline() {
    randomCurve(4);
    select = -1;
  }
  
  void randomCurve(int order) {
    points = new ArrayList<PVector>();
    knots = new ArrayList<Float>();
    weights = new ArrayList<Float>();
    curve = new ArrayList<PVector>();
    colors = new ArrayList<PVector>();
    degree = order - 1;
    steps = 0.001;

    for (int i = 0; i <= degree; i++)
      addPoint(random(width), random(height));
    resizeKnots();
  }
  
  void resizeKnots() {
    knots.clear();
    for (int i = 0; i < 2*degree + points.size()-1; i++)
      knots.add(i*1.0);
  }
  
  void addPoint(float x, float y) {
    points.add(new PVector(x, y));
    weights.add(1.0);
    curve.add(new PVector());
    colors.add(new PVector(20+random(230), 20+random(230), 20+random(230)));
    resizeKnots();
  }
  
  void deletePoint(int s) {
    if (points.size() == 1)
      return;

    points.remove(s);
    weights.remove(weights.size()-1);
    curve.remove(curve.size()-1);
    degree = max(min(degree, points.size()-1), 0);
    resizeKnots();
  }
  
  void adjustDegree(int d) {
    d += degree;
    if (d >= points.size())
      return;
    if (d < 1)
      return;
    degree = d;
    resizeKnots();
  }
  
  void draw() {
    float t0 = degree;
    float t1 = points.size();
    float dt = steps;
    
    for (float t = t0; t <= t1; t += dt) {
      PVector p = evalCurve(t);
      point(p.x/p.z, p.y/p.z);
    }
    
    PVector m = new PVector(mouseX, mouseY, 40);
    for (int i = 0; i < points.size(); i++) {
      PVector c = colors.get(i);
      PVector p = points.get(i);
      
      if ((select == -1 && inCircle(m, p)) || select == i) {
        fill(10, 10, 10);
        select = i;
      } else
        fill(c.x, c.y, c.z);
      circle(p.x, p.y, m.z);
      
      fill(255, 255, 255);
      textAlign(CENTER);
      text("P"+i, p.x, p.y+fontsz/2);
    }
  }
  
  PVector evalCurve(float t) {
    if (curve.size() < 1)
      return new PVector();

    int k = (int)t;
    int p = degree;
    
    for (int i = 0; i <= p; i++) {
      PVector P = points.get(i+k-p);
      float   w = weights.get(i+k-p);
      PVector d = curve.get(i);
      d.x = P.x * w;
      d.y = P.y * w;
      d.z = w;
    }

    for (int r = 1; r <= p; r++) {
      for (int i = p; i >= r; i--) {
        float a = (t - knots.get(i+k-p)) / (knots.get(i+1+k-r) - knots.get(i+k-p));
        
        PVector c = curve.get(i-1);
        PVector d = curve.get(i);
        
        d.x = (1-a)*c.x + a*d.x;
        d.y = (1-a)*c.y + a*d.y;
        d.z = (1-a)*c.z + a*d.z;
      }
    }
    
    return curve.get(p);
  }
  
  void dump() {
    println("Degree:", spl.degree);
    println("Points:", spl.points.size());
    println("Knots:", spl.knots.size());
    for (int i = 0; i < spl.knots.size(); i++)
      print(spl.knots.get(i) + " ");
    println();
    println();
  }
}
