/*

https://www.johndcook.com/expsum/details.html

The exponential sum plots on this site draw lines between the consecutive partial sums of

Sum[n=0, N] exp(2*PI*I * (n/m + n*n/d + n*n*n/y))

where m is the month, d is the day, and y is the last two digits of the year.
I have used different values of N over time. Currently I use N = 2 lcm(m, d, y) + 1.

*/

Expsum expsum;

void setup() {
  size(1024, 768);
  expsum = new Expsum();
}

void draw() {
  background(230);
  expsum.draw();
}

void keyPressed() {
  if (keyCode == LEFT) {
    expsum.day -= 1;
  } else if (keyCode == RIGHT) {
    expsum.day += 1;
  } else if (keyCode == UP) {
    expsum.year += 1;
  } else if (keyCode == DOWN) {
    expsum.year -= 1;
  } else if (key == 'q') {
    expsum.month -= 1;
  } else if (key == 'w') {
    expsum.month += 1;
  }
  
  expsum.month = clamp(expsum.month, 1, 12);
  expsum.day = clamp(expsum.day, 1, 31);
  expsum.year = clamp(expsum.year, 1, 99);
  expsum.gen();
}

int clamp(int x, int a, int b) {
  if (x < a)
    x = a;
  else if (x > b)
    x = b;
  return x;
}

int gcd(int a, int b) {
  while (b != 0) {
    int c = a % b;
    a = b;
    b = c;
  }
  return a;
}

int lcm(int a, int b) {
  return abs(a*b) / gcd(a, b);
}

class Rect {
  PVector min, max;
  
  Rect() {
    min = new PVector(Integer.MAX_VALUE, Integer.MAX_VALUE);
    max = new PVector(0, 0);
  }
}

class Expsum {
  PVector[] points;
  Rect extents;
  int day;
  int month;
  int year;
  
  Expsum() {
    this(day(), month(), year() % 100);
  }
  
  Expsum(int day, int month, int year) {
    this.day = day;
    this.month = month;
    this.year = year;
    gen();
  }

  void gen() {
    int N = 2*lcm(month, lcm(day, year)) + 1;
    points = new PVector[N + 1];
    extents = new Rect();
    for (int n = 0; n <= N; n++) {
      float t = (n*1.0)/month + (n*n*1.0)/day + (n*n*n*1.0)/year;
      PVector p = new PVector(cos(2*PI*t), sin(2*PI*t));
      if (n > 0) {
        p.x += points[n-1].x;
        p.y += points[n-1].y;
      }
      
      points[n] = p;
      extents.min.x = min(extents.min.x, p.x);
      extents.min.y = min(extents.min.y, p.y);
      extents.max.x = max(extents.max.x, p.x);
      extents.max.y = max(extents.max.y, p.y);
    }
  }
  
  void draw() {
    drawShape();
    drawStatus();
  }
  
  void drawStatus() {
    String str = String.format("%02d/%02d/%02d (%d points)", month, day, year, points.length);

    push();
    textSize(32);
    fill(0);
    text(str, 16, 32);
    pop();
  }
  
  void drawShape() {
    int n = points.length;
    if (n == 0)
      return;

    push();
    stroke(0, 68, 136);
    translate(width/2-100, height/3);

    float w = 300;
    float h = 300;
    for (int i = 1; i < n; i++) {
      float x1 = map(points[i-1].x, extents.min.x, extents.max.x, 0, w);
      float y1 = map(points[i-1].y, extents.min.y, extents.max.y, 0, h);
      float x2 = map(points[i].x, extents.min.x, extents.max.x, 0, w);
      float y2 = map(points[i].y, extents.min.y, extents.max.y, 0, h);

      line(x1, y1, x2, y2);
    }
    pop();
  }
}
