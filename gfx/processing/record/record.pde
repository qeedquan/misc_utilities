import processing.pdf.*;

boolean recording;

void setup() {
  size(1024, 768);
  recording = false;
}

void draw() {
  ellipse(mouseX, mouseY, 10, 10);
  square(mouseX+10, mouseY+10, 10);
}

void keyPressed() {
  if (key != ' ')
    return;
  
  if (!recording) {
    println("Begin Recording");
    beginRecord(PDF, "record.pdf");
  } else {
    println("End Recording");
    endRecord();
  }
  recording = !recording;
}
