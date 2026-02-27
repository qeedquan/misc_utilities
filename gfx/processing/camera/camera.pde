Camera cam;
PVector ang;

void setup() {
  size(800, 800, P3D);
  cam = new Camera();
  ang = new PVector(0, 0, 0);
  lights();
}

void draw() {
  background(150, 150, 150);
  textSize(16);
  
  camera(width/2, height/2, height/(2*tan(radians(30))), width/2, height/2, 0, 0, 1, 0);
  text("Eye " + cam.eye, 16, 16);
  text("Center" + cam.center, 16, 32);
  
  cam.apply();
  
  float size = 10;
  pushMatrix();  
  rotateX(radians(ang.x));
  rotateY(radians(ang.y));
  rotateZ(radians(ang.z));
  
  translate(0, 0, 0);
  box(size);
  
  translate(0, -2*size, 0);
  sphere(size);
  translate(0, 4*size, 0);
  sphere(size);
  
  translate(-2*size, 0, 0);
  triangle(0, 0, 0, size, size, size);
  translate(4*size, 0, 0);
  triangle(0, 0, 0, size, size, size);
  
  translate(-2*size, -6*size, 0);
  ellipse(0, 0, size, 1.5*size);
  translate(size, 8*size, 0);
  square(0, 0, size);
  translate(size, -4*size, 0);
  square(0, 0, size);
  
  popMatrix();
  
  ang.x = (ang.x + 1) % 360;
  ang.y = (ang.y + 1) % 360;
  
  keyPressed();
  mouseMoved();
}

void keyPressed() {
  if (!keyPressed) {
    return;
  }
  
  float step = 1;
  switch (key) {
  case 'w':
    cam.moveForward(step);
    break;
  case 's':
    cam.moveForward(-step);
    break;
  case 'a':
    cam.moveRight(-step);
    break;
  case 'd':
    cam.moveRight(step);
    break;
  }
}

void mouseMoved() {
  if (!mousePressed)
    return;
  
  float sens = 0.05;
  cam.rotateRight(radians(mouseX-pmouseX)*sens);
  cam.rotateUp(radians(mouseY-pmouseY)*sens);
}

void mouseWheel(MouseEvent ev) {
  float step = 10;
  cam.moveForward(step*ev.getCount());
}

PVector rotateVector(PVector vector, PVector axis, float angle) {
  Quat qr = new Quat(axis, angle);
  Quat qi = qr.conjugate();
  Quat qp = new Quat(vector);
  return qi.mult(qp).mult(qr).vector();
}

class Camera {
  PVector eye;
  PVector center;
  PVector up;
  
  // same concept as lookat() function in other graphics libs
  // one thing to note the center part acts like a reference for the eye to
  // create a normalized direction vector for the forward direction
  // this means that the center point can actually be any point scaled by an arbitary constant k
  // k*center as long (k*center)-eye does not change direction
  // example:
  // a center point of (0, 0, 0), (0, 0, 40), (0, 0, -100) with eye (0, 0, 50)
  // gives the same camera matrix
  Camera() {
    center = new PVector(0, 0, 0);
    eye = new PVector(center.x, center.y, 200);
    up = new PVector(0, 1, 0);
  }
  
  PVector getNormalizedViewVector() {
    return PVector.sub(center, eye).normalize();
  }
  
  // move forward/back (in the direction of the forward vector)
  void moveForward(float distance) {
    PVector forward = getNormalizedViewVector();
    forward.mult(distance);
    eye.add(forward);
    center.add(forward);
  }
  
  // move left/right (in the direction of the right vector)
  // we can calculate the right vector by crossing the forward with the up vector
  void moveRight(float distance) {
    PVector forward = getNormalizedViewVector();
    PVector right = forward.cross(up);
    right.mult(distance);
    eye.add(right);
    center.add(right);
  }
  
  void rotateRight(float angle) {
    PVector forward = getNormalizedViewVector();
    PVector rot = rotateVector(forward, up, angle);
    center = PVector.add(eye, rot);
  }
  
  void rotateUp(float angle) {
    PVector forward = getNormalizedViewVector();
    PVector right = forward.cross(up);
    PVector rot = rotateVector(forward, right, angle);
    center = PVector.add(eye, rot);
  }
  
  void apply() {
    camera(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z);
  }
};

class Quat {
  float x, y, z, w;
  
  Quat(float x, float y, float z, float w) {
    set(x, y, z, w);
  }
  
  Quat(PVector v) {
    set(v);
  }
  
  Quat(PVector axis, float angle) {
    float c = cos(angle);
    float s = sin(angle);
    PVector u = axis.normalize();
    x = s*u.x;
    y = s*u.y;
    z = s*u.z;
    w = c;
  }
  
  void set(PVector v) {
    set(v.x, v.y, v.z, 0);
  }
  
  void set(float x, float y, float z, float w) {
    this.x = x;
    this.y = y;
    this.z = z;
    this.w = w;
  }
  
  Quat mult(Quat q) {
    float nw = w*q.w - x*q.x - y*q.y - z*q.z;
    float nx = x*q.w + w*q.x + y*q.z - z*q.y;
    float ny = y*q.w + w*q.y + z*q.x - x*q.z;
    float nz = z*q.w + w*q.z + x*q.y - y*q.x;
    return new Quat(nx, ny, nz, nw);
  }
  
  Quat conjugate() {
    return new Quat(-x, -y, -z, w);
  }
  
  PVector vector() {
    return new PVector(x, y, z);
  }
}
