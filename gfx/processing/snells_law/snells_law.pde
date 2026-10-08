/*

https://en.wikipedia.org/wiki/Snell%27s_law
https://arachnoid.com/OpticalRayTracer/snells_law_calculator.html

*/

Surface surface;
int fontsz;

void setup() {
  size(1200, 800);
  fontsz = 16;
  textSize(fontsz);

  surface = new Surface();
}

void draw() {
  background(150);
  surface.draw();
  surface.update();
}

void dashline(float x1, float y1, float x2, float y2, int n) {
  PVector p = new PVector(x1, y1);
  PVector q = new PVector();
  for (int i = 1; i < 2*n; i++) {
    float t = i*0.5/n;

    q.set(x1 + (x2-x1)*t, y1 + (y2-y1)*t);
    if ((i&1) != 0)
      line(p.x, p.y, q.x, q.y);
    p.set(q);
  }
}

void keyPressed() {
  surface.keyPressed();
}

void mouseWheel(MouseEvent ev) {
  surface.mouseWheel(ev);
}

float sign(float x) {
  return (x < 0) ? -1 : 1;
}

float clamp(float x, float a, float b) {
  return min(max(x, a), b);
}

float[] snell(float[] p, int mask) {
  float[] r = new float[4];
  for (int i = 0; i < 4; i++)
    r[i] = p[i];

  if ((mask&0x1) != 0)
    r[0] = p[1]*sin(p[3])/sin(p[2]);
  if ((mask&0x2) != 0)
    r[1] = p[0]*sin(p[2])/sin(p[3]);
  if ((mask&0x4) != 0)
    r[2] = asin(p[1]/p[0]*sin(p[3]));
  if ((mask&0x8) != 0)
    r[3] = asin(p[0]/p[1]*sin(p[2]));
  
  return r;
}

class Rect {
  PVector min, max;
  
  Rect() {
    min = new PVector();
    max = new PVector();
  }
  
  Rect(float x1, float y1, float x2, float y2) {
    min = new PVector(x1, y1);
    max = new PVector(x2, y2);
  }
  
  float width() {
    return max.x - min.x;
  }
  
  float height() {
    return max.y - min.y;
  }
  
  boolean inside(PVector p) {
    return min.x <= p.x && p.x <= max.x && 
           min.y <= p.y && p.y <= max.y;
  }
}

class Textbox {
  Rect box;
  String text;
  PVector col;
  PVector selcol;
  boolean sel;
  
  Textbox() {
    box = new Rect();
    col = new PVector(80, 80, 117);
    selcol = new PVector(50, 50, 230);
    text = "";
    sel = false;
  }

  void draw() {
    push();
    strokeWeight(2);
    if (sel)
      fill(selcol.x, selcol.y, selcol.z);
    else
      fill(col.x, col.y, col.z);
    rect(box.min.x, box.min.y, box.width(), box.height());
    
    fill(255);
    textAlign(CENTER, CENTER);
    text(text, (box.min.x+box.max.x)/2, (box.min.y+box.max.y)/2);
    pop();
  }
}

class Surface {
  float angle;
  float[] params;
  Textbox[] texts;
  int textsel;
  
  Surface() {
    texts = new Textbox[4];
    for (int i = 0; i < texts.length; i++) {
      float x = 16;
      float y = height - fontsz*12 + fontsz*3*i;
      float sx = 150;
      float sy = fontsz*2;
      texts[i] = new Textbox();
      texts[i].box = new Rect(x, y, x+sx, y+sy);
    }
    reset();
  }
  
  void update() {
    textsel = -1;
    PVector m = new PVector(mouseX, mouseY);
    for (int i = 0; i < texts.length; i++) {
      texts[i].sel = false;
      if (texts[i].box.inside(m)) {
        texts[i].sel = true;
        textsel = i;
        break;
      }
    }
  }
  
  void reset() {
    angle = 0;
    params = snell(new float[]{1, 1.5, radians(60), 0}, 0x8);
  }
  
  void keyPressed() {
    if (keyCode == ' ')
      reset();
    else if (key == ENTER) {
      if (textsel != -1)
        params = snell(params, 1<<textsel);
    }
  }
 
