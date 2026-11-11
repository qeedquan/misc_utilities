/*

https://antongerdelan.net/opengl/raycasting.html
https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms
https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection

*/

PShader shader;
Camera camera;
int fontsz;
Ray pickray;
Hit hitpoint;
ArrayList<Shape> shapes;

void setup() {
  size(1024, 768, P3D);
  colorMode(RGB, 1);
  
  fontsz = 16;
  textSize(fontsz);
  
  shader = loadShader("mouse_picking_3d_frag.glsl", "mouse_picking_3d_vert.glsl");
  camera = new Camera();

  shapes = new ArrayList<Shape>();
  shapes.add(new Sphere(new PVector(-3, 0, 0), 1.0));
  shapes.add(new Cube(new PVector(3, 0, 0), 2));
  shapes.add(new Triangle(new PVector(0, -3, 0), 2));
  shapes.add(new Sphere(new PVector(3, 3, 0), 1.5));
  shapes.add(new Cube(new PVector(-3, 3, 0), 1.5));
  shapes.add(new Plane(new PVector(-7, 0, 0), 1.5));
  shapes.add(new Plane(new PVector(7, 0, 0), 1.5));
}

void draw() {
  pickray = getPickRay();

  background(.2, .3, .5);
  strokeWeight(2);
  drawShapes();
  drawAxis();
  drawHUD();
  
  if (keyPressed) {
    keyPressed();
  }
}

void reset() {
  camera.reset();
}

void keyPressed() {
  float speed = 0.1;
  switch (key) {
  case 'w':
    camera.moveVertical(-speed);
    break;
  case 's':
    camera.moveVertical(speed);
    break;
  case 'a':
    camera.moveHorizontal(-speed);
    break;
  case 'd':
    camera.moveHorizontal(speed);
    break;
  case ' ':
    reset();
    break;
  }
}  

void mouseWheel(MouseEvent ev) {
  camera.zoom(0.1*ev.getCount());
}

void mouseDragged() {
  if (mouseButton == LEFT) {
    float mx = (mouseX - pmouseX) * 1e-2;
    float my = (mouseY - pmouseY) * 1e-2;
    camera.rotatePolar(mx);
    camera.rotateAzimuth(my);
  }
}

void drawShapes() {
  push();
  hint(ENABLE_DEPTH_TEST);
  shader(shader);
  camera.applyView();
  camera.applyPerspective();

  hitpoint = null;
  Hit ht = new Hit();
  for (Shape s : shapes) {
    s.colour = color(.3, .4, .5);
    if (s.intersect(pickray, ht)) {
      s.colour = color(.3, .5, .7);
      hitpoint = ht;
    }
    s.draw();
  }
  
  PVector mp = pickray.at(1);
  stroke(.5, .6, .7);
  strokeWeight(10);
  point(mp.x, mp.y, mp.z);
  pop();
}

void drawAxis() {
  PVector[] pts = new PVector[]{
    new PVector(1, 0, 0),
    new PVector(0, 1, 0),
    new PVector(0, 0, 1),
  };
  PVector view = camera.getView();
  
  PMatrix3D rotate = lookat(new PVector(), view, camera.up);
  PMatrix3D scale = new PMatrix3D();
  PMatrix3D translate = new PMatrix3D();
  PMatrix3D matrix = new PMatrix3D();
  scale.scale(0.1);
  translate.translate(4, -3, 0);
  matrix.reset();
  matrix.apply(scale);
  matrix.apply(translate);
  matrix.apply(rotate);

  push();
  hint(ENABLE_DEPTH_TEST);
  camera(0, 0, 1, 0, 0, 0, 0, 1, 0);
  camera.applyPerspective();

  beginShape(LINES);
  for (PVector p : pts) {
    PVector o = new PVector();
    PVector q = p.copy();
    q.mult(-1);
    
    stroke(p.x, p.y, p.z);

    matrix.mult(o, o);
    matrix.mult(p, p);
    matrix.mult(q, q);

    vertex(o.x, o.y, o.z);
    vertex(p.x, p.y, p.z);

    stroke(0.6, 0.7, 0.8);
    vertex(o.x, o.y, o.z);
    vertex(q.x, q.y, q.z);
  }
  
  endShape();
  pop();
}

