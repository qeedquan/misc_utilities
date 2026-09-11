/*

https://www2.clarku.edu/faculty/djoyce/trig/tangents.html
https://en.wikipedia.org/wiki/Unit_circle#Trigonometric_functions_on_the_unit_circle

*/

float radius;
float angle;
int fontsz;
boolean animate;

void setup() {
  size(1024, 768);
  fontsz = 16;
  textSize(fontsz);
  radius = 350;
  angle = 0;
  animate = false;
}

void draw() {
  background(150);
  
  push();
  strokeWeight(2);
  fill(150, 170, 200);
  circle(width/2, height/2, radius);
  trigonometry(width/2, height/2, radius/2);
  drawText();
  pop();
      
  if (animate)
    angle += radians(1);
  angle %= 2*PI;
}

void drawText() {
  float c = cos(angle);
  float s = sin(angle);
  float t = tan(angle);
  float vc = 1 - s; 
  float vs = 1 - c;
  float sc = 1 / c;
  float cs = 1 / s;
  float ct = 1 / t;
  
  fill(255);
  text(String.format("%-16s % .0f", "Radius:", radius), 16, 16 + 0*fontsz);
  text(String.format("%-16s % .0f", "Angle:", degrees(angle)), 16, 16 + 1*fontsz);
  
  fill(0, 0, 255);
  text(String.format("%-16s % .6f", "Cosine:", c), 16, 16 + 2*fontsz);
  fill(255, 0, 0);
  text(String.format("%-16s % .6f", "Sine:", s), 16, 16 + 3*fontsz);
  fill(219, 188, 146);
  text(String.format("%-16s % .6f", "Tangent:", t), 16, 16 + 4*fontsz);
  fill(0, 255, 255);
  
  fill(0, 140, 140);
  text(String.format("%-16s % .6f", "Secant:", sc), 16, 16 + 5*fontsz);
  fill(255, 128, 0);
  text(String.format("%-16s % .6f", "Cosecant:", cs), 16, 16 + 6*fontsz);
  fill(0, 128, 0);
  text(String.format("%-16s % .6f", "Cotangent:", ct), 16, 16 + 7*fontsz);
  
  fill(0, 255, 255);
  text(String.format("%-16s % .6f", "Coversine:", vc), 16, 16 + 8*fontsz);  
  fill(0, 255, 0);
  text(String.format("%-16s % .6f", "Versine:", vs), 16, 16 + 9*fontsz);  
}

void trigonometry(float x, float y, float r) {
  float c = r * cos(angle);
  float s = r * sin(angle);
  float t = r * tan(angle);
  float vs = r * (1 - cos(angle));
  float vc = r * (1 - sin(angle));
  float sc = r * (1/cos(angle));
  float cs = r * (1/sin(angle));
  float ct = r * (1/tan(angle));
  
  noStroke();
  fill(100, 100, 200);
  triangle(x, y, x+c, y, x+c, y-s);

  // hypotenuse
  stroke(0, 0, 0);
  line(x, y, x+c, y-s);
  
  // coversine
  stroke(0, 255, 255);
  line(x, y-s, x, y-s-vc);
  
  // versine
  stroke(0, 255, 0);
  line(x+c, y, x+c+vs, y);
  
  // secant
  stroke(0, 140, 140);
  line(x, y+r, x+sc, y+r);
  
  // cosecant
  stroke(255, 128, 0);
  line(x-r, y, x-r, y+cs);
  
  // cosine
  stroke(0, 0, 255);
  line(x, y, x+c, y);
  
  // sine
  stroke(255, 0, 0);
  line(x+c, y, x+c, y-s);
  
  // cotangent
  stroke(0, 128, 0);
  line(x+r, y, x+r, y-ct);
  
  // tangent
  stroke(219, 188, 146);
  line(x+r, y, x+r, y-t);
  
  // hypotenuse to tangent
  stroke(200, 200, 200);
  line(x+c, y-s, x+r, y-t);
  
  // line connecting two points on a circle with one point
  // at the hypotenuse
  stroke(150, 50, 245);
  line(x+c, y-s, x-s, y+c);
}

void keyPressed() {
  if (keyCode == ' ')
    animate = !animate;
  else if (keyCode == LEFT)
    angle -= radians(5);
  else if (keyCode == RIGHT)
    angle += radians(5);
}

void mouseWheel(MouseEvent ev) {
  radius *= exp(0.2*ev.getCount());
}

float sign(float x) {
  return (x < 0) ? -1 : 1;
}
