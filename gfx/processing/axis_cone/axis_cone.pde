PVector angle = new PVector();
int     auto = 0;

void setup() {
  size(1024, 1024, P3D);
}

void keyPressed() {
  float step = 1.0/(2*PI);
  if (key == 'w')
    angle.y += step;
  if (key == 's')
    angle.y -= step;
  if (key == 'a')
    angle.x -= step;
  if (key == 'd')
    angle.x += step;
  if (key == 'q')
    angle.z += step;
  if (key == 'e')
    angle.z -= step;
  if (key == '1')
    auto ^= 0x1;
  if (key == '2')
    auto ^= 0x2;
  if (key == '3')
    auto ^= 0x4;
}

void draw() {
  background(100);
  lights();
  
  float step = 0.01;
  if ((auto&0x1) != 0)
    angle.x += step;
  if ((auto&0x2) != 0)
    angle.y += step;
  if ((auto&0x4) != 0)
    angle.z += step;
    
  if (angle.x >= 2*PI)
    angle.x -= 2*PI;
  if (angle.y >= 2*PI)
    angle.y -= 2*PI;
  if (angle.z >= 2*PI)
    angle.z -= 2*PI;
  
  drawAxis('x', new RGB(255, 0, 0));
  drawAxis('y', new RGB(0, 255, 0));
  drawAxis('z', new RGB(0, 0, 255));
  drawAxis('w', new RGB(255, 255, 255));

  fill(255);
  textSize(16);
  text("X: " + angle.x, 100, 100);
  text("Y: " + angle.y, 100, 116);
  text("Z: " + angle.z, 100, 132);
}

void drawAxis(int axis, RGB col) {
  float radius = 30;
  float tall = 80;
  int sides = 20;
  
  pushMatrix();
  translate(width/2, height/2);
  switch (axis) {
  case 'x':
    rotateZ(PI/2 + angle.x);
    break;
  case 'y':
    rotateX(angle.y);
    break;
  case 'z':
    rotateX(PI/2);
    rotateZ(angle.z);
    break;
  case 'w':
    rotateX(-PI/4 + angle.x);
    rotateZ(PI/4 + angle.z);
    rotateY(PI/4 + angle.y);
    break;
  }
  
  translate(0, -5*tall);
  fill(col.r, col.g, col.b);

  drawCylinder(1, radius, tall, sides);
  translate(0, tall);
  drawCylinder(radius, radius, 4*tall, sides);
  popMatrix();
}

void drawCylinder(float topRadius, float bottomRadius, float tall, int sides) {
  float angle = 0;
  float angleIncrement = TWO_PI / sides;
  beginShape(QUAD_STRIP);
  for (int i = 0; i < sides + 1; ++i) {
    vertex(topRadius*cos(angle), 0, topRadius*sin(angle));
    vertex(bottomRadius*cos(angle), tall, bottomRadius*sin(angle));
    angle += angleIncrement;
  }
  endShape();
  
  // If it is not a cone, draw the circular top cap
  if (topRadius != 0) {
    angle = 0;
    beginShape(TRIANGLE_FAN);
    
    // Center point
    vertex(0, 0, 0);
    for (int i = 0; i < sides + 1; i++) {
      vertex(topRadius * cos(angle), 0, topRadius * sin(angle));
      angle += angleIncrement;
    }
    endShape();
  }

  // If it is not a cone, draw the circular bottom cap
  if (bottomRadius != 0) {
    angle = 0;
    beginShape(TRIANGLE_FAN);

    // Center point
    vertex(0, tall, 0);
    for (int i = 0; i < sides + 1; i++) {
      vertex(bottomRadius * cos(angle), tall, bottomRadius * sin(angle));
      angle += angleIncrement;
    }
    endShape();
  }
}

class RGB {
  public int r, g, b;
  
  RGB(int r, int g, int b) {
    this.r = r;
    this.g = g;
    this.b = b;
  }
};