  // snell's law allows us to calculate how light rays can bend going into different surfaces
  // sin(t1)/sin(t2) = n1/n2
  // t1/t2 is the incoming angle and the outgoing angle a ray makes with the surface
  // n1/n2 is the refractive index of the incoming and outgoing surface
  //
  // when we change the parameters we only affect the incoming/outgoing light ray angles because of the following
  // physical assumptions applied:
  // 1. changing the angles only changes the light ray angles,
  //    the refraction index still says the same since we assumed the surface didn't change.
  // 2. changing the refraction index will for either surface keeps the incoming light angle
  //    the same, so only the outgoing angle changes
  //
  // The light ray hits a surface at one side so only a half plane is allowed, meaning the angles
  // we can make with the incoming ray is [-90, 90] for 180 degree coverage.
  // The outgoing light is not clamped so some unphysical situation can apply.
  //
  // The refractive index n1 and n2 determines how light bends
  // n1 == n2 means the medium are the same so the light ray does not bend (travel in a straight line)
  // n1 < n2 is the normal case, since n2 is a medium that will make light slower (bends)
  // n1 > n2 the ray of light is exiting a refractive medium so total internal reflection may be met
  void mouseWheel(MouseEvent ev) {
    int mask = 0x8;
    switch (textsel) {
    case -1:
      angle = (angle + radians(ev.getCount()*10)) % (2*PI);
      break;
    case 0:
    case 1:
      params[textsel] += sign(ev.getCount()) * 1e-2;
      break;
    case 2:
    case 3:
      params[textsel] += radians(ev.getCount());
      if (textsel == 3)
        mask = 0x4;
      break;
    }
    params = snell(params, mask);
    params[2] = clamp(params[2], -PI/2, PI/2);
  }
  
  void draw() {
    drawBG();
    drawRays();
    drawParams(params);
  }
  
  void drawBG() {
    push();
    strokeWeight(2);
    
    fill(100, 150, 180);
    drawPlane(width/2, height/2, 2, 2, 0, 0, angle);
    
    fill(100, 180, 120);
    drawPlane(width/2, height/2, 2, 2, 0, height/2, angle);
    
    drawNormalLine(angle);
    
    pop();
  }
  
  void drawParams(float[] p) {
    String[] names = { "n1", "n2", "θ1", "θ2" };
    for (int i = 0; i < p.length; i++) {
      float r = (i >= 2) ? degrees(p[i]) : p[i];
      texts[i].text = String.format("%s: % .6f", names[i], r);
      texts[i].draw();
    }
  }
  
  void drawRays() {
    push();
    float r = width;
    float t = angle + PI/2 - params[2];
    PVector p = new PVector(width/2, height/2);
    PVector d = new PVector(-r*cos(t), -r*sin(t));
    stroke(200, 0, 0);
    line(p.x, p.y, p.x+d.x, p.y+d.y);
    
    t = angle + PI/2 - params[3];
    d = new PVector(r*cos(t), r*sin(t));
    stroke(0, 0, 200);
    line(p.x, p.y, p.x+d.x, p.y+d.y);
    pop();
  }
  
  void drawNormalLine(float angle) {
    float r = width/4;
    PVector p = new PVector(width/2, height/2);
    PVector d = new PVector(r*cos(angle+PI/2), r*sin(angle+PI/2));
    dashline(p.x-d.x, p.y-d.y, p.x+d.x, p.y+d.y, 20);
  }
  
  void drawPlane(float cx, float cy, float sx, float sy, float tx, float ty, float angle) {
    PVector corners[] = {
      new PVector(0, 0),
      new PVector(1, 0),
      new PVector(1, 1),
      new PVector(0, 1),
    };
    PMatrix2D rot = new PMatrix2D();
    
    rot.rotate(angle);
    beginShape(QUADS);
    for (PVector p : corners) {
      p.x = sx * (p.x*cx*2 - cx + tx);
      p.y = sy * (p.y*cy - cy + ty);
      p = rot.mult(p, null);
      p.x += cx;
      p.y += cy;
      vertex(p.x, p.y);
    }
    endShape();
  }
}
