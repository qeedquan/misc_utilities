/*

Any vector can be written as a linear combination of its basis vector
We want to see how various linear combinations of X, Y, and Z axes affect
the resulting vector and its angle in relation to the standard axes.

*/

int[] angles;
int xyzmask;
PGraphics textLayer;

void setup() {
  size(1200, 800, P3D);
  colorMode(RGB, 1);

  angles = new int[3];
    
  textLayer = createGraphics(width, height, JAVA2D);
  textLayer.smooth(4);
}

void draw() {
  hint(ENABLE_DEPTH_TEST);
  
  background(0.5, 0.5, 0.5);
  resetMatrix();
  camera(2.5, -2.5, 5, 0, 0, 0, 0, 1, 0);
  scale(1, -1, 1);
  perspective(radians(50), width*1.0/height, 1, 1000);
  
  PMatrix3D id = new PMatrix3D();
  PMatrix3D xr = new PMatrix3D();
  PMatrix3D yr = new PMatrix3D();
  PMatrix3D zr = new PMatrix3D();
  id.reset();
  xr.rotateY(radians(angles[0]));
  yr.rotateZ(radians(angles[1]));
  zr.rotateY(radians(angles[2]));

  drawAxes(id, 2, 1, 0x7);
  drawAxes(xr, 0.8, 0.8, 0x1);
  drawAxes(yr, 0.8, 0.8, 0x2);
  drawAxes(zr, 0.8, 0.8, 0x4);
  PVector plc = drawPlaneCombination(xr, yr, zr, xyzmask);
  drawHUD(plc);
}

PVector drawPlaneCombination(PMatrix3D xr, PMatrix3D yr, PMatrix3D zr, int mask) {
  PVector p = new PVector(0, 0, 0);
  if ((mask & 1) != 0)
    p = PVector.add(p, xr.mult(new PVector(1, 0, 0), null));
  if ((mask & 2) != 0)
    p = PVector.add(p, yr.mult(new PVector(0, 1, 0), null));
  if ((mask & 4) != 0)
    p = PVector.add(p, zr.mult(new PVector(0, 0, 1), null));
  p.normalize();
  
  stroke(1);
  beginShape(LINES);
  vertex(0, 0, 0);
  vertex(p.x, p.y, p.z);
  endShape();
  
  return p;
}

void drawAxes(PMatrix3D m, float l, float c, int mask) {
  PVector[] tab = {
    new PVector(1, 0, 0),
    new PVector(0, 1, 0),
    new PVector(0, 0, 1),
  };
  
  for (int i = 0; i < tab.length; i++) {
    if (((1<<i) & mask) == 0)
      continue;

    PVector p = PVector.mult(tab[i], l);
    PVector t = PVector.mult(tab[i], c);
    p = m.mult(p, null);
    
    strokeWeight(2);
    beginShape(LINES);
    stroke(abs(t.x), abs(t.y), abs(t.z));
    vertex(p.x, p.y, p.z);
    vertex(-p.x, -p.y, -p.z);
    endShape();
    
    strokeWeight(5);
    beginShape(POINTS);
    stroke(0);
    vertex(p.x, p.y, p.z);
    endShape();
  }
}

void drawHUD(PVector plc) {
  String comb = "";
  if ((xyzmask&1) != 0)
    comb += "X";
  if ((xyzmask&2) != 0)
    comb += "Y";
  if ((xyzmask&4) != 0)
    comb += "Z";
  
  // for any two orthogonal vectors, the vector generated will be halfway between so 45 degrees
  float ax = PVector.angleBetween(plc, new PVector(1, 0, 0));
  float ay = PVector.angleBetween(plc, new PVector(0, 1, 0));
  float az = PVector.angleBetween(plc, new PVector(0, 0, 1));
  float at = ax + ay + az;
    
  String str = "X: " + angles[0] + "\n";
  str += "Y: " + angles[1] + "\n";
  str += "Z: " + angles[2] + "\n";
  str += "Combination: " + comb + "\n";
  str += "Angle: " + degrees(ax) + " " + degrees(ay) + " " + degrees(az) + "\n";
  str += "Total Angle: " + degrees(at) + "\n";
  str += "Magnitude: " + plc.mag();
  
  push();
  camera();
  imageMode(CORNER);
  rectMode(CORNER);
  textLayer.beginDraw();
  textLayer.background(0, 0, 0, 0);
  textLayer.fill(255);
  textLayer.textSize(16);
  textLayer.textAlign(LEFT, LEFT);
  textLayer.text(str, width>>1, height>>1);
  textLayer.endDraw();
  translate(-width*0.35, -height/3);
  image(textLayer, 0, 0);
  pop();
}

void keyPressed() {
  if (key == CODED) {
    if (keyCode == LEFT)
      xyzmask--;
    else if (keyCode == RIGHT)
      xyzmask++;
  } else if (key == 's')
    angles[1]--;
  else if (key == 'w')
    angles[1]++;
  else if (key == 'a')
    angles[0]--;
  else if (key == 'd')
    angles[0]++;
  else if (key == 'q')
    angles[2]--;
  else if (key == 'e')
    angles[2]++;
  else if (key == ' ')
    angles[0] = angles[1] = angles[2] = 0;

  for (int i = 0; i < 3; i++) {
    if (angles[i] < 0)
      angles[i] += 360;
    else if (angles[i] >= 360)
      angles[i] -= 360;
  }
  xyzmask &= 7;
}
