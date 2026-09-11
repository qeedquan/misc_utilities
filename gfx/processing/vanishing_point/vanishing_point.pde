/*

https://web.stanford.edu/class/cs231a/course_notes/02-single-view-metrology.pdf
https://lefthandwriter.github.io/vision/2019/02/06/Vanishing-Points.html
https://cs.adelaide.edu.au/~ianr/Teaching/CompGeom/lec2.pdf

*/

PVector center;
PVector vanish;
ArrayList<PVector> lines;

void setup() {
  surface.setTitle("Vanishing Point");
  size(800, 800, P3D);
  strokeWeight(3);
  reset();
}

void reset() {  
  center = new PVector(width/2, height/2, 0);
  vanish = new PVector(center.x, center.y, 0);
  
  lines = new ArrayList<PVector>();
  lines.add(new PVector(center.x - 50, height, 0));
  lines.add(new PVector(center.x + 50, height, 0));
}

void draw() {
  background(125, 194, 240);
  stroke(30, 30, 80);
  line(0, vanish.y, 0, width, vanish.y, 0);
  
  fill(0, 0, 100);
  circle(vanish.x, vanish.y, 10);
  
  stroke(50, 50, 50);
  for (int i = 0; i < lines.size(); i++) {
    PVector ln = lines.get(i);
    line(ln.x, ln.y, vanish.x, vanish.y);
  }
  stroke(200, 0, 0);
  line(vanish.x, vanish.y, mouseX, height);
  stroke(0, 0, 200);
  line(mouseX, vanish.y, mouseX, height);
}

void keyPressed() {
  if (key == 'r')
    reset();
}

void mouseClicked() {
  if (mouseButton == LEFT) {
    vanish.x = mouseX;
    vanish.y = mouseY;
  } else if (mouseButton == RIGHT) {
    lines.add(new PVector(mouseX, height, 0));
  }
}
