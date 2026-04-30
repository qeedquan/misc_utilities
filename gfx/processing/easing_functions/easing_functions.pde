/*

http://robertpenner.com/easing/
https://www.raylib.com/cheatsheet/cheatsheet.html

*/

PVector[] points;
color[]   palette;
int       easetype;
int       fontsz;

void setup() {
  size(1200, 800);
  fontsz = 16;
  textSize(fontsz);
  reset();
}

void reset() {
  easetype = 0;
  points = new PVector[]{
    new PVector(300, 600),
    new PVector(800, 200),
  };
  palette = new color[32];
  for (int i = 0; i < palette.length; i++)
    palette[i] = color(random(255), random(255), random(255));
}

void draw() {
  background(160, 170, 200);
  drawEaseCurve(easetype, points);
  drawInfo();
}

void drawEaseCurve(int type, PVector[] p) {
  float dt = 5e-3;
  int t0 = type;
  int t1 = type;
  if (type == 26) {
    t0 = 2;
    t1 = 9;
  } else if (type == 27) {
    t0 = 10;
    t1 = 17;
  } else if (type == 28) {
    t0 = 18;
    t1 = 25;
  } else if (type == 29) {
    t0 = 0;
    t1 = 25;
  }

  strokeWeight(2);
  for (int i = t0; i <= t1; i++) {
    push();
    stroke(palette[i]);
    curve(i, p[0].x, p[0].y, p[1].x, p[1].y, dt);
    pop();
    if (mousePressed && mouseButton == CENTER) {
      float t = linearSolve(mouseX, p[0].x, p[1].x);
      if (0 <= t && t <= 1) {
        stroke(0);
        fill(255);
        circle(mouseX, ease(i, t, p[0].y, p[1].y-p[0].y, 1.0), 10);
      }
    }
  }
  fill(30, 140, 150);
  circle(p[0].x, p[0].y, 10);
  fill(250, 100, 200);
  circle(p[1].x, p[1].y, 10);
}

void drawInfo() {
  fill(255);
  text(curveName(easetype), 20, 20);
}

void keyPressed() {
  if (keyCode == LEFT)
    easetype--;
  else if (keyCode == RIGHT)
    easetype++;
  else if (key == ' ')
    reset();

  easetype = wrap(easetype, 0, 29);
}

void mousePressed() {
  if (mouseButton == LEFT)
    points[0].set(mouseX, mouseY);
  else if (mouseButton == RIGHT)
    points[1].set(mouseX, mouseY);
}

float linearSolve(float x, float x0, float x1) {
  return (x - x0) / (x1 - x0);
}

int wrap(int x, int a, int b) {
  if (x > b)
    x = a;
  else if (x < a)
    x = b;
  return x;
}

String curveName(int type) {
  String tab[] = {
    "Step", "Linear",
    
    "Sine In", "Circle In", "Cubic In", "Quadratic In",
    "Exponential In", "Back In", "Bounce In", "Elastic In",
    
    "Sine Out", "Circle Out", "Cubic Out", "Quadratic Out",
    "Exponential Out", "Back Out", "Bounce Out", "Elastic Out",
    
    "Sine In-Out", "Circle In-Out", "Cubic In-Out", "Quadratic In-Out",
    "Exponential In-Out", "Back In-Out", "Bounce In-Out", "Elastic In-Out",
    
    "All In", "All Out", "All In-Out", "All"
  };

  if (type < 0 || type >= tab.length)
    return "Unknown";
  return tab[type];
}

void curve(int type, float x0, float y0, float x1, float y1, float dt) {
  float px = x0;
  float py = y0;
  for (float t = 0; t <= 1.0; t += dt) {
    float x = lerp(x0, x1, t);
    float y = ease(type, t, y0, y1-y0, 1.0);
    line(round(px), round(py), round(x), round(y));
    px = x;
    py = y;
  }
}

