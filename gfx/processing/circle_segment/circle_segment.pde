/*

https://en.wikipedia.org/wiki/Circular_segment

*/

int fontsz;
boolean animating;

PVector disc;
int direction;
float segment;

void setup() {
  size(1024, 768);
  strokeWeight(2);
  disc = new PVector(width/2, height/2, 200);
  segment = radians(270);
  direction = 0;
  animating = true;
  fontsz = 16;
  textSize(fontsz);
}

void draw() {
  background(150, 130, 120);

  circle(disc.x, disc.y, disc.z*2);

  float t = map(sin(segment), -1, 1, 0, 1);
  stroke(40, 60, 100);
  drawCircularSegment(disc, round(t*2*disc.z), direction);
  
  text(String.format("Segment: %.6f", t), 16, 16 + 0*fontsz);
  text(String.format("Direction: %d", direction), 16, 16 + 1*fontsz);
  
  if (animating)
    segment = (segment + radians(1)) % TWO_PI;
}

void keyPressed() {
  if (key == ' ')
    animating = !animating;
  else if (keyCode == LEFT)
    direction--;
  else if (keyCode == RIGHT)
    direction++;

  direction %= 4;
  if (direction < 0)
    direction += 4;
}

void mousePressed() {
  if (mouseButton == LEFT) {
    disc.x = mouseX;
    disc.y = mouseY;
  }
}

void mouseWheel(MouseEvent ev) {
  disc.z *= exp(0.5*ev.getCount());
}

/*

A circular segment is a region of a circle which is "cut off" from the rest of the circle by a secant or a chord.
We can describe this using the following equation:

R = h + d = h/2 + c^2/8h
R: radius
h: height of the segment
d: height of the triangular portion
c: chord length
t: [0, 2*R]

*/

void drawCircularSegment(PVector c, int t, int dir) {
  float r = c.z;
  for (int i = 0; i < t; i++) {
    float h = (i <= r) ? i : 2*r - i;
    float s = sqrt(8*h*(r - h/2.0));    

    float x1 = 0, x2 = 0, y1 = 0, y2 = 0;
    switch (dir) {
    case 0: // top-bottom
      y1 = c.y - r + i;
      y2 = y1;
      x1 = c.x - r + (2*r - s)/2;
      x2 = x1 + s;
      break;
    
    case 1: // bottom-top
      y1 = c.y + r - i;
      y2 = y1;
      x1 = c.x - r + (2*r - s)/2;
      x2 = x1 + s;
      break;
      
    case 2: // left-right
      y1 = c.y - r + (2*r - s)/2;
      y2 = y1 + s;
      x1 = c.x - r + i;
      x2 = x1;
      break;

    case 3: // right-left
      y1 = c.y - r + (2*r - s)/2;
      y2 = y1 + s;
      x1 = c.x + r - i;
      x2 = x1;
      break;
      
    default:
      assert(false);
      break;
    }
    line(x1, y1, x2, y2);
  }
}