void drawHUD() {
  resetShader();
  hint(DISABLE_DEPTH_TEST);
  camera();
  perspective();
  fill(1);
  PVector eye = camera.getEye();
  text(String.format("Orbit Camera"), 32, 32 + 0*fontsz);
  text(String.format("Center [%.3f %.3f %.3f]", camera.center.x, camera.center.y, camera.center.z), 32, 32 + 1*fontsz);
  text(String.format("Eye [%.3f %.3f %.3f]", eye.x, eye.y, eye.z), 32, 32 + 2*fontsz);
  text(String.format("Radius %.3f", camera.radius), 32, 32 + 3*fontsz);
  text(String.format("Polar %.3f", degrees(camera.polar)), 32, 32 + 4*fontsz);
  text(String.format("Azimuth %.3f", degrees(camera.azimuth)), 32, 32 + 5*fontsz);
  text(String.format("Mouse (World Space)"), 32, 32 + 6*fontsz);
  text(String.format("  Origin [%.3f %.3f %.3f]", pickray.origin.x, pickray.origin.y, pickray.origin.z), 32, 32 + 7*fontsz);
  text(String.format("  Direction [%.3f %.3f %.3f]", pickray.direction.x, pickray.direction.y, pickray.direction.z), 32, 32 + 8*fontsz);
  if (hitpoint != null)
    text(String.format("  Hit %.3f [%.3f %.3f %.3f]", hitpoint.t, hitpoint.p.x, hitpoint.p.y, hitpoint.p.z), 32, 32 + 9*fontsz);
}

/*

We want all objects expressed in the same space to find intersections.
Since all of the objects are expressed as world space in the code, we want to find the mouse location
in world space. The way to do this is to reverse the matrix pipeline.

When we move the mouse, the coordinates we get are in window coordinates, we can think of these coordinates
as a scaled version of the normalized device coordinate (ndc).

We want the following to transform the mouse coordinates from the window space back to world space, so we need
to unproject these series of transforms that the traditional matrix pipeline entails.
window -> ndc -> clip -> world

window -> ndc
scale [0:w], [0:h] to [-1, 1] for a point (nx, ny)

ndc -> clip
Since ndc and clip space are same range for the points, we can just copy directly, but we need to define what z is
We define z to be +-1 since we basically want the mouse position always in front of the eye (the sign is based on whatever the coordinate convention is)
So we have the following coordinate in clip space:
clip_point = (nx, ny, +-1, 1)
eye_point = inverse(projection_matrix) * clip

clip -> world
wor = inverse(view_matrix) * vec4(eye.xy, +-1, 0)
It is important that we treat this as a vector here because the view matrix has a translation component to it but we just want the orientation.

Now we can make the ray as follows:
ray_origin = camera_eye
ray_direction = wor

The origin is always at the camera eye because we want to always look things through the frame of reference of the eye.
The direction is to the point of the mouse location.

When we evaluate it as follows:
mouse_pos = origin + direction
(basically it's a 3D point that is equal to the 2D mouse position after everything is transformed through the matrix pipeline)

*/

Ray getPickRay() {
  PMatrix3D proj = persp(camera.fovy, camera.aspect, camera.znear, camera.zfar);
  PMatrix3D view = lookat(camera.getEye(), camera.center, camera.up);
  proj.invert();
  view.invert(); 
  
  float x = (2.0 * mouseX) / width - 1.0;
  float y =  1.0 - (2.0 * mouseY) / height;

  PVector ndc = new PVector(x, y, 1);
  PVector clip = new PVector(ndc.x, ndc.y, -1);
  PVector eye = proj.mult(clip, null);
  eye.z = -1;
  
  // have to do this because there is no way to specify a vector has w=0
  // it is always set to w=1 for matrix multiplication
  PVector wor = new PVector();
  wor.x = view.multX(eye.x, eye.y, eye.z, 0);
  wor.y = view.multY(eye.x, eye.y, eye.z, 0);
  wor.z = view.multZ(eye.x, eye.y, eye.z, 0);
  wor.normalize();
  
  return new Ray(camera.getEye(), wor);
}

float clamp(float x, float a, float b) {
  return min(max(x, a), b);
}

PMatrix3D lookat(PVector eye, PVector center, PVector up) {
  PVector u = up.copy();
  PVector f = PVector.sub(center, eye);
  f.normalize();
  u.normalize();
  PVector s = f.cross(u);
  s.normalize();
  u = s.cross(f);
  
  PMatrix3D m = new PMatrix3D();
  PMatrix3D t = new PMatrix3D();
  t.translate(-eye.x, -eye.y, -eye.z);
  m.set(s.x, s.y, s.z, 0,
        u.x, u.y, u.z, 0,
       -f.x, -f.y, -f.z, 0,
       0, 0, 0, 1);
  m.apply(t);
  return m;
}

