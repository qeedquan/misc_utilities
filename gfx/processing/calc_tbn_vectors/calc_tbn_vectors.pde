// compute normal/tangent/bitangent vectors for a given
// triangle with position and texture coordinates
Triangle tri;

void setup() {
  size(800, 800);
  tri = new Triangle();
}

void draw() {
  background(100);
  tri.draw();
}

void mousePressed() {
  switch (mouseButton) {
  case LEFT:
    tri.p1 = new Vec3(mouseX, mouseY, random(1000));
    tri.t1 = randv2n();
    break;
  case RIGHT:
    tri.p2 = new Vec3(mouseX, mouseY, random(1000));
    tri.t2 = randv2n();
    break;
  case CENTER:
    tri.p0 = new Vec3(mouseX, mouseY, random(1000));
    tri.t0 = randv2n();
    break;
  }
}

class Vec3 {
  public float x, y, z;
  
  Vec3() {
    this(0, 0, 0);
  }
  
  Vec3(float vx, float vy, float vz) {
    x = vx;
    y = vy;
    z = vz;
  }
  
  Vec3(Vec3 v) {
    this(v.x, v.y, v.z);
  }
  
  void set(float vx, float vy, float vz) {
    x = vx;
    y = vy;
    z = vz;
  }
  
  void set(Vec3 v) {
    set(v.x, v.y, v.z);
  }
  
  Vec3 add(Vec3 v) {
    return new Vec3(x+v.x, y+v.y, z+v.z);
  }
  
  Vec3 sub(Vec3 v) {
    return new Vec3(x-v.x, y-v.y, z-v.z);
  }
  
  Vec3 scale(float s) {
    return new Vec3(x*s, y*s, z*s);
  }
  
  float dot(Vec3 v) {
    return x*v.x + y*v.y + z*v.z;
  }
  
  Vec3 cross(Vec3 v) {
    return new Vec3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
  }
  
  float length() {
    return sqrt(dot(this));
  }
  
  Vec3 normalize() {
    float l = length();
    return new Vec3(x/l, y/l, z/l);
  }
  
  Vec3 xy() {
    return new Vec3(x, y, 0);
  }
  
  String toString() {
    return String.format("{%.3f, %.3f, %.3f}", x, y, z);
  }
};

class Triangle {
  public Vec3 p0, p1, p2;
  public Vec3 t0, t1, t2;
  
  Triangle() {
    p0 = randv3();
    p1 = randv3();
    p2 = randv3();
    t0 = randv2n();
    t1 = randv2n();
    t2 = randv2n();
  }
  
  Vec3 calcNormal() {
    // 2 edges create a parallelogram on a plane
    // we can view the 2 edges as a 2 basis vectors,
    // so we can take their cross product to get
    // another vector that is perpendicular
    // to the plane, which is the normal vector
    // normalize it since we only care about direction
    Vec3 v1 = p1.sub(p0);
    Vec3 v2 = p2.sub(p0);
    return v1.cross(v2).normalize();
  }
  