float ease(int type, float t, float b, float c, float d) {
  switch (type) {
  case 0:
    return step(t, b, c, d);
  case 1:
    return linear(t, b, c, d);

  case 2:
    return sine_in(t, b, c, d);
  case 3:
    return circ_in(t, b, c, d);
  case 4:
    return cubic_in(t, b, c, d);
  case 5:
    return quad_in(t, b, c, d);
  case 6:
    return expo_in(t, b, c, d);
  case 7:
    return back_in(t, b, c, d);
  case 8:
    return bounce_in(t, b, c, d);
  case 9:
    return elastic_in(t, b, c, d);

  case 10:
    return sine_out(t, b, c, d);
  case 11:
    return circ_out(t, b, c, d);
  case 12:
    return cubic_out(t, b, c, d);
  case 13:
    return quad_out(t, b, c, d);
  case 14:
    return expo_out(t, b, c, d);
  case 15:
    return back_out(t, b, c, d);
  case 16:
    return bounce_out(t, b, c, d);
  case 17:
    return elastic_out(t, b, c, d);
    
  case 18:
    return sine_in_out(t, b, c, d);
  case 19:
    return circ_in_out(t, b, c, d);
  case 20:
    return cubic_in_out(t, b, c, d);
  case 21:
    return quad_in_out(t, b, c, d);
  case 22:
    return expo_in_out(t, b, c, d);
  case 23:
    return back_in_out(t, b, c, d);
  case 24:
    return bounce_in_out(t, b, c, d);
  case 25:
    return elastic_in_out(t, b, c, d);
  }
  return 0;
}

float step(float t, float b, float c, float d) {
  return (t < d/2.0) ? b : b+c;
}

float linear(float t, float b, float c, float d) {
  return c*t/d + b;
}

float sine_in(float t, float b, float c, float d) {
  return (-c*cos(t/d*(PI/2.0)) + c + b);
}

float sine_out(float t, float b, float c, float d) {
  return (c*sin(t/d*(PI/2.0f)) + b);
}

float sine_in_out(float t, float b, float c, float d) {
  return (-c/2.0f*(cos(PI*t/d) - 1.0f) + b);
}

float circ_in(float t, float b, float c, float d) {
  t /= d;
  return (-c*(sqrt(1.0f - t*t) - 1.0f) + b);
}

float circ_out(float t, float b, float c, float d) {
  t = t/d - 1.0f;
  return (c*sqrt(1.0f - t*t) + b);
}

float circ_in_out(float t, float b, float c, float d) {
   if ((t/=d/2.0f) < 1.0f)
     return (-c/2.0f*(sqrt(1.0f - t*t) - 1.0f) + b);
  t -= 2.0f;
  return (c/2.0f*(sqrt(1.0f - t*t) + 1.0f) + b);
}

float cubic_in(float t, float b, float c, float d) {
  t /= d;
  return (c*t*t*t + b);
}

float cubic_out(float t, float b, float c, float d) {
  t = t/d - 1.0f;
  return (c*(t*t*t + 1.0f) + b);
}

float cubic_in_out(float t, float b, float c, float d) {
  if ((t/=d/2.0f) < 1.0f)
    return (c/2.0f*t*t*t + b);
  t -= 2.0f;
  return (c/2.0f*(t*t*t + 2.0f) + b);
}

float quad_in(float t, float b, float c, float d) {
  t /= d;
  return (c*t*t + b);
}

float quad_out(float t, float b, float c, float d) {
  t /= d;
  return (-c*t*(t - 2.0f) + b);
}

float quad_in_out(float t, float b, float c, float d) {
  if ((t/=d/2) < 1)
    return (((c/2)*(t*t)) + b);
  return (-c/2.0f*(((t - 1.0f)*(t - 3.0f)) - 1.0f) + b);
}

float expo_in(float t, float b, float c, float d) {
  return (t == 0.0f) ? b : (c*pow(2.0f, 10.0f*(t/d - 1.0f)) + b);
}