PMatrix3D persp(float fovy, float aspect, float znear, float zfar) {
  float f = 1/tan(fovy/2);
  float sz = (zfar+znear)/(znear-zfar);
  float tz = (2*zfar*znear)/(znear-zfar);

  PMatrix3D m = new PMatrix3D();
  m.set(f/aspect, 0, 0, 0,
        0, -f, 0, 0,
        0, 0, sz, tz,
        0, 0, -1, 0);
  
  return m;
}

void dumpMatrix(PMatrix3D m) {
  println(String.format("%.3f %.3f %.3f %.3f", m.m00, m.m01, m.m02, m.m03));
  println(String.format("%.3f %.3f %.3f %.3f", m.m10, m.m11, m.m12, m.m13));
  println(String.format("%.3f %.3f %.3f %.3f", m.m20, m.m21, m.m22, m.m23));
  println(String.format("%.3f %.3f %.3f %.3f", m.m30, m.m31, m.m32, m.m33));
  println();
}

class Ray {
  PVector origin;
  PVector direction;
  
  Ray(PVector origin, PVector direction) {
    this.origin = origin;
    this.direction = direction;
  }
  
  PVector at(float t) {
    return PVector.add(origin, PVector.mult(direction, t));
  }
  
  void draw(float t) {
    PVector o = origin;
    PVector p = at(t);
    
    push();
    strokeWeight(1);
    stroke(.4, .5, .6);
    strokeWeight(5);
    beginShape(LINES);
    vertex(o.x, o.y, o.z);
    vertex(p.x, p.y, p.z);
    endShape();
    pop();
  }
}

class Hit {
  float t;
  PVector p;
}

abstract class Shape {
  PShape shape;
  PVector center;
  float size;
  color colour;
  
  Shape(PVector center, float size) {
    this.center = center;
    this.size = size;
    this.colour = color(.3, .4, .5);
  }
  
  void draw() {
    push();
    translate(center.x, center.y, center.z);
    rotateY(millis()*1e-3);
    shape.setStroke(true);
    shape.setFill(colour);
    shape(shape);
    pop();
  }
  
  abstract boolean intersect(Ray r, Hit h);
}

class Cube extends Shape {
  PVector lb;
  PVector rt;
  
  Cube(PVector center, float size) {
    super(center, size);
    shape = createShape(BOX, size);
    lb = new PVector(center.x - size/2, center.y - size/2, center.z - size/2);
    rt = new PVector(center.x + size/2, center.y + size/2, center.z + size/2);
  }
  
  boolean intersect(Ray r, Hit h) {
    PVector dirfrac = new PVector();
    
    // r.dir is unit direction vector of ray
    dirfrac.x = 1.0f / r.direction.x;
    dirfrac.y = 1.0f / r.direction.y;
    dirfrac.z = 1.0f / r.direction.z;
    // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
    // r.org is origin of ray
    float t1 = (lb.x - r.origin.x)*dirfrac.x;
    float t2 = (rt.x - r.origin.x)*dirfrac.x;
    float t3 = (lb.y - r.origin.y)*dirfrac.y;
    float t4 = (rt.y - r.origin.y)*dirfrac.y;
    float t5 = (lb.z - r.origin.z)*dirfrac.z;
    float t6 = (rt.z - r.origin.z)*dirfrac.z;

    float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
    float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

    // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
    if (tmax < 0)
      return false;

    // if tmin > tmax, ray doesn't intersect AABB
    if (tmin > tmax)
      return false;
    
    h.t = tmin;
    h.p = r.at(h.t);
    return true;
  }
}

class Sphere extends Shape {  
  Sphere(PVector center, float size) {
    super(center, size);
    shape = createShape(SPHERE, size);
  }
  
  boolean intersect(Ray r, Hit hit) {
    PVector oc = PVector.sub(r.origin, center);
    float a = PVector.dot(r.direction, r.direction);
    float hb = PVector.dot(oc, r.direction);
    float c = PVector.dot(oc, oc) - size*size;

    float dm = hb*hb - a*c;
    if (dm < 0)
      return false;
    
    float t = (-hb + sqrt(dm)) / a;

    hit.t = t;
    hit.p = r.at(t);
    return true;
  }
}

class Triangle extends Shape {
  PVector[] points;

  Triangle(PVector center, float size) {
    super(center, size);
    
    points = new PVector[]{
      new PVector(0, -size/2),
      new PVector(-size/2, size/2),
      new PVector(size/2, size/2),
    };
    shape = createShape(TRIANGLE, points[0].x, points[0].y, points[1].x, points[1].y, points[2].x, points[2].y);
    for (PVector p : points)
      p.add(center);
  }
  