  void calcTangents(Vec3 normal, Vec3 tangent, Vec3 bitangent) {
    // since we calculated the normal is perpendicular to the triangle
    // the remaining two vector that form a basis should be on the plane
    // that is spanned by the two edges of the position triangle
    // there are infinite of these vectors since we can view it the basis
    // as coordinates on a circle as long as they are 90 degrees apart.
    
    // the basis vector convention that most people choose to use
    // to create the tangent/bitangent is the follows:
    // since the tangent/bitangent vectors are on the same plane
    // as the edges of the triangle in the position, and we want to
    // map the triangle texture coordinate to the position coordinate
    // hence we have the following equation:
    
    // dp1 - edge 1 of triangle position
    // dp2 - edge 2 of triangle position
    // dt1 - edge 1 of triangle texcoord
    // dt2 - edge 2 of triangle texcoord
    // dp1 = dt1.x*T + dt1.y*B
    // dp2 = dt2.x*T + dt2.y*B
    // so the edges of the triangle position is written in term of
    // the basis of T and B with scale size of the texture coordinate
    
    // https://en.wikipedia.org/wiki/Cramer%27s_rule#Finding_inverse_matrix
    // we can use cramer rule to solve the simultaneous equation
    // and get these equations:
    // x = (c1*b2 - b1*c2)/(a1*b2 - b1*a2)
    // y = (a1*c2 - c1*a2)/(a1*b2 - b1*a2)
    // [a1 b1] [x] = [c1]
    // [a2 b2] [y] = [c2]
    
    Vec3 dp1 = p1.sub(p0);
    Vec3 dp2 = p2.sub(p0);
    Vec3 dt1 = t1.sub(t0);
    Vec3 dt2 = t2.sub(t0);
    float d = dt1.x*dt2.y - dt1.y*dt2.x;
    if (d != 0)
      d = 1.0/d;
    
    Vec3 x0 = dp1.scale(dt2.y);
    Vec3 x1 = dp2.scale(dt1.y);
    Vec3 x2 = x0.sub(x1);
    Vec3 x3 = x2.scale(d);
    
    Vec3 y0 = dp2.scale(dt1.x);
    Vec3 y1 = dp1.scale(dt2.x);
    Vec3 y2 = y0.sub(y1);
    Vec3 y3 = y2.scale(d);
    
    // they should match since the dp1 and dp2
    // vector are written in terms of the T and B basis vector
    // the T and B basis are not in the same direction as dp1/dp2
    // but they are scaled and combined to make dp1/dp2
    println(dp1 + " " + x3.scale(dt1.x).add(y3.scale(dt1.y)));
    println(dp2 + " " + x3.scale(dt2.x).add(y3.scale(dt2.y)));
    
    // use gram schmidt to orthogonalize the vectors
    // http://foundationsofgameenginedev.com/FGED2-sample.pdf
    // t_p = nrm(t - (dot(t, n)*n))
    Vec3 T0 = x3.normalize();
    Vec3 T1 = T0.sub(normal.scale(T0.dot(normal)));
    
    // b_p = nrm(b - (dot(b,n)*n - (dot(b, t_p)*t_p))
    Vec3 B0 = y3.normalize();
    Vec3 B1 = B0.sub(normal.scale(B0.dot(normal)));
    Vec3 B2 = T1.scale(B0.dot(T1));
    Vec3 B3 = B1.sub(B2);
    
    tangent.set(T1.normalize());
    bitangent.set(B3.normalize());
  }
  
  void draw() {
    drawTriangle();
    drawVectors();
  }
  
  void drawVectors() {
    Vec3 normal = calcNormal();
    Vec3 tangent = new Vec3();
    Vec3 bitangent = new Vec3();
    calcTangents(normal, tangent, bitangent);
    
    float sz = 16;
    float x = 10;
    float y = 32;
    textSize(sz);
    text("P0: " + p0 + " T0: " + t0, x, y);
    y += sz;
    text("P1: " + p1 + " T1: " + t1, x, y);
    y += sz;
    text("P2: " + p2 + " T2: " + t2, x, y);
    y += sz;
    text("N: " + normal, x, y);
    y += sz;
    text("T: " + tangent, x, y);
    y += sz;
    text("B: " + bitangent, x, y);
    y += sz;
    text("Dot: " + normal.dot(tangent) + " " + normal.dot(bitangent) + " " + tangent.dot(bitangent), x, y);
    
    stroke(255, 255, 255);
    float l = p1.sub(p0).length();
    Vec3 tp = tangent.scale(l/2);
    arrow(p0.x, p0.y, p0.x+tp.x, p0.y+tp.y);
    
    stroke(0, 0, 0);
    l = p2.sub(p0).length();
    Vec3 bp = bitangent.scale(l/2);
    arrow(p0.x, p0.y, p0.x+bp.x, p0.y+bp.y);
  }
  
  void drawTriangle() {
    stroke(255, 0, 0);
    arrow(p0.x, p0.y, p1.x, p1.y);
    stroke(0, 255, 0);
    arrow(p0.x, p0.y, p2.x, p2.y);
    stroke(0, 0, 255);
    arrow(p1.x, p1.y, p2.x, p2.y);
  }
};

void arrow(float x1, float y1, float x2, float y2) {
  line(x1, y1, x2, y2);
  pushMatrix();
  translate(x2, y2);
  float a = atan2(x1-x2, y2-y1);
  rotate(a);
  line(0, 0, -10, -10);
  line(0, 0, 10, -10);
  popMatrix();
}

Vec3 randv3() {
  return new Vec3(random(width/2), random(height/2), random(1000));
}

Vec3 randv2n() {
  Vec3 v = new Vec3(random(1), random(1), random(1));
  return v.normalize().xy();
}
