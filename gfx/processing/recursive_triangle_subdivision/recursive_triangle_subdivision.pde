/*

https://www.johndcook.com/blog/2023/09/09/recursive-triangle-subdivision/

*/

ArrayList<PVector> points;
int maxdepth;

void setup() {
  size(1024, 768);
  maxdepth = 4;
  points = subdivision(maxdepth);
}

void draw() {
  background(255);
  blendMode(BLEND);
  drawLines(points);
  drawStatus();
}

void drawStatus() {
  String str = String.format("Depth: %d Points: %d", maxdepth, points.size());
  push();
  textSize(32);
  fill(0);
  text(str, 16, 32);
  pop();
}

void drawLines(ArrayList<PVector> points) {
  var w = 600;
  var h = 600;
  
  push();
  scale(1, -1);
  translate(width/2 - w/2, -(height/2 + h/2));
  stroke(0, 0, 255, 30);
  var length = points.size();
  for (var i = 0; i < length - 1; i++) {
    var p = points.get(i);
    var q = points.get(i + 1);

    var x1 = map(p.x, -1, 1, 0, w);
    var y1 = map(p.y, -1, 1, 0, h);
    var x2 = map(q.x, -1, 1, 0, w);
    var y2 = map(q.y, -1, 1, 0, h);
    line(x1, y1, x2, y2);
  }
  pop();
}

void keyPressed() {
  if (keyCode == LEFT) {
    maxdepth -= 1;
  } else if (keyCode == RIGHT) {
    maxdepth += 1;
  }
  
  maxdepth = clamp(maxdepth, 1, maxdepth);
  points = subdivision(maxdepth);
}

int clamp(int x, int a, int b) {
  if (x < a)
    x = a;
  else if (x > b)
    x = b;
  return x;
}

PVector centroid(ArrayList<PVector> points) {
  var center = new PVector();
  var length = points.size();
  if (length == 0)
    return center;
    
  for (var point : points) {
    center.x += point.x;
    center.y += point.y;
  }
  center.x /= length;
  center.y /= length;
  return center;
}

ArrayList<PVector> subdivision(int depth) {
  var points = new ArrayList<PVector>();
  var result = new ArrayList<PVector>();
  points.add(new PVector(0.0, 1.0));
  points.add(new PVector(-0.866, -0.5));
  points.add(new PVector(0.866, -0.5));
  mesh(points, depth, result);
  return result;
}

void mesh(ArrayList<PVector> points, int depth, ArrayList<PVector> result) {
  if (depth > 0) {
    var center = centroid(points);
    var length = points.size();
    for (var i = 0; i < length; i++) {
      for (var j = i + 1; j < length; j++) {
        var newpoints = new ArrayList<PVector>();
        newpoints.add(points.get(i));
        newpoints.add(points.get(j));
        newpoints.add(center);
        render(newpoints, result);
        mesh(newpoints, depth - 1, result);
      }
    }
  }
}

void render(ArrayList<PVector> points, ArrayList<PVector> result) {
  var length = points.size();
  for (var i = 0; i < length; i++) {
    for (var j = i + 1; j < length; j++) {
      var p = points.get(i);
      var q = points.get(j);
      result.add(p);
      result.add(q);
    }
  }
}
