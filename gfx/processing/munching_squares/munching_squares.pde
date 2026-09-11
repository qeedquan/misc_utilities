/*

https://en.wikipedia.org/wiki/Munching_square

*/

int tick;
int modulus;

void setup() {
  size(800, 600);
  background(0);
  loadPixels();
  reset();
}

void reset() {
  tick = 0;
  modulus = 1000;
}

void keyPressed() {
  if (keyCode == RIGHT)
    modulus++;
  else if (keyCode == LEFT)
    modulus--;
  
  println("Tick:", tick);
  println("Modulus:", modulus);
}

void clear() {
  for (int i = 0; i < pixels.length; i++)
    pixels[i] = color(0);
}

void draw() {
  munch(tick);  
  if (++tick >= modulus) {
    clear();
    tick = 0;
  }
  updatePixels();
}

// plot y = x xor t for varying t
void munch(int t) {
  for (int x = 0; x < width; x++) {
    int y = x ^ t;
    int i = y*width + x;
    if (i < pixels.length) {
      int r = t ^ x ^ y;
      int g = t;
      int b = t ^ x;
      r &= 0xff;
      g &= 0xff;
      b &= 0xff;
      pixels[y*width+x] = color(r, g, b);
    }
  }
}
