/*

https://en.wikipedia.org/wiki/CMYK_color_model
https://en.wikipedia.org/wiki/RGB_color_model
https://en.wikipedia.org/wiki/HSL_and_HSV
https://en.wikipedia.org/wiki/YCbCr
http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html

*/

PShader shader;

Camera camera;
PVector[] cube;
float cubeSize;

PGraphics textLayer;

int colorMatrix;
int colorSpace;

void setup() {
  size(1024, 768, P3D);
  colorMode(RGB, 1);
  shader = loadShader("color_cube_frag.glsl", "color_cube_vert.glsl");
  
  camera = new Camera();
  camera.setDistance(10);
  resetCamera();
  reshapeWindow(width, height);
  
  cubeSize = 1;
  cube = genCube(cubeSize);
  
  textLayer = createGraphics(width, height, JAVA2D);
  textLayer.smooth(4);
  
  colorMatrix = 0;
  colorSpace = 0;
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
  shader.set("colorMatrix", getColorMatrix(colorMatrixNames[colorMatrix]));
  shader.set("colorSpace", colorSpace);

  resetMatrix();
  camera.applyModelView();
  camera.applyPerspective();

  beginShape(QUADS);
  stroke(0, 0, 0);
  for (int i = 0; i < cube.length; i++) {
    PVector p = cube[i];
    PVector c = getCubeColor(p);
    vertex(p.x, p.y, p.z);
    fill(c.x, c.y, c.z);
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
  String str = "Color Matrix: " + colorMatrixNames[colorMatrix] + "\n";
  str += "Color Mode: " + colorSpaceNames[colorSpace];
  
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
  translate(-width/3, -height/3);
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
  else if (key == ENTER) {
    println("Camera Matrix");
    printMatrix();
  } else if (key == CODED) {
    if (keyCode == RIGHT)
      colorMatrix = (colorMatrix + 1) % colorMatrixNames.length;
    else if (keyCode == LEFT)
      colorMatrix = (colorMatrix == 0) ? (colorMatrixNames.length-1) : (colorMatrix-1);
    else if (keyCode == UP)
      colorSpace = (colorSpace + 1) % colorSpaceNames.length;
    else if (keyCode == DOWN)
      colorSpace = (colorSpace == 0) ? (colorSpaceNames.length-1) : (colorSpace-1);
  }
}

String colorMatrixNames[] = {
  "RGB", "Adobe RGB", "Apple RGB", "Best RGB", "Beta RGB",
  "Bruce RGB", "CIE RGB", "ColorMatch RGB", "Don RGB 4",
  "ECI RGB", "Ekta Space PS5", "NTSC RGB", "PAL/SECAM RGB",
  "ProPhoto RGB", "SMPTE-C RGB", "sRGB", "Wide Gamut RGB",
  "YcBCr",
};

String colorSpaceNames[] = {
  "RGB", "HSV", "HSL", "CMYK",
};

PMatrix3D getColorMatrix(String type) {
  PMatrix3D m = new PMatrix3D();
  m.reset();
  
  switch (type) {
  case "Adobe RGB":
    m.set(0.5767309, 0.1855540, 0.1881852, 0,
          0.2973769, 0.6273491, 0.0752741, 0,
          0.0270343, 0.0706872, 0.9911085, 0,
          0, 0, 0, 1);
    break;
    
  case "Apple RGB":
    m.set(0.4497288, 0.3162486, 0.1844926, 0,
          0.2446525, 0.6720283, 0.0833192, 0,
          0.0251848, 0.1411824, 0.9224628, 0,
          0, 0, 0, 1);
    break;
    
  case "Best RGB":
    m.set(0.6326696, 0.2045558, 0.1269946, 0,
          0.2284569, 0.7373523, 0.0341908, 0,
          0.0000000, 0.0095142, 0.8156958, 0,
          0, 0, 0, 1);
    break;
    
  case "Beta RGB":
    m.set(0.6712537, 0.1745834, 0.1183829, 0,
          0.3032726, 0.6637861, 0.0329413, 0,
          0.0000000, 0.0407010, 0.7845090, 0,
          0, 0, 0, 1);
    break;
    
  case "Bruce RGB":
    m.set(0.4674162, 0.2944512, 0.1886026, 0,
          0.2410115, 0.6835475, 0.0754410, 0,
          0.0219101, 0.0736128, 0.9933071, 0,
          0, 0, 0, 1);
    break;
  
  case "CIE RGB":
    m.set(0.4887180, 0.3106803, 0.2006017, 0,
          0.1762044, 0.8129847, 0.0108109, 0,
          0.0000000, 0.0102048, 0.9897952, 0,
          0, 0, 0, 1);
    break;
  
  case "ColorMatch RGB":
    m.set(0.5093439, 0.3209071, 0.1339691, 0,
          0.2748840, 0.6581315, 0.0669845, 0,
          0.0242545, 0.1087821, 0.6921735, 0,
          0, 0, 0, 1);
    break;
    
  case "Don RGB 4":
    m.set(0.6457711, 0.1933511, 0.1250978, 0,
          0.2783496, 0.6879702, 0.0336802, 0,
          0.0037113, 0.0179861, 0.8035125, 0,
          0, 0, 0, 1);
    break;
    
  case "ECI RGB":
    m.set(0.6502043, 0.1780774, 0.1359384, 0,
          0.3202499, 0.6020711, 0.0776791, 0,
          0.0000000, 0.0678390, 0.7573710, 0,
          0, 0, 0, 1);
    break;
  
  case "Ekta Space PS5":
    m.set(0.5938914, 0.2729801, 0.0973485, 0,
          0.2606286, 0.7349465, 0.0044249, 0,
          0.0000000, 0.0419969, 0.7832131, 0,
          0, 0, 0, 1);
    break;
  
  case "NTSC RGB":
    m.set(0.6068909, 0.1735011, 0.2003480, 0,
          0.2989164, 0.5865990, 0.1144845, 0,
          0.0000000, 0.0660957, 1.1162243, 0,
          0, 0, 0, 1);
    break;
  
  case "PAL/SECAM RGB":
    m.set(0.4306190, 0.3415419, 0.1783091, 0,
          0.2220379, 0.7066384, 0.0713236, 0,
          0.0201853, 0.1295504, 0.9390944, 0,
          0, 0, 0, 1);
    break;
  
  case "ProPhoto RGB":
    m.set(0.7976749, 0.1351917, 0.0313534, 0,
          0.2880402, 0.7118741, 0.0000857, 0,
          0.0000000, 0.0000000, 0.8252100, 0,
          0, 0, 0, 1);
    break;
  
  case "SMPTE-C RGB":
    m.set(0.3935891, 0.3652497, 0.1916313, 0,
          0.2124132, 0.7010437, 0.0865432, 0,
          0.0187423, 0.1119313, 0.9581563, 0,
          0, 0, 0, 1);
    break;
  
  case "sRGB":
    m.set(0.4124564, 0.3575761, 0.1804375, 0,
          0.2126729, 0.7151522, 0.0721750, 0,
          0.0193339, 0.1191920, 0.9503041, 0,
          0, 0, 0, 1);
    break;
  
  case "Wide Gamut RGB":
     m.set(0.7161046, 0.1009296, 0.1471858, 0,
           0.2581874, 0.7249378, 0.0168748, 0,
           0.0000000, 0.0517813, 0.7734287, 0,
           0, 0, 0, 1);
    break;
    
  case "YcBCr":
    m.set(1.0000, 1.0000, 1.0000, 0.0000,
          0.0000, -0.3441, 1.7720, 0.0000,
          1.4020, -0.7141, 0.0000, 0.0000,
         -0.7010, 0.5291, -0.8860, 1.0000);
    break;
  }
  
  return m;
}

PVector getCubeColor(PVector p) {
  PVector c = new PVector(p.x/cubeSize, p.y/cubeSize, p.z/cubeSize);
  c.x = map(c.x, -1, 1, 0, 1);
  c.y = map(c.y, -1, 1, 0, 1);
  c.z = map(c.z, -1, 1, 0, 1);
  return c;
}

PVector[] genCube(float s) {
  PVector[] p = new PVector[8];
  for (int i = 0; i < p.length; i++) {
    p[i] = new PVector(nzf(i&1), nzf(i&2), nzf(i&4));
    p[i].mult(s);
  }
  
  int[][] f = {
    {0, 4, 6, 2},
    {1, 3, 7, 5},
    {0, 1, 5, 4},
    {2, 6, 7, 3},
    {0, 2, 3, 1},
    {4, 5, 7, 6},
  };
  
  PVector[] v = new PVector[24];
  int n = 0;
  for (int i = 0; i < f.length; i++) {
    v[n++] = p[f[i][0]];
    v[n++] = p[f[i][1]];
    v[n++] = p[f[i][2]];
    v[n++] = p[f[i][3]];
  }
  return v;
}

float nzf(int v) {
  return (v == 0) ? -1 : 1;
}

class Rect {
  float x, y, w, h;
  
  void set(float x, float y, float w, float h) {
    this.x = x;
    this.y = y;
    this.w = w;
    this.h = h;
  }
};

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
