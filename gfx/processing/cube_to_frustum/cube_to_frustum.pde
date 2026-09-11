/*

http://www.songho.ca/opengl/gl_projectionmatrix.html
https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/projection-matrices-what-you-need-to-know-first
https://en.wikipedia.org/wiki/Viewing_frustum

*/

PShader shader;

Camera camera;
PVector[] l_cube;
PVector[] t_cube;
float cubeSize;

Frustum frustum;
int select;
boolean wireframe;
boolean transform;

PGraphics textLayer;

void setup() {
  size(1024, 768, P3D);
  colorMode(RGB, 1);
  shader = loadShader("cube_to_frustum_frag.glsl", "cube_to_frustum_vert.glsl");
  
  camera = new Camera();
  camera.setDistance(10);
  resetCamera();
  reshapeWindow(width, height);
  
  cubeSize = 1;
  l_cube = genCube(LINES, cubeSize);
  t_cube = genCube(TRIANGLES, cubeSize);
  wireframe = true;
  transform = true;
  
  frustum = new Frustum(-1, 1, -1, 1, -1, 1);
  select = 0;

  textLayer = createGraphics(width, height, JAVA2D);
  textLayer.smooth(4);
}

void reshapeWindow(int width, int height) {
  camera.setViewport(0, 0, width, height);
  camera.setPerspective(radians(50));
}

void resetCamera() {
  PMatrix3D identity = new PMatrix3D();
  identity.reset();
  camera.setRotation(identity);
  camera.setCenter(new PVector(0, 0, 0));
}

void draw() {
  hint(ENABLE_DEPTH_TEST);
  
  background(0.3, 0.3, 0.3);
  shader(shader);
  shader.set("u_resolution", new PVector(width, height));
  shader.set("u_time", millis()/1e3);

  resetMatrix();
  camera.applyModelView();
  camera.applyPerspective();

  frustum.gen();
  int       shape = LINES;
  PVector[] cube  = l_cube;
  if (!wireframe) {
    shape = TRIANGLES;
    cube = t_cube;
  }
    
  beginShape(shape);
  stroke(0, 0, 0);
  for (int i = 0; i < cube.length; i++) {
    PVector p = cube[i];
    if (transform) {
      float w = frustum.matrix.multW(p.x, p.y, p.z);
      p = frustum.matrix.mult(p, null);
      p.mult(1/w);
    }
    vertex(p.x, p.y, p.z);
  }
  endShape();
  
  if (mousePressed) {
    if (camera.button == 0)
      camera.mouseClick(mouseButton, mouseX, mouseY);
    
    pushMatrix();
    translate(camera.center.x, camera.center.y, camera.center.z);
    drawAxis();
    popMatrix();
  }

  resetShader();
  drawHUD();
}

void drawHUD() {
  hint(DISABLE_DEPTH_TEST);
  String tab[] = {"left", "right", "bottom", "top", "near", "far"};
  String str = "Frustum:\n";
  str += String.format("[%.6f %.6f]\n", frustum.left, frustum.right);
  str += String.format("[%.6f %.6f]\n", frustum.bottom, frustum.top);
  str += String.format("[%.6f %.6f]\n", frustum.near, frustum.far);
  if (select != 0) {
    str += String.format("Selected: %s\n", tab[select-1]);
  }

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
  translate(-width/2.8, -height/2.8);
  image(textLayer, 0, 0);
  pop();
}

void drawAxis() {
  push();
  scale(5, 5, 5);
  beginShape(LINES);

  stroke(1, 0, 0);
  vertex(0, 0, 0);
  vertex(1, 0, 0);
  
  stroke(0, 1, 0);
  vertex(0, 0, 0);
  vertex(0, 1, 0);
  
  stroke(0, 0, 1);
  vertex(0, 0, 0);
  vertex(0, 0, 1);
  
  stroke(0.5, 0.5, 0.5);
  vertex(0, 0, 0);
  vertex(-1, 0, 0);
  
  vertex(0, 0, 0);
  vertex(0, -1, 0);

  vertex(0, 0, 0);
  vertex(0, 0, -1);
  
  endShape();
  pop();
}

void mouseDragged() {
  camera.mouseDrag(mouseX, mouseY);
}

void mouseReleased() {
  camera.mouseRelease();
}

void keyPressed() {
  if (key == ' ')
    resetCamera();
  else if ('1' <= key && key <= '6')
    select = key - '0';
  else if (key == 'w')
    wireframe = !wireframe;
  else if (key == ENTER)
    transform = !transform;

  float step = 0;
  if (keyCode == LEFT)
    step = -1e-2;
  else if (keyCode == RIGHT)
    step = 1e-2;

  switch (select) {
  case 1:
    frustum.left += step;
    break;
  case 2:
    frustum.right += step;
    break;
  case 3:
    frustum.bottom += step;
    break;
  case 4:
    frustum.top += step;
    break;
  case 5:
    frustum.near += step;
    break;
  case 6:
    frustum.far += step;
    break;
  }
}

