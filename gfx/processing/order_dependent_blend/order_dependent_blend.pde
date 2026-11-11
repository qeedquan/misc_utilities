// blending is order dependent, this demo shows that by permuting the order of the blends
// on the same shapes, if want order independent blending need to do something more than just
// blend naively
// https://en.wikipedia.org/wiki/Order-independent_transparency
int N = 36;

color[] colors;
Perm    perm;

int     blend_mode  = 0;
int     square_type = 0;

boolean shuffling_colors = false;
boolean shuffling_draw   = false;

void alloc(int n) {
  colors = randomColors(n);
  perm = new Perm(n);
}

void setup() {
  size(800, 800);
  alloc(N);
}

void draw() {
  background(204);
  squares(square_type, width/2, height/2, 250, colors);
  if (shuffling_colors)
    shuffleColors();
}

int setBlend(int mode) {
  int[] modes = {
    BLEND, ADD, SUBTRACT, DARKEST, LIGHTEST, DIFFERENCE, EXCLUSION,
    MULTIPLY, SCREEN, REPLACE
  };
  if (mode < 0)
    mode = 0;
  if (mode >= modes.length)
    mode = modes.length - 1;
  blendMode(modes[mode]);
  return mode;
}

void keyPressed() {
  if (keyCode == ' ')
    alloc(N);
  if (keyCode == RIGHT)
    square_type = (square_type < 1) ? square_type+1 :square_type;
  if (keyCode == LEFT)
    square_type = (square_type > 0) ? square_type-1 : square_type;
  if (key == 's')
    shuffling_colors = !shuffling_colors;
  if (key == 'd')
    shuffling_draw = !shuffling_draw;
  if (key == 'z')
    blend_mode = setBlend(blend_mode - 1);
  if (key == 'x')
    blend_mode = setBlend(blend_mode + 1);
  if (key == '1') {
    if (N > 0)
      alloc(--N);
  }
  if (key == '2') {
    alloc(++N);
  }
  
  println("colors: ", N);
  println("blend mode: ", blend_mode); 
  println("square type: ", square_type);
  println("shuffling color: ", shuffling_colors);
  println("shuffling draw: ", shuffling_draw);
  println();
}

void squares(int mode, int xc, int yc, int size, color[] cols) {
  Rect[] rl = new Rect[cols.length];

  int orig_size = size;
  for (int i = 0; i < cols.length; i++) {    
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    switch (mode) {
    case 0:
      x = xc - size/2;
      y = yc - size/2;
      if ((i & 1) != 0)
        x = xc;
      if ((i & 2) != 0)
        y = yc;
      w = h = size/2;
      break;
    
    case 1:
      int ox = i & 1;
      int oy = (i >> 1) & 1;
      x = xc + ox*size/2 - orig_size/2;
      y = yc + oy*size/2 - orig_size/2;
      w = h = size;
      break;
    }
    
    rl[i] = new Rect(x, y, w, h);
    if ((i&3) == 3) {
      size /= 2;
    }
  }
  
  IntList shuf = new IntList();
  for (int i = 0; i < rl.length; i++)
    shuf.append(i);
  if (shuffling_draw)
    shuf.shuffle();
  for (int i = 0; i < rl.length; i++) {
    int j = shuf.get(i);
    fill(cols[j]);
    rect(rl[j].x, rl[j].y, rl[j].w, rl[j].h);
  }
}

color[] randomColors(int size) {
  color[] colors = new color[size];
  for (int i = 0; i < size; i++) {
    colors[i] = color(random(0, 255), random(0, 255), random(0, 255), random(0, 255));
  }
  return colors;
}

void shuffleColors() {
  int[] shuf = perm.next();
  if (shuf == null)
    return;
  for (int i = 0; i < colors.length; i++) {
    colors[shuf[i]] = colors[i];
  }
}

class Perm {
  class Elem {
    int     idx;
    boolean dir;
    
    Elem get() {
      Elem p = new Elem();
      p.idx = idx;
      p.dir = dir;
      return p;
    }
  };
  
  private Elem[] elems;
  
  Perm(int size) {
    elems = new Elem[size];
    for (int i = 0; i < elems.length; i++)
      elems[i] = new Elem();
    reset();
  }
  
  public void reset() {
    for (int i = 0; i < elems.length; i++) {
      elems[i].idx = i;
      elems[i].dir = false;
    }
  }
  
  private int[] largest() {
    int x = -1;
    int y = -1;
    int v = -1;
    for (int i = 0; i < elems.length; i++) {
      if (i > 0 && !elems[i].dir && elems[i].idx > elems[i-1].idx && elems[i].idx > v) {
        x = i;
        y = i - 1;
        v = elems[i].idx;
      } else if (i < elems.length-1 && elems[i].dir && elems[i].idx > elems[i+1].idx && elems[i].idx > v) {
        x = i;
        y = i + 1;
        v = elems[i].idx;
      }
    }
    
    return new int[]{x, y, v};
  }
  
  private void reverse(int v) {
    for (int i = 0; i < elems.length; i++) {
      if (elems[i].idx > v)
        elems[i].dir = !elems[i].dir;
    }
  }
  
  private void swap(int i, int j) {
    Elem a = elems[i].get();
    Elem b = elems[j].get();
    elems[i] = b;
    elems[j] = a;
  }
  
  public int[] next() {
    int[] r = largest();
    if (r[0] == -1) {
      reset();
      r = largest();
    }
    if (r[0] < 0)
      return null;

    swap(r[0], r[1]);
    reverse(r[2]);
    
    int[] p = new int[elems.length];
    for (int i = 0; i < elems.length; i++)
      p[i] = elems[i].idx;
    return p;
  }
};

class Rect {
  public int x, y, w, h;
  
  Rect(int x, int y, int w, int h) {
    this.x = x;
    this.y = y;
    this.w = w;
    this.h = h;
  }
};
