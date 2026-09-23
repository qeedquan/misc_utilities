// ported from https://github.com/fabioarnold/TwoTriangles
Swirl swirl;
PVector center;
boolean pause;

void setup() {
  size(1200, 800);
  colorMode(RGB, 1);
  loadPixels();
  swirl = new Swirl();
  center = new PVector(width/2, height/2);
  pause = false;
}

void draw() {
  swirl.draw(millis(), center.x - width/2, center.y - height/2, 0, 0, width, height);
  updatePixels();
  if (!pause)
    center.set(mouseX, mouseY);
}

void keyPressed() {
  if (keyCode == ENTER)
    pause = !pause;
  else
    swirl.keyPressed();
    
  println("Pause:", pause);
  println("Stripe:", swirl.stripe);
  println("Curl:", swirl.curl);
  println("Speed:", swirl.speed);
  println();
}

float clamp(float x, float a, float b) {
  return min(max(x, a), b);
}

float smoothstep(float t0, float t1, float x) {
  float t = clamp((x - t1) / (t1 - t0), 0.0, 1.0);
  return t * t * (3.0 - 2.0 * t);
}

PVector mix(PVector a, PVector b, float t) {
  float cr = lerp(a.x, b.x, t);
  float cg = lerp(a.y, b.y, t);
  float cb = lerp(a.z, b.z, t);
  return new PVector(cr, cg, cb);
}

PVector randrgb() {
  return new PVector(random(1.0), random(1.0), random(1.0));
}

class Swirl {
  float speed;
  float curl;
  float blur;
  int stripe;
  PVector[] pal;
  
  Swirl() {
    reset();
  }
  
  void reset() {
    curl = 12;
    speed = 12;
    stripe = 8;
    blur = 0;
    pal = new PVector[]{
      new PVector(0.5, 0.7, 0.8),
      new PVector(0.4, 0.6, 0.8),
    };
  }
  
  void keyPressed() {
    if (keyCode == LEFT)
      curl -= 1.0;
    else if (keyCode == RIGHT)
      curl += 1.0;
    else if (keyCode == UP)
      stripe += 1;
    else if (keyCode == DOWN)
      stripe -= 1;
    else if (key == ' ')
      reset();
    else if (key == 'r') {
      pal[0] = randrgb();
      pal[1] = randrgb();
    }
  }
  
  void draw(float t, float cx, float cy, float x0, float y0, float w, float h) {
    for (int i = 0; i < pixels.length; i++)
      pixels[i] = color(0);
    for (float y = 0; y <= h; y++) {
      for (float x = 0; x <= w; x++) {
        float ax = max(1.0, w/h);
        float ay = max(1.0, h/w);
        float px = 2.0*(x-cx)/min(w, h) - ax;
        float py = 2.0*(y-cy)/min(w, h) - ay;
        float d2 = 0.25*(px*px + py*py);
        float ang = atan2(py, px);
        float stp = smoothstep(-blur, blur, sin(stripe*ang + speed*t + curl*sqrt(d2)));
        PVector col = mix(pal[0], pal[1], stp);
        
        int ox = round(x0 + x);
        int oy = round(y0 + y);
        int i = oy * width + ox;
        if (i < pixels.length)
          pixels[i] = color(col.x, col.y, col.z);
      }
    }
  }
}
