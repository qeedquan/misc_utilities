// https://processing.org/tutorials/pshader/

PShader toon;

void setup() {
  size(1024, 768, P3D);
  noStroke();
  fill(204);
  toon = loadShader("toon_frag.glsl", "toon_vert.glsl");
  toon.set("fraction", 1.0);
}

/*

The toon effect works by separating light intensity into sharp discrete
bands so we can see a cel shading effect

*/

void draw() {
  shader(toon);
  background(100, 100, 150);
  float dirY = (mouseY / float(height) - 0.5) * 2;
  float dirX = (mouseX / float(width) - 0.5) * 2;
  directionalLight(200, 200, 200, -dirX, -dirY, -1);
  translate(width/2, height/2);
  sphere(120);
}
