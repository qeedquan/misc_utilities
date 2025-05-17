ArrayList<PVector> fss = new ArrayList<PVector>();
ArrayList<PVector> fscols = new ArrayList<PVector>();

ArrayList<PVector> tris = new ArrayList<PVector>();
ArrayList<PVector> tricols = new ArrayList<PVector>();

int mode;

void setup() {
  size(1024, 768);
  colorMode(RGB, 1);
  strokeWeight(3);
  textSize(16);
  mode = TRIANGLE_FAN;
}

void conv() {
  if (mode == TRIANGLE_FAN)
    fan2tri(fss, fscols, tris, tricols);
  else
    strip2tri(fss, fscols, tris, tricols);
}

void keyPressed() {
  if (key == '1')
    mode = TRIANGLE_FAN;
  else if (key == '2')
    mode = TRIANGLE_STRIP;
  conv();
}

void mouseClicked() {
  if (mouseButton == RIGHT) {
    fss.clear();
    tris.clear();
    fscols.clear();
    tricols.clear();
    return;
  }
  
  if (mouseY < height/2) {
    fss.add(new PVector(mouseX, mouseY));
    fscols.add(new PVector(random(0, 1), random(0, 1), random(0, 1)));
    conv();
  }
}

void draw() {
  background(80, 100, 100);
  line(0, height/2, width, height/2);
  shape(mode, fss, fscols);
  fill(0, 0, 0);
  text((mode == TRIANGLE_FAN) ? "Triangle Fan" : "Triangle Strip", 20, 20);
  pushMatrix();
  translate(0, height/2);
  shape(TRIANGLES, tris, tricols);
  fill(0, 0, 0);
  text("Triangle", 20, 20);
  popMatrix();
}

void shape(int shape, ArrayList<PVector> verts, ArrayList<PVector> cols) {
  if (verts.size() >= 3) {
    beginShape(shape);
    for (int i = 0; i < verts.size(); i++) {
      PVector v = verts.get(i);
      PVector c = cols.get(i);
      fill(c.x, c.y, c.z);
      vertex(v.x, v.y);
    }
    endShape();
  }
  
  for (int i = 0; i < verts.size(); i++) {
    PVector v = verts.get(i);
    point(v.x, v.y);
  }
}

void fan2tri(ArrayList<PVector> fans, ArrayList<PVector> fancols, ArrayList<PVector> tris, ArrayList<PVector> tricols) {
  tris.clear();
  tricols.clear();
  
  if (fans.size() < 3) {
    tris.addAll(fans);
    tricols.addAll(fancols);
    return;
  }

  for (int i = 1; i < fans.size()-1; i++) {
    tris.add(fans.get(0));
    tris.add(fans.get(i));
    tris.add(fans.get(i+1));
    
    tricols.add(fancols.get(0));
    tricols.add(fancols.get(i));
    tricols.add(fancols.get(i+1));
  }
}

void strip2tri(ArrayList<PVector> strips, ArrayList<PVector> stripcols, ArrayList<PVector> tris, ArrayList<PVector> tricols) {
  tris.clear();
  tricols.clear();
  
  if (strips.size() < 3) {
    tris.addAll(strips);
    tricols.addAll(stripcols);
    return;
  }

  for (int i = 0; i < strips.size()-2; i++) {
    tris.add(strips.get(i));
    tris.add(strips.get(i+1));
    tris.add(strips.get(i+2));
    
    tricols.add(stripcols.get(i));
    tricols.add(stripcols.get(i+1));
    tricols.add(stripcols.get(i+2));
  }
}

void dump(ArrayList<PVector> verts) {
  for (int i = 0; i < verts.size(); i++) {
    PVector v = verts.get(i);
    println(v.x + " " + v.y + " " + v.z);
  }
}