PVector[] genCube(int type, float s) {
  PVector[] P = new PVector[]{
    // Front face
    new PVector(-1.0, -1.0,  1.0),
    new PVector(1.0, -1.0,  1.0),
    new PVector(1.0,  1.0,  1.0),
    new PVector(-1.0,  1.0,  1.0),

    // Back face
    new PVector(-1.0, -1.0, -1.0),
    new PVector(-1.0,  1.0, -1.0),
    new PVector(1.0,  1.0, -1.0),
    new PVector(1.0, -1.0, -1.0),

    // Top face
    new PVector(-1.0,  1.0, -1.0),
    new PVector(-1.0,  1.0,  1.0),
    new PVector(1.0,  1.0,  1.0),
    new PVector(1.0,  1.0, -1.0),

    // Bottom face
    new PVector(-1.0, -1.0, -1.0),
    new PVector(1.0, -1.0, -1.0),
    new PVector(1.0, -1.0,  1.0),
    new PVector(-1.0, -1.0,  1.0),

    // Right face
    new PVector(1.0, -1.0, -1.0),
    new PVector(1.0,  1.0, -1.0),
    new PVector(1.0,  1.0,  1.0),
    new PVector(1.0, -1.0,  1.0),

    // Left face
    new PVector(-1.0, -1.0, -1.0),
    new PVector(-1.0, -1.0,  1.0),
    new PVector(-1.0,  1.0,  1.0),
    new PVector(-1.0,  1.0, -1.0),
  };
  
  PVector[] v = null;
  int       n = 0;
  switch (type) {
  case LINES:
    v = new PVector[2*P.length];
    for (int i = 0; i < P.length; i += 4) {
      for (int j = 0; j < 4; j++) {
        v[n++] = P[i+j];
        v[n++] = P[i+(j+1)%4];
      }
    }
    break;
  
  case TRIANGLES:
    v = new PVector[6*P.length/4];
    for (int i = 0; i < P.length; i += 4) {
      for (int j = 0; j < 3; j++) {
        v[n++] = P[i+j];
      }
      for (int j = 2; j < 5; j++) {
        v[n++] = P[i+(j%4)];
      }
    }
    break;

  default:
    assert(false);
    break;
  }  
  
  for (int i = 0; i < v.length; i++)
    v[i].mult(s);
  
  return v;
}

class Rect {
  float x, y, w, h;
  
  void set(float x, float y, float w, float h) {
    this.x = x;
    this.y = y;
    this.w = w;
    this.h = h;
  }
}

/*

A viewing frustum matrix generated by glFrustum transforms a cube into two frustum whose apex meets at the center of the cube.
The frustum matrix simulates depth by projecting things far away as smaller (they meet at a vanishing point); the vanishing
point is the center of the cube in this case. The frustum matrix also preserves the size of the cube in the sense that it does
not scale the sides so the initial cube will be a bounding box of the frustum if we use the coordinates of cube for the frustum sides.

Many diagrams show the picture as the eye looking at the near plane and then projecting outwards to see the far plane,
but the way this transform works is that you start looking at the near plane with a large plane and then as you go farther out things become
smaller as you go to the far plane; in this case the eye is actually the vanishing point.
Either way of thinking about this gives the same frustum shape, so it works out the same way.

*/

class Frustum {
  float left, right;
  float bottom, top;
  float near, far;
  PMatrix3D matrix;
  
  Frustum(float l, float r, float b, float t, float n, float f) {
    left = l;
    right = r;
    bottom = b;
    top = t;
    near = n;
    far = f;
    matrix = new PMatrix3D();
  }
  
  void gen() {
    float X = 2*near/(right-left);
    float Y = 2*near/(top-bottom);
    float A = (right+left)/(right-left);
    float B = (top+bottom)/(top-bottom);
    float C = -(far+near)/(far-near);
    float D = -(2*far*near)/(far-near);
    matrix.set(X, 0, A, 0,
               0, Y, B, 0,
               0, 0, C, D,
               0, 0, -1, 0);
  }
}


// processing is left-handed (even though opengl is right-handed)
// so this camera works with left handed coordinate system
// (x goes to the right, y goes down, z goes outwards)
class Camera {
  // states
  PVector click;
  int button;
  
  // rotation
  PMatrix3D rotation0;
  PMatrix3D rotation;
  
  // translation

  // stores fovy and aspect ratio for
  // standard perspective matrix
  PVector perspective;

  // stores window viewport
  Rect viewport;
  
  PVector center0;
  PVector center;
  
  // zoom
  
  // distance is from z axis
  float distance0;
  float distance;
  
  Camera() {
    click = new PVector();
    
    rotation0 = new PMatrix3D();
    rotation = new PMatrix3D();
    
    perspective = new PVector();
    viewport = new Rect();
    center0 = new PVector();
    center = new PVector();
  }
  
  void setPerspective(float fovy) {
    perspective.x = fovy;
  }
  
  void setCenter(PVector center) {
    this.center0 = center.copy();
    this.center = center.copy();
  }
  
  void setRotation(PMatrix3D rotation) {
    this.rotation0 = rotation.get();
    this.rotation = rotation.get();
  }
  