float expo_out(float t, float b, float c, float d) {
  return (t == d) ? (b + c) : (c*(-pow(2.0f, -10.0f*t/d) + 1.0f) + b);
}

float expo_in_out(float t, float b, float c, float d) {
  if (t == 0.0f)
    return b;
  if (t == d)
    return (b + c);
  t /= d/2.0f;
  if (t < 1.0f)
    return (c/2.0f*pow(2.0f, 10.0f*(t - 1.0f)) + b);
  return (c/2.0f*(-pow(2.0f, -10.0f*(t - 1.0f)) + 2.0f) + b);
}

float back_in(float t, float b, float c, float d) {
  float s = 1.70158f;
  float p = t/d;
  t /= d;
  return (c*(p)*t*((s + 1.0f)*t - s) + b);
}

float back_out(float t, float b, float c, float d) {
  float s = 1.70158f;
  t = t/d - 1.0f;
  return (c*(t*t*((s + 1.0f)*t + s) + 1.0f) + b);
}

float back_in_out(float t, float b, float c, float d) {
    float s = 1.70158f;
    t /= d/2.0f;
    if (t < 1.0f) {
        s *= 1.525f;
        return (c/2.0f*(t*t*((s + 1.0f)*t - s)) + b);
    }

    t -= 2.0f;
    float p = t;
    s *= 1.525f;
    return (c/2.0f*((p)*t*((s + 1.0f)*t + s) + 2.0f) + b);
}

float bounce_out(float t, float b, float c, float d) {
  t /= d;
  if (t < (1.0f/2.75f)) {
        return (c*(7.5625f*t*t) + b);
  } else if (t < (2.0f/2.75f)) {
    t -= 1.5f/2.75f;
    float p = t;
        return (c*(7.5625f*(p)*t + 0.75f) + b);
  } else if (t < (2.5/2.75)) {
    t -= 2.25f/2.75f;
    float p = t;
    return (c*(7.5625f*(p)*t + 0.9375f) + b);
  } else {
    t -= (2.625f/2.75f);
    float p = t; 
    return (c*(7.5625f*(p)*t + 0.984375f) + b);
  }
}

float bounce_in(float t, float b, float c, float d) {
  return (c - bounce_out(d - t, 0.0f, c, d) + b);
}

float bounce_in_out(float t, float b, float c, float d) {
  if (t < d/2.0f)
    return bounce_in(t*2.0f, 0.0f, c, d)*0.5f + b;
  else
    return bounce_out(t*2.0f - d, 0.0f, c, d)*0.5f + c*0.5f + b;
}

float elastic_in(float t, float b, float c, float d) {
  if (t == 0.0f)
    return b;
  t /= d;
  if (t == 1.0f)
    return (b + c);

  float p = d*0.3f;
  float a = c;
  float s = p/4.0f;
  t -= 1.0f;
  float pf = a*pow(2.0f, 10.0f*(t));
  return (-(pf*sin((t*d-s)*(2.0f*PI)/p )) + b);
}

float elastic_out(float t, float b, float c, float d) {
  if (t == 0.0f)
    return b;
  t /= d;
  if ((t) == 1.0f)
    return (b + c);

  float p = d*0.3f;
  float a = c;
  float s = p/4.0f;
  return (a*pow(2.0f,-10.0f*t)*sin((t*d-s)*(2.0f*PI)/p) + c + b);
}

float elastic_in_out(float t, float b, float c, float d) {
  if (t == 0.0f)
    return b;
  t /= d/2.0f;
  if (t == 2.0f)
    return (b + c);

  float p = d*(0.3f*1.5f);
  float a = c;
  float s = p/4.0f;

  if (t < 1.0f) {
    float pf = a*pow(2.0f, 10.0f*(t-=1.0f));
    return -0.5f*(pf*sin((t*d-s)*(2.0f*PI)/p)) + b;
  }
  float pf = a*pow(2.0f, -10.0f*(t-=1.0f));
  return (pf*sin((t*d-s)*(2.0f*PI)/p)*0.5f + c + b);
}
