// this simulates 1/z step for x and y
// basically if you project x, y to be x/z, and y/z, it will move
// the line to the left, because the scale affects the location too
// here we try to simulate 1/z scale but keep the line centered, so calculate
// the length of previous line against this line and subtract
size(1200, 800);
float l = 400;
float x0 = 1200/2 - 200;
float y0 = 700;
float x1 = x0 + l;
float y1 = y0;
float s = 1.1;
float d = 0;
stroke(0, 0, 255, 255);
for (int i = 0; i < 50; i++) {
  line(x0, y0, x1, y1);
  y0 = y1 = y0 - 5;
  d = l - l/s;
  l = l/s;
  x0 += d/2;
  x1 -= d/2;
}