  void setDistance(float distance) {
    this.distance0 = distance;
    this.distance = distance;
  }

  void setViewport(float x, float y, float w, float h) {
    viewport.set(x, y, w, h);
    perspective.y = w / h;
  }
  
  void mouseRelease() {
    rotation0 = rotation.get();
    center0 = center.copy();
    distance0 = distance;
    button = 0;
  }
  
  void mouseClick(int button, float x, float y) {
    click.set(x, y);
    this.button = button;
    
    switch (button) {
    case LEFT:
      rotation = rotation0.get();
      break;
    case CENTER:
      center = center0.copy();
      break;
    case RIGHT:
      distance = distance0;
      break;
    }
  }
  
  void mouseDrag(float x, float y) {
    switch (button) {
    case LEFT:
      arcBallRotation(x, y);
      break;
    case CENTER:
      planeTranslation(x, y);
      break;
    case RIGHT:
      distanceZoom(x, y);
      break;
    }
  }
  
  // standard perspective matrix
  void applyPerspective() {
    perspective(this.perspective.x, this.perspective.y, 1, 1000);
  }
  
  // the object is assumed to be placed at the center of the screen
  // the camera is placed in a forward distance to it looking at the object
  // we rotate the object and not the camera
  // this gives a perspective view of always looking at the object when we move
  // this is different from a flying camera where we move in the frame of the camera
  // giving a first person view, this camera is an object based camera in the sense
  // if we try to move around it looks like the object is moved and not the camera
  void applyModelView() {
    camera(0, 0, distance, 0, 0, 0, 0, 1, 0);
    applyMatrix(rotation.m00, rotation.m01, rotation.m02, rotation.m03,
                rotation.m10, rotation.m11, rotation.m12, rotation.m13,
                rotation.m20, rotation.m21, rotation.m22, rotation.m23,
                rotation.m30, rotation.m31, rotation.m32, rotation.m33);
    translate(-center.x, -center.y, -center.z);
  }
  
  void arcBallRotation(float x, float y) {
    float sx = click.x - viewport.w/2;
    float sy = click.y - viewport.h/2;
    float ex = x - viewport.w/2;
    float ey = y - viewport.h/2;
    
    float scale = 1 / min(viewport.w, viewport.h);
    sx *= scale;
    sy *= scale;
    ex *= scale;
    ey *= scale;
    
    float sl = sqrt(sx*sx + sy*sy);
    float el = sqrt(ex*ex + ey*ey);
    if (sl > 1) {
      sx /= sl;
      sy /= sl;
      sl = 1;
    }
    if (el > 1) {
      ex /= el;
      ey /= el;
      el = 1;
    }
    
    float sz = sqrt(1 - sl*sl);
    float ez = sqrt(1 - el*el);
    float d = sx*ex +sy*ey + sz*ez;
    if (d != 1) {
      PVector axis = new PVector(sy*ez - ey*sz, sz*ex - ex*sx, sx*ey - ex*sy);
      axis.normalize();
      
      float angle = 2*acos(d);
      rotation.reset();
      rotation.rotate(angle, axis.x, axis.y, axis.z);
      rotation.apply(rotation0);
    } else
      rotation = rotation0.get();
  }
  
  void planeTranslation(float x, float y) {
    // figure where we are relative to the viewport origin
    float sx = click.x - viewport.x;
    float sy = click.y - viewport.y;
    float cx = x - viewport.x;
    float cy = y - viewport.y;
    
    // distance to the near plane
    float d = viewport.w * 0.5/tan(perspective.x/2);
    
    // remap where we are to the center point
    float su = sy - viewport.h/2;
    float cu = cy - viewport.h/2;
    float sr = sx - viewport.w/2;
    float cr = cx - viewport.w/2;
    
    // move in the xy direction depending on how much we moved the mouse by
    PVector move = new PVector(cr-sr, cu-su);
    PVector dirx = new PVector(rotation.m00, rotation.m01, rotation.m02);
    PVector diry = new PVector(rotation.m10, rotation.m11, rotation.m12);
    move.mult(-distance/d);
    dirx.mult(move.x);
    diry.mult(move.y);
    center = PVector.add(PVector.add(center0, dirx), diry);
  }
  
  // zooming in and out depends on how far the mouse is dragged in the y direction
  // click represents the location when the mouse click first happened and y is where
  // we dragged the mouse to, since both of the location are smaller than the viewport
  // we end up with a delta y of [-1, 1] range which we pass to the exponential function
  // this is called exponential zooming, where we want to move based on how small or large
  // we are, if we are very far away we want each movement to move alot more than if we were smaller
  // so we cannot use an additive factor but a multiplicative factor
  void distanceZoom(float x, float y) {
    // map where we are relative to the viewport origin
    float sy = click.y - viewport.y;
    float cy = y - viewport.y;
    
    // the delta is how far we moved normalized by viewport height, giving [-1, 1]
    float dy = (cy - sy) / viewport.h;
    
    // exponential zoom, the distance is the forward distance for the eye
    distance = distance0 * exp(dy);
  }
}
