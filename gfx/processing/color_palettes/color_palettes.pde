Palette[] palettes;
int       mood;
int       fontsz;

void setup() {
  size(1024, 768);
  fontsz = 14;
  textSize(fontsz);

  mood = 0;
  palettes = new Palette[]{
    new Palette("Raylib", raylib),
    new Palette("Macbeth Chart", macbeth),
  };
}

void draw() {
  background(200, 200, 200);
  blendMode(REPLACE);
  strokeWeight(5);
  drawPalette(palettes[mood].colors);
  drawInfo();
}

void keyPressed() {  
  if (keyCode == LEFT)
    mood--;
  else if (keyCode == RIGHT)
    mood++;
  mood = wrap(mood, 0, palettes.length-1);
}

int wrap(int x, int a, int b) {
  if (x < a)
    return b;
  if (x > b)
    return a;
  return x;
}

void drawPalette(Color[] palette) {
  for (int i = 0; i < palette.length; i++) {
    int l = 100;
    int x = 20 + l*(i%8) + 10*(i%8);
    int y = 80 + l*(i/8) + 10*(i/8);
    String s = palette[i].name;
    PVector c = palette[i].value;
    fill(c.x, c.y, c.z);
    rect(x, y, l, l);
    
    if (showColorName(x, y, l)) {
      fill(0, 0, 0);
      rect(x, y+l-fontsz-10, l, fontsz+10);

      fill(255, 255, 255);
      textAlign(CENTER);
      text(s, x+l/2, y+l-fontsz);
    }
  }
}

void drawInfo() {
  push();
  fill(80, 100, 180);
  textSize(32);
  textAlign(LEFT);
  text(palettes[mood].name, 16, height-16);
  pop();
}

boolean showColorName(int x, int y, int l) {
  if (keyPressed && keyCode == SHIFT)
    return true;
  if (x <= mouseX && mouseX <= x+l && y <= mouseY && mouseY <= y+l)
    return true;
  return false;
}

class Palette {
  String name;
  Color[] colors;
  
  Palette(String name, Color[] colors) {
    this.name = name;
    this.colors = colors;
  }
}

class Color {
  String name;
  PVector value;
  
  Color(String name, PVector value) {
    this.name = name;
    this.value = value;
  }
}

// https://www.raylib.com/
Color raylib[] = {
  new Color("Light Gray", new PVector(200, 200, 200)),
  new Color("Gray", new PVector(130, 130, 130)),
  new Color("Dark Gray", new PVector(80, 80, 80)),
  new Color("Yellow", new PVector(253, 249, 0)),
  new Color("Gold", new PVector(255, 203, 0)),
  new Color("Orange", new PVector(255, 161, 0)),
  new Color("Pink", new PVector(255, 109, 194)),
  new Color("Red", new PVector(230, 41, 55)),
  new Color("Maroon", new PVector(190, 33, 55)),
  new Color("Green", new PVector(0, 228, 48)),
  new Color("Lime", new PVector(0, 158, 47)),
  new Color("Dark Green", new PVector(0, 117, 44)),
  new Color("Sky Blue", new PVector(102, 191, 255)),
  new Color("Blue", new PVector(0, 121, 241)),
  new Color("Dark Blue", new PVector(0, 82, 172)),
  new Color("Purple", new PVector(200, 122, 255)),
  new Color("Violet", new PVector(135, 60, 190)),
  new Color("Dark Purple", new PVector(112, 31, 126)),
  new Color("Beige", new PVector(211, 176, 131)),
  new Color("Brown", new PVector(127, 106, 79)),
  new Color("Dark Brown", new PVector(76, 63, 47)),
  new Color("White", new PVector(255, 255, 255)),
  new Color("Black", new PVector(0, 0, 0)),
  new Color("Magenta", new PVector(255, 0, 255)),
  new Color("Ray White", new PVector(245, 245, 245)),
};

// https://en.wikipedia.org/wiki/ColorChecker
Color macbeth[] = {
  new Color("Dark Skin", new PVector(115, 82, 68)),
  new Color("Light Skin", new PVector(194, 150, 130)),
  new Color("Blue Sky", new PVector(98, 122, 157)),
  new Color("Foliage", new PVector(87, 108, 67)),
  new Color("Blue Flower", new PVector(133, 128, 177)),
  new Color("Bluish Green", new PVector(103, 189, 170)),
  new Color("Orange", new PVector(214, 126, 44)),
  new Color("Purplish Blue", new PVector(80, 91, 166)),
  new Color("Moderate Red", new PVector(193, 90, 99)),
  new Color("Purple", new PVector(94, 60, 108)),
  new Color("Yellow Green", new PVector(157, 188, 64)),
  new Color("Orange Yellow", new PVector(224, 163, 46)),
  new Color("Blue", new PVector(56, 61, 150)),
  new Color("Green", new PVector(70, 148, 73)),
  new Color("Red", new PVector(175, 54, 60)),
  new Color("Yellow", new PVector(231, 199, 31)),
  new Color("Magenta", new PVector(187, 86, 149)),
  new Color("Cyan", new PVector(8, 133, 161)),
  new Color("White", new PVector(243, 243, 242)),
  new Color("Neutral 8", new PVector(200, 200, 200)),
  new Color("Neutral 6.5", new PVector(160, 160, 160)),
  new Color("Neutral 5", new PVector(122, 122, 122)),
  new Color("Neutral 3.5", new PVector(85, 85, 85)),
  new Color("Black", new PVector(52, 52, 52)),
};
