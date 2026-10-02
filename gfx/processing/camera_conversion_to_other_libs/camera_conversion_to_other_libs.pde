/*

Processing uses a left-handed coordinate system by default with Y axis going down
Standard OpenGL libraries uses a right handed coordinate system with Y axis going up

*/

void setup() {
  size(600, 600, P3D);
}
 
void draw() {
  noFill();
  background(204);

  beginCamera();
  camera(width/2.0, height/2.0, (height/2.0) / tan(PI*30.0/180.0),  width/2.0, height/2.0, 0, 0, 1, 0);
  perspective(PI/3.0, width/height, 0.05, 4000);
  endCamera();
  translate(200, 200, 0);
  rotateX(radians(-45));
  rotateY(radians(60));
  box(150);
  translate(300, 0, 0);
  box(50);
  translate(0, 400, 0);
  rotateZ(radians(36));
  box(120);
}