  boolean intersect(Ray r, Hit hp) {
    PVector v0 = points[0];
    PVector v1 = points[1];
    PVector v2 = points[2];
    PVector e1 = PVector.sub(v1, v0);
    PVector e2 = PVector.sub(v2, v0);
    PVector h = r.direction.cross(e2);
    float a = e1.dot(h);

    // This ray is parallel to this triangle.
    if (a > -EPSILON && a < EPSILON)
      return false;
    float f = 1.0 / a;
    PVector s = PVector.sub(r.origin, v0);
    float u = f * (s.dot(h));
    if (u < 0.0 || u > 1.0)
      return false;
    PVector q = s.cross(e1);
    float v = f * r.direction.dot(q);
    if (v < 0.0 || u + v > 1.0)
      return false;
    
    // At this stage we can compute t to find out where the intersection point is on the line.
    hp.t = f * e2.dot(q);
    // ray intersection
    if (hp.t > EPSILON)  {
      hp.p = r.at(hp.t);
      return true;
    }
    
    // This means that there is a line intersection but not a ray intersection.
    return false;
  }
}

class Plane extends Shape {
  PVector normal;
  PVector[] points;
  
  Plane(PVector center, float size) {
    super(center, size);
    
    points = new PVector[]{
      new PVector(-size/2, -size/2),
      new PVector(size/2, -size/2),
      new PVector(size/2, size/2),
      new PVector(-size/2, size/2),
    };
    shape = createShape(QUAD, points[0].x, points[0].y, points[1].x, points[1].y, points[2].x, points[2].y, points[3].x, points[3].y);
    for (PVector p : points)
      p.add(center);
      
    PVector X = PVector.sub(points[1], points[0]);
    PVector Y = PVector.sub(points[2], points[0]);
    X.normalize();
    Y.normalize();

    normal = X.cross(Y);
    normal.normalize();
  }
  
  boolean intersect(Ray r, Hit h) {
    float d = PVector.dot(normal, r.direction);
    if (abs(d) > EPSILON) {
      PVector p = PVector.sub(center, r.origin);
      h.t = PVector.dot(p, normal) / d;
      h.p = r.at(h.t);
      if (h.t < 0)
        return false;

      // if we want to hit a bounded plane we need to see if the quad generated by the plane is out of bounds
      // remove this if we want to hit an infinite plane
      // this assumes the ray is axis aligned with the plane, otherwise we have to generate a local frame to align it
      if (h.p.x < points[0].x || h.p.x > points[2].x)
        return false;
      if (h.p.y < points[0].y || h.p.y > points[2].y)
        return false;

      return true;
    }
    return false;
  }
}

class Camera {
  PVector center;
  PVector up;

  float radius;
  float min_radius;
  float polar;
  float azimuth;
  
  float fovy;
  float aspect;
  float znear;
  float zfar;
  
  Camera() {
    reset();
  }
  
  void reset() {
    center = new PVector(0, 0, 0);
    up = new PVector(0, 1, 0);
    radius = 15;
    min_radius = 1;
    polar = PI/2;
    azimuth = 0;

    fovy = PI/4;
    aspect = width*1.0/height;
    znear = 0.1;
    zfar = 1000;
  }
  
  void applyView() {
    PVector eye = getEye();
    camera(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z);
  }
  
  void applyPerspective() {
    perspective(fovy, aspect, znear, zfar);
  }
  
  PVector getEye() {
    float sp = sin(polar);
    float cp = cos(polar);
    float sa = sin(azimuth);
    float ca = cos(azimuth);
    
    float t = radius * ca;
    float y = center.y + t*sa;
    float x = center.x + t*cp;
    float z = center.z + t*sp;
    return new PVector(x, y, z);
  }
  
  PVector getView() {
    PVector view = PVector.sub(center, getEye());
    view.normalize();
    return view;
  }
  
  void moveHorizontal(float speed) {
    PVector view = getView();   
    PVector strafe = view.cross(up);
    strafe.normalize();
    strafe.mult(speed);
    center.add(strafe);
  }
  
  void moveVertical(float speed) {
    PVector u = up.copy();
    u.mult(speed);
    center = PVector.add(center, u);
  }
  
  void rotatePolar(float r) {
    polar = (polar + r) % TWO_PI;
    if (polar < 0)
      polar += TWO_PI;
  }
  
  void rotateAzimuth(float r) {
    float cap = PI/2 - 1e-3;
    azimuth = clamp(azimuth+r, -cap, cap);
  }
  
  void zoom(float factor) {
    radius *= exp(factor);
    radius = max(radius, min_radius);
  }
}
