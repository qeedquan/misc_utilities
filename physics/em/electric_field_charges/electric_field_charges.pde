/*

https://en.wikipedia.org/wiki/Coulomb%27s_law
https://phys.org/news/2016-08-mysterious-magnetic-monopole.html
https://scipython.com/blog/visualizing-a-vector-field-with-matplotlib/
https://scipython.com/blog/the-electric-field-of-a-capacitor/

*/

/*

This represents an electric charge in space.
The sign of the charge (q) denotes whether or not it is a positive or negative charge.

Contrary to a magnetic monopole (which is theoretical), electric monopoles are shown to exist in nature (denoted simply as electric charge).
Due to this, we can represent a single positive or negative electric charge in space since it doesn't have to come in pairs unlike charges in magnetism.

The equation of the strength an electric charge imposes on space is given by Coulomb's Law.

There are three conditions to be fulfilled for the validity of Coulomb's inverse square law:

1. The charges must have a spherically symmetric distribution (e.g. be point charges, or a charged metal sphere).
2. The charges must not overlap (e.g. they must be distinct point charges).
3. The charges must be stationary with respect to each other.

Due to these restrictions, we represent the charge as a point electric charge.
The non-relativistic vector form of Coulomb's law is used.

q = the charge value
K = 1/(4*pi*e0)
R = position of this particular charge in space
r = position of where we want to measure how strong the field strength is, this will be affected by the charge strength
E(r) = K*q*(R-r) / |R-r|^3

E(r) can be simplified to be:
E(r) = K*q*normalize(R-r)/|R-r|^2
This is why people say Coloumb law is an inverse square law

E(r) returns a vector at any given point, so it generates a vector field when applied to all points in space.
This equation has a singularity when R=r, due to charge is always being discrete in reality, and the "continuous charge" assumption is just an approximation that is not supposed to allow.

Since K is a constant we scale it to be 1 and map the position space to be in and charges to be within a drawable space, otherwise the values can become too big to draw nicely.

Since the field strength is considered to be linear, calculating the strength of the field at a point due to multiple charges is given by adding up the sum of the strength given by a single charge on that position.
E(r) = E_1(r) + E_2(r) + E_3(r) ...

The number of charges in space is given various names:
Monopole    - 1 charge in space
Dipoles     - 2 charges
Quadrupoles - 4 charges
Octopoles   - 8 charges
and so on...

These special configurations are placed on a unit circle in an alternating charges with uniform angle differences from each other.
Hence, the configurations can be generated using <q*cos(t), q*sin(t)> where q alternates -1,+1,-1,+1 and t=360/N where N is the number of charges.

A capacitor can be represented as rows of electric charges of the same type separated by a distance d:

+++++++
ddddddd
-------

So it can be seen as a lattice of electric charges.

By convention electric field lines are represented as follows:
Positive charge has the field lines going out of it.
Negative charges has the field lines going into it.

*/

class Charge {
  final int K = 1;

  float q, x, y;

  Charge(float q, float x, float y) {
    this.q = q;
    this.x = x;
    this.y = y;
  }

  PVector strength(float x, float y) {
    float dx = this.x - x;
    float dy = this.y - y;
    float dist = pow(sqrt(dx*dx + dy*dy), 3);
    if (dist == 0)
      return new PVector();
    return new PVector(K*q*dx / dist, K*q*dy / dist);
  }
}

ArrayList<Charge> charges;

void setup() {
  size(1280, 800);
  reset();
}

void reset() {
  charges = new ArrayList<Charge>();
}

PVector field(float x, float y) {
  PVector E = new PVector();
  for (Charge c : charges) {
    E.add(c.strength(x, y));
  }
  return E;
}

void draw() {
  background(200);
  drawField();
  drawCharges();
}

/*

Represent the space of charges to be in [-1, 1]
This makes the vector field viewable, otherwise the values can get too big.

*/

void drawField() {
  int resolution = 32;
  for (int y = 0; y < height; y += resolution) {
    for (int x = 0; x < width; x += resolution) {
      float px = map(x, 0, width, -1, 1);
      float py = map(y, 0, height, -1, 1);
      
      PVector vector = field(px, py);
      float scale = constrain(vector.mag()+5, 0, resolution);
      vector.normalize();
      
      float x1 = x;
      float y1 = y;
      float x2 = x1 + vector.x*scale;
      float y2 = y1 + vector.y*scale;
      
      line(x1, y1, x2, y2);
      fill(255, 255, 255);
      
      // draw the circle at the starting point instead of ending point
      // to make it look like the convention of + charges going out and - charges going in
      // this also has to do with the fact that people used to think positive charges goes to negative charges
      // but its actually the other way around, negative charges goes to positive charge.
      circle(x1, y1, 4);
    }
  }
}

void drawCharges() {
  for (Charge c : charges) {
    if (c.q >= 0)
      fill(160, 0, 0, 255);
    else
      fill(0, 0, 160, 255);
    
    float x = map(c.x, -1, 1, 0, width);
    float y = map(c.y, -1, 1, 0, height);
    circle(x, y, 10);
  }
}

void keyPressed() {
  if (keyCode == ' ')
    reset();
}

void mousePressed() {
  float cx = map(mouseX, 0, width, -1, 1);
  float cy = map(mouseY, 0, height, -1, 1);
  if (mouseButton == LEFT) {
    charges.add(new Charge(1, cx, cy));
  } else if (mouseButton == RIGHT) {
    charges.add(new Charge(-1, cx, cy));
  }
}
