/*

https://en.wikipedia.org/wiki/Taxicab_geometry

*/

Vec2i origin;
Vec2i point;
boolean[][] visited;
boolean shortest;

void setup() {
  size(1024, 768);
  strokeWeight(2);
  
  origin = new Vec2i(width/2, height/2);
  point = new Vec2i(origin.x+100, origin.y);
  visited = new boolean[width][height];
  shortest = true;
}

void draw() {
  background(220);
  drawPath(origin, point);
  circle(origin.x, origin.y, 20);
  circle(point.x, point.y, 20);
}

void drawPath(Vec2i p, Vec2i q) {
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++)
      visited[x][y] = false;
  }
  drawPathRec(p, q, taxidist(p, q), visited);
}

void drawPathRec(Vec2i p, Vec2i q, int d, boolean[][] v) {
  if (v[p.x][p.y])
    return;
  if (d == 0)
    return;
  
  Vec2i r = new Vec2i(p.x, p.y);
  int sd = (shortest) ? shortdist(p, q) : 0;
  for (int y = -1; y <= 1; y++) {
    for (int x = -1; x <= 1; x++) {
      if (x == 0 && y == 0)
        continue;

      r.set(p.x+x, p.y+y);
      int nd = taxidist(r, q);
      if ((!shortest && nd < d) || (shortest && nd == sd)) {
        stroke(r.x, r.y, max(r.x, r.y));
        point(r.x, r.y);
        drawPathRec(r, q, nd, v);
        v[r.x][r.y] = true;
      }
    }
  }
}

int shortdist(Vec2i p, Vec2i q) {
  Vec2i r = new Vec2i(p.x, p.y);
  int d = 1000000;
  for (int y = -1; y <= 1; y++) {
    for (int x = -1; x <= 1; x++) {
      if (x == 0 && y == 0)
        continue;

      r.set(p.x+x, p.y+y);
      d = min(d, taxidist(r, q));
    }
  }
  return d;
}

void keyPressed() {
  if (keyCode == ' ')
    shortest = !shortest;
}

void mouseMoved() {
  point.set(mouseX, mouseY);
}

int taxidist(Vec2i p, Vec2i q) {
  int dx = p.x - q.x;
  int dy = p.y - q.y;
  return abs(dx) + abs(dy);
}

class Vec2i {
  int x, y;
  
  Vec2i(int x, int y) {
    set(x, y);
  }
  
  void set(int x, int y) {
    this.x = x;
    this.y = y;
  }
};
