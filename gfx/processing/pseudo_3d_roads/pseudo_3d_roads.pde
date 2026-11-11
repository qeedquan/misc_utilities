/*

Ported from
https://codeincomplete.com/articles/javascript-racer-v1-straight/
https://codeincomplete.com/articles/javascript-racer-v2-curves/
https://codeincomplete.com/articles/javascript-racer-v3-hills/
https://codeincomplete.com/articles/javascript-racer-v4-final/
http://www.extentofthejam.com/pseudo/

*/

final int STRAIGHT = 0;
final int CURVE = 1;
final int HILL = 2;
final int FINAL = 3;

Assets assets;
Straight straight;
Curve curve;
Hill hill;
Final finals;
Sim sim;

void setup() {
  size(640, 480);
  frameRate(60);
  textSize(16);
  assets = new Assets();
  reset(FINAL);
}

void reset(int type) {
  switch (type) {
  case STRAIGHT:
    straight = new Straight(assets);
    sim = straight;
    break;
  case CURVE:
    curve = new Curve(assets);
    sim = curve;
    break;
  case HILL:
    hill = new Hill(assets);
    sim = hill;
    break;
  case FINAL:
    finals = new Final(assets);
    sim = finals;
    break;
  default:
    assert(false);
  }
  sim.loadSettings();
  sim.resetRoad();
  sim.resetTime();
}

void draw() {
  float now = millis();
  float dt = (now - sim.lastTime) / 1e3;
  sim.lastTime = now;
  sim.gdt += dt;
  while (sim.gdt > sim.step) {
    sim.update(sim.step);
    sim.gdt -= sim.step;
  }
  
  sim.frameCount++;
  sim.frameTime += dt;
  if (sim.frameTime >= 1) {
    sim.lastFrameCount = sim.frameCount;
    sim.frameTime = 0;
    sim.frameCount = 0;
  }
  
  background(0);
  sim.draw();
  push();
  fill(100, 100, 100);
  text(String.format("FPS: %d", sim.lastFrameCount), 32, 32);
  text(String.format("World: %s", sim.worldName()), 32, 56);
  pop();
}

void keyPressed() {
  if (key == '1')
    reset(STRAIGHT);
  else if (key == '2')
    reset(CURVE);
  else if (key == '3')
    reset(HILL);
  else if (key == '4')
    reset(FINAL);
  else
    sim.keyHandle(true);
}

void keyReleased() {
  sim.keyHandle(false);
}

int randomChoice(int[] choices) {
  return choices[(int)random(0, choices.length)];
}

float randomChoice(float[] choices) {
  return choices[(int)random(0, choices.length)];
}

int randomInt(int min, int max) {
  return round(random(min, max));
}

float limit(float x, float start, float end) {
  if (x < start)
    x = start;
  if (x > end)
    x = end;
  return x;
}

float increase(float start, float increment, float max) {
  float result = start + increment;
  while (result >= max)
    result -= max;
  while (result < 0)
    result += max;
  return result;
}

float accelerate(float v, float accel, float dt) {
  return v + (accel * dt);
}

float clamp(float x, float a, float b) {
  return max(a, min(x, b));
}

float interpolate(float a, float b, float percent) {
  return a + (b - a) * percent;
}

float exponentialFog(float distance, float density) {
  float E = 2.71828182845904523536028747135266249775724709369995;
  return 1 / (pow(E, (distance * distance * density)));
}

float percentRemaining(float n, float total) {
  return (n % total) / total;
}

float easeIn(float a, float b, float percent) {
  return a + (b - a) * pow(percent, 2);
}

float easeOut(float a, float b, float percent) {
  return a + (b - a) * (1 - pow(1 - percent, 2));
}

float easeInOut(float a, float b, float percent) {
  return a + (b - a) * ((-cos(percent * PI) / 2) + 0.5);
}

void project(Waypoint p, PVector camera, float cameraDepth, float width, float height, float roadWidth) {
  p.camera = PVector.sub(p.world, camera);
  p.screenScale = cameraDepth / p.camera.z;
  p.screen.x = round((width/2)  + (p.screenScale * p.camera.x  * width/2));
  p.screen.y = round((height/2) - (p.screenScale * p.camera.y  * height/2));
  p.screen.w = round(             (p.screenScale * roadWidth   * width/2));
}

boolean overlap(float x1, float w1, float x2, float w2, float percent) {
  float half = percent / 2;
  float min1 = x1 - (w1*half);
  float max1 = x1 + (w1*half);
  float min2 = x2 - (w2*half);
  float max2 = x2 + (w2*half);
  return !((max1 < min2) || (min1 > max2));
}

class Rect {
  float x, y, w, h;
  
  Rect() {
    x = y = w = h = 0;
  }
  
  Rect(float x, float y, float w, float h) {
    this.x = x;
    this.y = y;
    this.w = w;
    this.h = h;
  }
  
  String toString() {
    return String.format("Rect(%.3f %.3f %.3f %.3f)", x, y, w, h);
  }
}

class Assets {
  PImage background;
  PImage sprites;
  
  Assets() {
    background = loadImage("images/background.png");
    sprites = loadImage("images/sprites.png");
  }
}

class Sprite {
  Rect source;
  float offset;
  
  Sprite(Rect source, float offset) {
    this.source = source;
    this.offset = offset;
  }
}

class Car {
  float offset;
  float z;
  Rect sprite;
  float speed;
  float percent;
  
  Car(float offset, float z, Rect sprite, float speed) {
    this.offset = offset;
    this.z = z;
    this.sprite = sprite;
    this.speed = speed;
    this.percent = 0;
  }
}

class Waypoint {
  PVector camera;
  PVector world;
  Rect screen;
  float screenScale;
  
  Waypoint() {
    camera = new PVector();
    world = new PVector();
    screen = new Rect();
    screenScale = 0;
  }
}

class Segment {
  int index;
  Waypoint p1;
  Waypoint p2;
  boolean looped;
  float fog;
  float curve;
  float clip;
  color[] col;
  ArrayList<Car> cars;
  ArrayList<Sprite> sprites;
  
  Segment() {
    p1 = new Waypoint();
    p2 = new Waypoint();
    cars = new ArrayList<Car>();
    sprites = new ArrayList<Sprite>();
  }
}

class Render {
  float rumbleWidth(float projectedRoadWidth, float lanes) {
    return projectedRoadWidth/max(6, 2*lanes); 
  }
  
  float laneMarkerWidth(float projectedRoadWidth, float lanes) {
    return projectedRoadWidth/max(32, 8*lanes);
  }
  
  void polygon(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, color col) {
    push();
    noStroke();
    fill(col);
    beginShape();
    vertex(x1, y1);
    vertex(x2, y2);
    vertex(x3, y3);
    vertex(x4, y4);
    endShape();
    pop();
  }
  
  void background(PImage image, float width, float height, Rect layer, float rotation, float offset) {
    float iw = layer.w/2;
    float ih = layer.h;

    float sx = layer.x + floor(layer.w*rotation);
    float sy = layer.y + offset;
    float sw = min(iw, layer.x+layer.w-sx);
    float sh = ih;
    PImage sp = image.get((int)sx, (int)sy, (int)sw, (int)sh);

    float dx = 0;
    float dy = 0;
    float dw = floor(width * (sw/iw));
    float dh = height;

    image(sp, dx, dy, dw, dh);
    if (sw < iw) {
      sp = image.get((int)layer.x, (int)sy, (int)(iw-sw), (int)sh);
      image(sp, dw-1, dy, width-dw, dh);
    }
  }
  
  void segment(float width, float lanes, float x1, float y1, float w1, float x2, float y2, float w2, float fog, color[] col) {
    float r1 = rumbleWidth(w1, lanes);
    float r2 = rumbleWidth(w2, lanes);
    float l1 = laneMarkerWidth(w1, lanes);
    float l2 = laneMarkerWidth(w2, lanes);
    
    push();
    noStroke();
    fill(col[GRASS]);
    rect(0, y2, width, y1-y2);
    
    polygon(x1-w1-r1, y1, x1-w1, y1, x2-w2, y2, x2-w2-r2, y2, col[RUMBLE]);
    polygon(x1+w1+r1, y1, x1+w1, y1, x2+w2, y2, x2+w2+r2, y2, col[RUMBLE]);
    polygon(x1-w1,    y1, x1+w1, y1, x2+w2, y2, x2-w2,    y2, col[ROAD]);
    
    if (LANE < col.length) {
      float lanew1 = w1 * 2/lanes;
      float lanew2 = w2 * 2/lanes;
      float lanex1 = x1 - w1 + lanew1;
      float lanex2 = x2 - w2 + lanew2;
      for (int lane = 1; lane < lanes; lane++) {
         polygon(lanex1 - l1 / 2, y1, lanex1 + l1 / 2, y1, lanex2 + l2 / 2, y2, lanex2 - l2 / 2, y2, col[LANE]);
         lanex1 += lanew1;
         lanex2 += lanew2;
      }
    }

    fog(0, y1, width, y2-y1, fog);
    pop();
  }
  
  void sprite(float width, float height, float resolution, float roadWidth, PImage sprites, Rect sprite, float scale, float destX, float destY, float offsetX, float offsetY, float clipY, float spriteScale) {
    float destW = (sprite.w * scale * width/2) * (spriteScale * roadWidth);
    float destH = (sprite.h * scale * width/2) * (spriteScale * roadWidth);

    destX += destW * offsetX;
    destY += destH * offsetY;
    
    float clipH = 0;
    if (clipY != 0)
      clipH = max(0, destY+destH-clipY);
    if (clipH < destH) {
      PImage sp = sprites.get((int)sprite.x, (int)sprite.y, (int)sprite.w, (int)(sprite.h-(sprite.h*clipH/destH)));
      image(sp, destX, destY, destW, destH-clipH);
    }
  }
  
  void player(float width, float height, float resolution, float roadWidth, PImage sprites, float speedPercent, float scale, float destX, float destY, float steer, float updown, float spriteScale) {
    float bounce = (1.5 * random(1.0) * speedPercent * resolution) * (((((int)random(100))&1) != 0) ? 1 : -1);
    Rect sprite;
    
    if (steer < 0)
      sprite = (updown > 0) ? SPRITES[PLAYER_UPHILL_LEFT] : SPRITES[PLAYER_LEFT];
    else if (steer > 0)
      sprite = (updown > 0) ? SPRITES[PLAYER_UPHILL_RIGHT] : SPRITES[PLAYER_RIGHT];
    else
      sprite = (updown > 0) ? SPRITES[PLAYER_UPHILL_STRAIGHT] : SPRITES[PLAYER_STRAIGHT];

    sprite(width, height, resolution, roadWidth, sprites, sprite, scale, destX, destY + bounce, -0.5, -1, 0, spriteScale);
  }
  
  void fog(float x, float y, float width, float height, float fog) {
    if (fog >= 1)
      return;

    color col = COLORS[FOG];
    push();
    fill((col>>16)&0xff, (col>>8)&0xff, (col>>0)&0xff, 50*fog);
    rect(x, y, width, height);
    pop();
  }
}

abstract class Sim {  
  int lastFrameCount;
  int frameCount;
  float frameTime;

  float fps;
  float step;
  float gdt;
  float lastTime;

  PImage background;
  PImage sprites;
  Render render;
  
  // all the cars
  Car[] cars;

  // total number of cars on the road
  int totalCars;

  // road segments
  Segment[] segments;
  ArrayList<Segment> seglist;

  // length of a single segment
  int segmentLength;
  
  // length of track (all segments)
  int trackLength;

  // number of segments to draw
  float drawDistance;

  // number of segments per red/white rumble strip
  float rumbleLength;
  
  // actually half the roads width, easier math if the road spans from -roadWidth to +roadWidth
  float roadWidth;

  // number of lanes
  float lanes;
  
  // field of view
  float fieldOfView;
  
  // camera height
  float cameraHeight;
  
  // z distance camera is from screen (computed)
  float cameraDepth;
  
  // scaling factor to provide resolution independence (computed)
  float resolution;
  
  // current camera Z position (add playerZ to get player's absolute Z position)
  float position;
  
  // player x offset from center of road (-1 to 1 to stay independent of roadWidth
  float playerX;

  // player relative z distance from camera (computed)
  float playerZ;
  
  // current speed
  float speed;
  
  // top speed (ensure we can't move more than 1 segment in a single frame to make collision detection easier)
  float maxSpeed;

  // acceleration rate - tuned until it 'felt' right
  float accel;
  
  // deceleration rate when braking
  float breaking;

  // 'natural' deceleration rate when neither accelerating, nor braking
  float decel;
  
  // off road deceleration is somewhere in between
  float offRoadDecel;

  // limit when off road deceleration no longer applies (e.g. you can always go at least this speed even when off road)
  float offRoadLimit;
  
  // exponential fog density
  float fogDensity;
  
  // the reference sprite width should be 1/3rd the (half-)roadWidth
  float spriteScale;
  
  // centrifugal force multiplier when going around curves
  float centrifugal;
  
  // current sky scroll offset
  float skyOffset;
  
  // current hill scroll offset
  float hillOffset;
  
  // current tree scroll offset
  float treeOffset;
  
  // background sky layer scroll speed when going around curve (or up hill)
  float skySpeed;
  
  // background hill layer scroll speed when going around curve (or up hill)
  float hillSpeed;
  
  // background tree layer scroll speed when going around curve (or up hill)
  float treeSpeed;
  
  // key input
  boolean keyLeft;
  boolean keyRight;
  boolean keyFaster;
  boolean keySlower;

  Sim(Assets assets) {
    background = assets.background;
    sprites = assets.sprites;
    render = new Render();
    defaultSettings();
  }
  
  void defaultSettings() {
    fps = 60;
    step = 1/fps;

    seglist = new ArrayList<Segment>();
    segmentLength = 200;
    drawDistance = 300;
    roadWidth = 2000;
    rumbleLength = 3;
    lanes = 3;
    fieldOfView = 100;
    cameraHeight = 1000;
    fogDensity = 5;
    playerX = 0;
    playerZ = 0;
    speed = 0;
    maxSpeed = segmentLength / step;
    accel = maxSpeed / 5;
    breaking = -maxSpeed;
    decel = -maxSpeed / 5;
    offRoadDecel = -maxSpeed / 2;
    offRoadLimit = maxSpeed / 4;
    
    keyLeft = false;
    keyRight = false;
    keyFaster = false;
    keySlower = false;
    spriteScale = 0.3 / SPRITES[PLAYER_STRAIGHT].w;
    
    centrifugal = 0.3;
    skyOffset = 0;
    hillOffset = 0;
    treeOffset = 0;
    skySpeed = 0.001;
    hillSpeed = 0.002;
    treeSpeed = 0.003;
    
    totalCars = 200;
  }
  
  void loadSettings() {
    cameraDepth = 1 / tan(radians(fieldOfView / 2));
    playerZ = cameraHeight * cameraDepth;
    resolution = height / 480.0;
  }
  
  void resetTime() {
    gdt = 0;
    lastTime = millis();
    frameTime = 0;
    lastFrameCount = 0;
    frameCount = 0;
  }

  Segment findSegment(float z) {
    return segments[floor(z/segmentLength) % segments.length];
  }
  
  float lastY() {
    if (seglist.size() == 0)
      return 0;
    
    Segment sg = seglist.get(seglist.size() - 1);
    return sg.p2.world.y;
  }
  
  void keyHandle(boolean pressed) {
    if ((key == CODED && keyCode == LEFT) || key == 'a') {
      keyLeft = pressed;
    } else if ((key == CODED && keyCode == RIGHT) || key == 'd') {
      keyRight = pressed;
    } else if ((key == CODED && keyCode == UP) || key == 'w') {
      keyFaster = pressed;
    } else if ((key == CODED && keyCode == DOWN) || key == 's') {
      keySlower = pressed;
    }
  }
  
  abstract void resetRoad();
  abstract void update(float dt);
  abstract void draw();
  abstract String worldName();
}

class Straight extends Sim {
  Straight(Assets assets) {
    super(assets);
  }

  void resetRoad() {
    segments = new Segment[500];
    for (int i = 0; i < segments.length; i++) {
      Segment sg = new Segment();
      sg.index = i;
      sg.p1.world.z = i*segmentLength;
      sg.p2.world.z = (i+1)*segmentLength;
      sg.col = (floor(i/rumbleLength)%2 != 0) ? LIGHT : DARK;
      segments[i] = sg;
    }
    segments[findSegment(playerZ).index + 2].col = START;
    segments[findSegment(playerZ).index + 3].col = START;
    for(int i = 0; i < rumbleLength; i++)
      segments[segments.length-1-i].col = FINISH;
    
    trackLength = segments.length * segmentLength;
  }

  void draw() {    
    render.background(background, width, height, BACKGROUND[SKY], 0, 0);
    render.background(background, width, height, BACKGROUND[HILLS], 0, 0);
    render.background(background, width, height, BACKGROUND[TREES], 0, 0);
    
    Segment bsg = findSegment(position);
    float maxy = height;
    for (int i = 0; i < drawDistance; i++) {
      Segment sg = segments[(bsg.index + i) % segments.length];
      sg.looped = (sg.index < bsg.index);
      sg.fog = exponentialFog(i/drawDistance, fogDensity);
      
      project(sg.p1, new PVector(playerX*roadWidth, cameraHeight, position - (sg.looped ? trackLength : 0)), cameraDepth, width, height, roadWidth);
      project(sg.p2, new PVector(playerX*roadWidth, cameraHeight, position - (sg.looped ? trackLength : 0)), cameraDepth, width, height, roadWidth);

      // don't draw if behind or already clipped
      if ((sg.p1.camera.z <= cameraDepth) || (sg.p2.screen.y >= maxy))
        continue;

      render.segment(width, lanes, sg.p1.screen.x, sg.p1.screen.y, sg.p1.screen.w, sg.p2.screen.x, sg.p2.screen.y, sg.p2.screen.w, sg.fog, sg.col);
      maxy = sg.p2.screen.y;
    }
    
    render.player(width, height, resolution, roadWidth, sprites, speed/maxSpeed, cameraDepth/playerZ, width/2, height, speed * (keyLeft ? -1 : keyRight ? 1 : 0), 0, spriteScale);
  }
  
  void update(float dt) {
    position = increase(position, dt * speed, trackLength);

    // at top speed, should be able to cross from left to right (-1 to 1) in 1 second
    float dx = dt * 2 * (speed / maxSpeed); 
    if (keyLeft)
      playerX -= dx;
    else if (keyRight)
      playerX += dx;

    if (keyFaster)
      speed = accelerate(speed, accel, dt);
    else if (keySlower)
      speed = accelerate(speed, breaking, dt);
    else
      speed = accelerate(speed, decel, dt);

    if (((playerX < -1) || (playerX > 1)) && (speed > offRoadLimit))
      speed = accelerate(speed, offRoadDecel, dt);

    // dont ever let player go too far out of bounds
    // or exceed maxSpeed
    playerX = clamp(playerX, -2, 2);     
    speed = clamp(speed, 0, maxSpeed);
  }
  
  String worldName() {
    return "Straight";
  }
}

class Curve extends Sim {
  final int ROAD_LENGTH_NONE = 0;
  final int ROAD_LENGTH_SHORT = 25;
  final int ROAD_LENGTH_MEDIUM = 50;
  final int ROAD_LENGTH_LONG = 100;
  
  final int ROAD_CURVE_NONE = 0;
  final int ROAD_CURVE_EASY = 2;
  final int ROAD_CURVE_MEDIUM = 4;
  final int ROAD_CURVE_HARD = 6;

  Curve(Assets assets) {
    super(assets);
  }
  
  void addSegment(float curve) {
    int n = seglist.size();
    Segment sg = new Segment();
    sg.index = n;
    sg.p1.world.z = n*segmentLength;
    sg.p2.world.z = (n+1)*segmentLength;
    sg.curve = curve;
    sg.col = (floor(n/rumbleLength)%2 != 0) ? LIGHT : DARK;
    seglist.add(sg);
  }

  void addRoad(int enter, int hold, int leave, float curve) {
    for (int n = 0; n < enter; n++)
      addSegment(easeIn(0, curve, n/enter));
    for (int n = 0; n < hold; n++)
      addSegment(curve);
    for (int n = 0; n < leave; n++)
      addSegment(easeInOut(curve, 0, n/leave));
  }
  
  void addStraight(int num) {
    addRoad(num, num, num, 0);
  }

  void addCurve(int num, float curve) {
    addRoad(num, num, num, curve);
  }
  
  void addSCurves() {
    addRoad(ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM,  -ROAD_CURVE_EASY);
    addRoad(ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM,   ROAD_CURVE_MEDIUM);
    addRoad(ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM,   ROAD_CURVE_EASY);
    addRoad(ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM,  -ROAD_CURVE_EASY);
    addRoad(ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM,  -ROAD_CURVE_MEDIUM);
  }
  
  void resetRoad() {
    seglist.clear();
    addStraight(ROAD_LENGTH_SHORT/4);
    addSCurves();
    addStraight(ROAD_LENGTH_LONG);
    addCurve(ROAD_LENGTH_MEDIUM, ROAD_CURVE_MEDIUM);
    addCurve(ROAD_LENGTH_LONG, ROAD_CURVE_MEDIUM);
    addStraight(ROAD_LENGTH_MEDIUM);
    addSCurves();
    addCurve(ROAD_LENGTH_LONG, -ROAD_CURVE_MEDIUM);
    addCurve(ROAD_LENGTH_LONG, ROAD_CURVE_MEDIUM);
    addStraight(ROAD_LENGTH_MEDIUM);
    addSCurves();
    addCurve(ROAD_LENGTH_LONG, -ROAD_CURVE_EASY);

    segments = new Segment[seglist.size()];
    seglist.toArray(segments);

    segments[findSegment(playerZ).index + 2].col = START;
    segments[findSegment(playerZ).index + 3].col = START;
    for(int i = 0; i < rumbleLength; i++)
      segments[segments.length-1-i].col = FINISH;
    
    trackLength = segments.length * segmentLength;
  }
  
  void update(float dt) {
    Segment playerSegment = findSegment(position+playerZ);
    float speedPercent = speed/maxSpeed;
    float dx = dt * 2 * speedPercent;

    position = increase(position, dt * speed, trackLength);

    skyOffset  = increase(skyOffset,  skySpeed  * playerSegment.curve * speedPercent, 1);
    hillOffset = increase(hillOffset, hillSpeed * playerSegment.curve * speedPercent, 1);
    treeOffset = increase(treeOffset, treeSpeed * playerSegment.curve * speedPercent, 1);

    if (keyLeft)
      playerX = playerX - dx;
    else if (keyRight)
      playerX = playerX + dx;

    playerX = playerX - (dx * speedPercent * playerSegment.curve * centrifugal);

    if (keyFaster)
      speed = accelerate(speed, accel, dt);
    else if (keySlower)
      speed = accelerate(speed, breaking, dt);
    else
      speed = accelerate(speed, decel, dt);

    if (((playerX < -1) || (playerX > 1)) && (speed > offRoadLimit))
      speed = accelerate(speed, offRoadDecel, dt);

    // dont ever let player go too far out of bounds
    playerX = clamp(playerX, -2, 2);     
    speed = clamp(speed, 0, maxSpeed);
  }
  
  void draw() {
    Segment baseSegment = findSegment(position);
    float basePercent = percentRemaining(position, segmentLength);
    float maxy = height;

    float x = 0;
    float dx = -(baseSegment.curve * basePercent);

    render.background(background, width, height, BACKGROUND[SKY], 0, skyOffset);
    render.background(background, width, height, BACKGROUND[HILLS], 0, hillOffset);
    render.background(background, width, height, BACKGROUND[TREES], 0, treeOffset);

    for (int n = 0 ; n < drawDistance ; n++) {
      Segment segment = segments[(baseSegment.index + n) % segments.length];
      segment.looped = segment.index < baseSegment.index;
      segment.fog = exponentialFog(n/drawDistance, fogDensity);

      project(segment.p1, new PVector((playerX * roadWidth) - x,      cameraHeight, position - (segment.looped ? trackLength : 0)), cameraDepth, width, height, roadWidth);
      project(segment.p2, new PVector((playerX * roadWidth) - x - dx, cameraHeight, position - (segment.looped ? trackLength : 0)), cameraDepth, width, height, roadWidth);

      x = x + dx;
      dx = dx + segment.curve;

      if ((segment.p1.camera.z <= cameraDepth) || (segment.p2.screen.y >= maxy))
        continue;

      render.segment(width, lanes, segment.p1.screen.x, segment.p1.screen.y, segment.p1.screen.w, segment.p2.screen.x, segment.p2.screen.y, segment.p2.screen.w, segment.fog, segment.col);
      maxy = segment.p2.screen.y;
    }

    render.player(width, height, resolution, roadWidth, sprites, speed/maxSpeed, cameraDepth/playerZ, width/2, height, speed * (keyLeft ? -1 : keyRight ? 1 : 0), 0, spriteScale);
  }
  
  String worldName() {
    return "Curvy";
  }
}

class Hill extends Sim {
  final int ROAD_LENGTH_NONE = 0;
  final int ROAD_LENGTH_SHORT = 25;
  final int ROAD_LENGTH_MEDIUM = 50;
  final int ROAD_LENGTH_LONG = 100;
  
  final int ROAD_HILL_NONE = 0;
  final int ROAD_HILL_LOW = 20;
  final int ROAD_HILL_MEDIUM = 40;
  final int ROAD_HILL_HIGH = 60;
  
  final int ROAD_CURVE_NONE = 0;
  final int ROAD_CURVE_EASY = 2;
  final int ROAD_CURVE_MEDIUM = 4;
  final int ROAD_CURVE_HARD = 6;
  
  Hill(Assets assets) {
    super(assets);
  }

  void addSegment(float curve, float y) {
    int n = seglist.size();
    Segment sg = new Segment();
    sg.index = n;
    sg.p1.world.y = lastY();
    sg.p1.world.z = n*segmentLength;
    sg.p2.world.y = y;
    sg.p2.world.z = (n+1)*segmentLength;
    sg.curve = curve;
    sg.col = ((int(n/rumbleLength)%2) != 0) ? DARK : LIGHT;
    seglist.add(sg);
  }

  void addRoad(int enter, int hold, int leave, float curve, float y) {
    float startY = lastY();
    float endY = startY + (floor(y) * segmentLength);

    int total = enter + hold + leave;
    for (int n = 0; n < enter; n++)
      addSegment(easeIn(0, curve, n*1.0/enter), easeInOut(startY, endY, n*1.0/total));
    for (int n = 0 ; n < hold; n++)
      addSegment(curve, easeInOut(startY, endY, (enter+n)*1.0/total));
    for (int n = 0 ; n < leave; n++)
      addSegment(easeInOut(curve, 0, n*1.0/leave), easeInOut(startY, endY, (enter+hold+n)*1.0/total));
  }

  void addStraight(int num) {
    addRoad(num, num, num, 0, 0);
  }

  void addHill(int num, int height) {
    addRoad(num, num, num, 0, height);
  }

  void addCurve(int num, float curve, int height) {
    addRoad(num, num, num, curve, height);
  }
        
  void addLowRollingHills(int num, int height) {
    addRoad(num, num, num,  0,  height/2);
    addRoad(num, num, num,  0, -height);
    addRoad(num, num, num,  0,  height);
    addRoad(num, num, num,  0,  0);
    addRoad(num, num, num,  0,  height/2);
    addRoad(num, num, num,  0,  0);
  }

  void addSCurves() {
    addRoad(ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM,  -ROAD_CURVE_EASY,    ROAD_HILL_NONE);
    addRoad(ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM,   ROAD_CURVE_MEDIUM,  ROAD_HILL_MEDIUM);
    addRoad(ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM,   ROAD_CURVE_EASY,   -ROAD_HILL_LOW);
    addRoad(ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM,  -ROAD_CURVE_EASY,    ROAD_HILL_MEDIUM);
    addRoad(ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM,  -ROAD_CURVE_MEDIUM, -ROAD_HILL_MEDIUM);
  }

  void addDownhillToEnd(int num) {
    addRoad(num, num, num, -ROAD_CURVE_EASY, -lastY()/segmentLength);
  }
  
  void resetRoad() {
    seglist.clear();

    addStraight(ROAD_LENGTH_SHORT/2);
    addHill(ROAD_LENGTH_SHORT, ROAD_HILL_LOW);
    addLowRollingHills(ROAD_LENGTH_SHORT, ROAD_HILL_LOW);
    addCurve(ROAD_LENGTH_MEDIUM, ROAD_CURVE_MEDIUM, ROAD_HILL_LOW);
    addLowRollingHills(ROAD_LENGTH_SHORT, ROAD_HILL_LOW);
    addCurve(ROAD_LENGTH_LONG, ROAD_CURVE_MEDIUM, ROAD_HILL_MEDIUM);
    addStraight(ROAD_LENGTH_MEDIUM);
    addCurve(ROAD_LENGTH_LONG, -ROAD_CURVE_MEDIUM, ROAD_HILL_MEDIUM);
    addHill(ROAD_LENGTH_LONG, ROAD_HILL_HIGH);
    addCurve(ROAD_LENGTH_LONG, ROAD_CURVE_MEDIUM, -ROAD_HILL_LOW);
    addHill(ROAD_LENGTH_LONG, -ROAD_HILL_MEDIUM);
    addStraight(ROAD_LENGTH_MEDIUM);
    addDownhillToEnd(200);

    segments = new Segment[seglist.size()];
    seglist.toArray(segments);

    segments[findSegment(playerZ).index + 2].col = START;
    segments[findSegment(playerZ).index + 3].col = START;
    for(int i = 0; i < rumbleLength; i++)
      segments[segments.length-1-i].col = FINISH;
    
    trackLength = segments.length * segmentLength;
  }
  
  void update(float dt) {
    Segment playerSegment = findSegment(position+playerZ);
    float speedPercent  = speed/maxSpeed;
    float dx = dt * 2 * speedPercent;

    position = increase(position, dt * speed, trackLength);

    skyOffset  = increase(skyOffset,  skySpeed  * playerSegment.curve * speedPercent, 1);
    hillOffset = increase(hillOffset, hillSpeed * playerSegment.curve * speedPercent, 1);
    treeOffset = increase(treeOffset, treeSpeed * playerSegment.curve * speedPercent, 1);

    if (keyLeft)
      playerX = playerX - dx;
    else if (keyRight)
      playerX = playerX + dx;

    playerX = playerX - (dx * speedPercent * playerSegment.curve * centrifugal);

    if (keyFaster)
      speed = accelerate(speed, accel, dt);
    else if (keySlower)
      speed = accelerate(speed, breaking, dt);
    else
      speed = accelerate(speed, decel, dt);

    if (((playerX < -1) || (playerX > 1)) && (speed > offRoadLimit))
      speed = accelerate(speed, offRoadDecel, dt);

    playerX = clamp(playerX, -2, 2);
    speed = clamp(speed, 0, maxSpeed);
  }

  void draw() {
    Segment baseSegment = findSegment(position);
    float basePercent = percentRemaining(position, segmentLength);
    Segment playerSegment = findSegment(position+playerZ);
    float playerPercent = percentRemaining(position+playerZ, segmentLength);
    float playerY = interpolate(playerSegment.p1.world.y, playerSegment.p2.world.y, playerPercent);
    float maxy = height;

    float x = 0;
    float dx = -(baseSegment.curve * basePercent);
    
    render.background(background, width, height, BACKGROUND[SKY], skyOffset, resolution * skySpeed  * playerY);
    render.background(background, width, height, BACKGROUND[HILLS], hillOffset, resolution * hillSpeed * playerY);
    render.background(background, width, height, BACKGROUND[TREES], treeOffset, resolution * treeSpeed * playerY);

    for (int n = 0; n < drawDistance; n++) {
      Segment segment = segments[(baseSegment.index + n) % segments.length];
      segment.looped = segment.index < baseSegment.index;
      segment.fog = exponentialFog(n/drawDistance, fogDensity);

      project(segment.p1, new PVector((playerX * roadWidth) - x,      playerY + cameraHeight, position - (segment.looped ? trackLength : 0)), cameraDepth, width, height, roadWidth);
      project(segment.p2, new PVector((playerX * roadWidth) - x - dx, playerY + cameraHeight, position - (segment.looped ? trackLength : 0)), cameraDepth, width, height, roadWidth);

      x = x + dx;
      dx = dx + segment.curve;

      if ((segment.p1.camera.z <= cameraDepth) || (segment.p2.screen.y >= segment.p1.screen.y) || (segment.p2.screen.y >= maxy))
          continue;

      render.segment(width, lanes, segment.p1.screen.x, segment.p1.screen.y, segment.p1.screen.w, segment.p2.screen.x, segment.p2.screen.y, segment.p2.screen.w, segment.fog, segment.col);

      maxy = segment.p2.screen.y;
    }

    render.player(width, height, resolution, roadWidth, sprites, speed/maxSpeed,
                  cameraDepth/playerZ,
                  width/2,
                  (height/2) - (cameraDepth/playerZ * interpolate(playerSegment.p1.camera.y, playerSegment.p2.camera.y, playerPercent) * height/2),
                  speed * (keyLeft ? -1 : keyRight ? 1 : 0),
                  playerSegment.p2.world.y - playerSegment.p1.world.y,
                  spriteScale);
  }
  
  String worldName() {
    return "Hilly";
  }
}

class Final extends Sim {
  final int ROAD_LENGTH_NONE = 0;
  final int ROAD_LENGTH_SHORT = 25;
  final int ROAD_LENGTH_MEDIUM = 50;
  final int ROAD_LENGTH_LONG = 100;
  
  final int ROAD_HILL_NONE = 0;
  final int ROAD_HILL_LOW = 20;
  final int ROAD_HILL_MEDIUM = 40;
  final int ROAD_HILL_HIGH = 60;
  
  final int ROAD_CURVE_NONE = 0;
  final int ROAD_CURVE_EASY = 2;
  final int ROAD_CURVE_MEDIUM = 4;
  final int ROAD_CURVE_HARD = 6;
  
  Final(Assets assets) {
    super(assets);
  }
  
  void addSprite(int n, int sprite, float offset) {
    segments[n].sprites.add(new Sprite(SPRITES[sprite], offset));
  }
  
  void addSegment(float curve, float y) {
    int n = seglist.size();
    Segment sg = new Segment();
    sg.index = n;
    sg.p1.world.y = lastY();
    sg.p1.world.z = n*segmentLength;
    sg.p2.world.y = y;
    sg.p2.world.z = (n+1)*segmentLength;
    sg.curve = curve;
    sg.col = ((int(n/rumbleLength)%2) != 0) ? DARK : LIGHT;
    seglist.add(sg);
  }

  void addRoad(int enter, int hold, int leave, float curve, float y) {
    float startY = lastY();
    float endY = startY + (floor(y) * segmentLength);

    int total = enter + hold + leave;
    for (int n = 0; n < enter; n++)
      addSegment(easeIn(0, curve, n*1.0/enter), easeInOut(startY, endY, n*1.0/total));
    for (int n = 0 ; n < hold; n++)
      addSegment(curve, easeInOut(startY, endY, (enter+n)*1.0/total));
    for (int n = 0 ; n < leave; n++)
      addSegment(easeInOut(curve, 0, n*1.0/leave), easeInOut(startY, endY, (enter+hold+n)*1.0/total));
  }
  
  void addStraight(int num) {
    addRoad(num, num, num, 0, 0);
  }

  void addHill(int num, int height) {
    addRoad(num, num, num, 0, height);
  }

  void addCurve(int num, float curve, int height) {
    addRoad(num, num, num, curve, height);
  }
        
  void addLowRollingHills(int num, int height) {
    addRoad(num, num, num,  0,                height/2);
    addRoad(num, num, num,  0,               -height);
    addRoad(num, num, num,  ROAD_CURVE_EASY,  height);
    addRoad(num, num, num,  0,                0);
    addRoad(num, num, num, -ROAD_CURVE_EASY,  height/2);
    addRoad(num, num, num,  0,                0);
  }

  void addSCurves() {
    addRoad(ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM, -ROAD_CURVE_EASY,    ROAD_HILL_NONE);
    addRoad(ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM,  ROAD_CURVE_MEDIUM,  ROAD_HILL_MEDIUM);
    addRoad(ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM,  ROAD_CURVE_EASY,   -ROAD_HILL_LOW);
    addRoad(ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM, -ROAD_CURVE_EASY,    ROAD_HILL_MEDIUM);
    addRoad(ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM, ROAD_LENGTH_MEDIUM, -ROAD_CURVE_MEDIUM, -ROAD_HILL_MEDIUM);
  }

  void addBumps() {
    addRoad(10, 10, 10, 0,  5);
    addRoad(10, 10, 10, 0, -2);
    addRoad(10, 10, 10, 0, -5);
    addRoad(10, 10, 10, 0,  8);
    addRoad(10, 10, 10, 0,  5);
    addRoad(10, 10, 10, 0, -7);
    addRoad(10, 10, 10, 0,  5);
    addRoad(10, 10, 10, 0, -2);
  }

  void addDownhillToEnd(int num) {
    addRoad(num, num, num, -ROAD_CURVE_EASY, -lastY()/segmentLength);
  }
  
  void resetSprites() {
    addSprite(20,  BILLBOARD07, -1);
    addSprite(40,  BILLBOARD06, -1);
    addSprite(60,  BILLBOARD08, -1);
    addSprite(80,  BILLBOARD09, -1);
    addSprite(100, BILLBOARD01, -1);
    addSprite(120, BILLBOARD02, -1);
    addSprite(140, BILLBOARD03, -1);
    addSprite(160, BILLBOARD04, -1);
    addSprite(180, BILLBOARD05, -1);

    addSprite(240,                  BILLBOARD07, -1.2);
    addSprite(240,                  BILLBOARD06,  1.2);
    addSprite(segments.length - 25, BILLBOARD07, -1.2);
    addSprite(segments.length - 25, BILLBOARD06,  1.2);
    
    for (int n = 10; n < 200; n += 4 + n/100) {
      addSprite(n, PALM_TREE, 0.5 + random(1)*0.5);
      addSprite(n, PALM_TREE,   1 + random(1)*2);
    }
    
    for (int n = 250; n < 1000; n += 5) {
      addSprite(n, COLUMN, 1.1);
      addSprite(n + randomInt(0, 5), TREE1, -1 - (random(1) * 2));
      addSprite(n + randomInt(0, 5), TREE2, -1 - (random(1) * 2));
    }
    
    for (int n = 200; n < segments.length; n += 3) {
      addSprite(n, randomChoice(PLANTS), randomChoice(new int[]{1,-1}) * (2 + random(1) * 5));
    }

    for (int n = 1000; n < (segments.length-50); n += 100) {
      int side = randomChoice(new int[]{1, -1});
      addSprite(n + randomInt(0, 50), randomChoice(BILLBOARDS), -side);
      for (int i = 0; i < 20 ; i++) {
        int sprite = randomChoice(PLANTS);
        float offset = side * (1.5 + random(1));
        addSprite(n + randomInt(0, 50), sprite, offset);
      }
    }     
  }
  
  void resetCars() {
    cars = new Car[totalCars];
    for (int n = 0; n < totalCars; n++) {
      float offset = random(1) * randomChoice(new float[]{-0.8, 0.8});
      float z = floor(random(1) * segments.length) * segmentLength;
      int sprite = randomChoice(CARS);
      float speed = maxSpeed/4 + random(1) * maxSpeed/(sprite == SEMI ? 4 : 2);
      Car car = new Car(offset, z, SPRITES[sprite], speed);
      Segment sg = findSegment(car.z);
      sg.cars.add(car);
      cars[n] = car;
    }
  }

  void resetRoad() {
    seglist.clear();

    addStraight(ROAD_LENGTH_SHORT);
    addLowRollingHills(ROAD_LENGTH_SHORT, ROAD_HILL_LOW);
    addSCurves();
    addCurve(ROAD_LENGTH_MEDIUM, ROAD_CURVE_MEDIUM, ROAD_HILL_LOW); 
    addBumps();
    addLowRollingHills(ROAD_LENGTH_SHORT, ROAD_HILL_LOW);
    addCurve(ROAD_LENGTH_LONG*2, ROAD_CURVE_MEDIUM, ROAD_HILL_MEDIUM);
    addStraight(ROAD_LENGTH_MEDIUM);
    addHill(ROAD_LENGTH_MEDIUM, ROAD_HILL_HIGH);
    addSCurves();
    addCurve(ROAD_LENGTH_LONG, -ROAD_CURVE_MEDIUM, ROAD_HILL_NONE);
    addHill(ROAD_LENGTH_LONG, ROAD_HILL_HIGH);
    addCurve(ROAD_LENGTH_LONG, ROAD_CURVE_MEDIUM, -ROAD_HILL_LOW);
    addBumps();
    addHill(ROAD_LENGTH_LONG, -ROAD_HILL_MEDIUM);
    addStraight(ROAD_LENGTH_MEDIUM);
    addSCurves();
    addDownhillToEnd(200);

    segments = new Segment[seglist.size()];
    seglist.toArray(segments);

    segments[findSegment(playerZ).index + 2].col = START;
    segments[findSegment(playerZ).index + 3].col = START;
    for(int i = 0; i < rumbleLength; i++)
      segments[segments.length-1-i].col = FINISH;
    
    trackLength = segments.length * segmentLength;
    
    resetSprites();
    resetCars();
  }
  
  void update(float dt) {
    Segment playerSegment = findSegment(position + playerZ);
    float playerW = SPRITES[PLAYER_STRAIGHT].w * spriteScale;
    float speedPercent = speed / maxSpeed;
    
    // at top speed, should be able to cross from left to right (-1 to 1) in 1 second
    float dx = dt * 2 * speedPercent;
    float startPosition = position;
    
    updateCars(dt, playerSegment, playerW);
    
    position = increase(position, dt * speed, trackLength);

    if (keyLeft)
      playerX = playerX - dx;
    else if (keyRight)
      playerX = playerX + dx;

    playerX = playerX - (dx * speedPercent * playerSegment.curve * centrifugal);

    if (keyFaster)
      speed = accelerate(speed, accel, dt);
    else if (keySlower)
      speed = accelerate(speed, breaking, dt);
    else
      speed = accelerate(speed, decel, dt);
      
    if ((playerX < -1) || (playerX > 1)) {
      if (speed > offRoadLimit)
        speed = accelerate(speed, offRoadDecel, dt);

      for (int n = 0; n < playerSegment.sprites.size(); n++) {
        Sprite sprite = playerSegment.sprites.get(n);
        float spriteW = sprite.source.w * spriteScale;
        if (overlap(playerX, playerW, sprite.offset + spriteW / 2.0 * (sprite.offset > 0 ? 1 : -1), spriteW, 0.5)) {
          speed = maxSpeed / 5;
          // stop in front of sprite (at front of segment)
          position = increase(playerSegment.p1.world.z, -playerZ, trackLength);
          break;
        }
      }
    }
    
    for (int n = 0; n < playerSegment.cars.size(); n++) {
      Car car = playerSegment.cars.get(n);
      float carW = car.sprite.w * spriteScale;
      if (speed > car.speed) {
        if (overlap(playerX, playerW, car.offset, carW, 0.8)) {
          speed = car.speed * (car.speed / speed);
          position = increase(car.z, -playerZ, trackLength);
          break;
        }
      }
    }
      
    // dont ever let it go too far out of bounds
    playerX = limit(playerX, -3, 3);
    // or exceed maxSpeed
    speed = limit(speed, 0, maxSpeed);

    skyOffset = increase(skyOffset, skySpeed * playerSegment.curve * (position - startPosition) / segmentLength, 1);
    hillOffset = increase(hillOffset, hillSpeed * playerSegment.curve * (position - startPosition) / segmentLength, 1);
    treeOffset = increase(treeOffset, treeSpeed * playerSegment.curve * (position - startPosition) / segmentLength, 1);
  }
  
  void updateCars(float dt, Segment playerSegment, float playerW) {
    for (int n = 0; n < cars.length; n++) {
      Car car = cars[n];
      Segment oldSegment = findSegment(car.z);
      car.offset = car.offset + updateCarOffset(car, oldSegment, playerSegment, playerW);
      car.z = increase(car.z, dt * car.speed, trackLength);
      car.percent = percentRemaining(car.z, segmentLength);
      Segment newSegment = findSegment(car.z);
      if (oldSegment != newSegment) {
        int index = oldSegment.cars.indexOf(car);
        oldSegment.cars.remove(index);
        newSegment.cars.add(car);
      }
    }
  }
  
  float updateCarOffset(Car car, Segment carSegment, Segment playerSegment, float playerW) {
    int lookahead = 20;
    float carW = car.sprite.w * spriteScale;

    // optimization, dont bother steering around other cars when 'out of sight' of the player
    if ((carSegment.index - playerSegment.index) > drawDistance)
      return 0;

    for (int i = 1; i < lookahead; i++) {
      Segment segment = segments[(carSegment.index + i) % segments.length];
      
      if ((segment == playerSegment) && (car.speed > speed) && (overlap(playerX, playerW, car.offset, carW, 1.2))) {
        float dir = 0;
        if (playerX > 0.5)
          dir = -1;
        else if (playerX < -0.5)
          dir = 1;
        else
          dir = (car.offset > playerX) ? 1 : -1;
        
        // the closer the cars (smaller i) and the greated the speed ratio, the larger the offset
        return dir * 1 / i * (car.speed - speed) / maxSpeed;
      }
      
      for (int j = 0; j < segment.cars.size(); j++) {
        Car otherCar = segment.cars.get(j);
        float otherCarW = otherCar.sprite.w * spriteScale;
        float dir = 0;
        if ((car.speed > otherCar.speed) && overlap(car.offset, carW, otherCar.offset, otherCarW, 1.2)) {
          if (otherCar.offset > 0.5)
            dir = -1;
          else if (otherCar.offset < -0.5)
            dir = 1;
          else
            dir = (car.offset > otherCar.offset) ? 1 : -1;
          return dir * 1 / i * (car.speed - otherCar.speed) / maxSpeed;
        }
      }
    }

    // if no cars ahead, but I have somehow ended up off road, then steer back on
    if (car.offset < -0.9)
      return 0.1;
    else if (car.offset > 0.9)
      return -0.1;
    else
    return 0;
  }

  void draw() {
    Segment baseSegment = findSegment(position);
    float basePercent = percentRemaining(position, segmentLength);
    Segment playerSegment = findSegment(position+playerZ);
    float playerPercent = percentRemaining(position+playerZ, segmentLength);
    float playerY = interpolate(playerSegment.p1.world.y, playerSegment.p2.world.y, playerPercent);
    float maxy = height;

    float x = 0;
    float dx = -(baseSegment.curve * basePercent);
    
    render.background(background, width, height, BACKGROUND[SKY], skyOffset, resolution * skySpeed  * playerY);
    render.background(background, width, height, BACKGROUND[HILLS], hillOffset, resolution * hillSpeed * playerY);
    render.background(background, width, height, BACKGROUND[TREES], treeOffset, resolution * treeSpeed * playerY);

    for (int n = 0; n < drawDistance; n++) {
      Segment segment = segments[(baseSegment.index + n) % segments.length];
      segment.looped = segment.index < baseSegment.index;
      segment.fog = exponentialFog(n/drawDistance, fogDensity);
      segment.clip = maxy;

      project(segment.p1, new PVector((playerX * roadWidth) - x,      playerY + cameraHeight, position - (segment.looped ? trackLength : 0)), cameraDepth, width, height, roadWidth);
      project(segment.p2, new PVector((playerX * roadWidth) - x - dx, playerY + cameraHeight, position - (segment.looped ? trackLength : 0)), cameraDepth, width, height, roadWidth);

      x = x + dx;
      dx = dx + segment.curve;

      if ((segment.p1.camera.z <= cameraDepth) || (segment.p2.screen.y >= segment.p1.screen.y) || (segment.p2.screen.y >= maxy))
          continue;

      render.segment(width, lanes, segment.p1.screen.x, segment.p1.screen.y, segment.p1.screen.w, segment.p2.screen.x, segment.p2.screen.y, segment.p2.screen.w, segment.fog, segment.col);

      maxy = segment.p1.screen.y;
    }
    
    for (int n = floor(drawDistance - 1); n > 0; n--) {
      Segment segment = segments[(baseSegment.index + n) % segments.length];
      
      for (int i = 0; i < segment.cars.size(); i++) {
        Car car = segment.cars.get(i);
        float spriteScaleScreen = interpolate(segment.p1.screenScale, segment.p2.screenScale, car.percent);
        float spriteX = interpolate(segment.p1.screen.x, segment.p2.screen.x, car.percent) + (spriteScaleScreen * car.offset * roadWidth * width / 2);
        float spriteY = interpolate(segment.p1.screen.y, segment.p2.screen.y, car.percent);
        render.sprite(width, height, resolution, roadWidth, sprites, car.sprite, spriteScaleScreen, spriteX, spriteY, -0.5, -1, segment.clip, spriteScale);
      }

      for (int i = 0; i < segment.sprites.size(); i++) {
        Sprite sprite = segment.sprites.get(i);
        float spriteScaleScreen = segment.p1.screenScale;
        float spriteX = segment.p1.screen.x + (spriteScaleScreen * sprite.offset * roadWidth * width / 2);
        float spriteY = segment.p1.screen.y;
        render.sprite(width, height, resolution, roadWidth, sprites, sprite.source, spriteScaleScreen, spriteX, spriteY, (sprite.offset < 0 ? -1 : 0), -1, segment.clip, spriteScale);
      }

      if (segment == playerSegment) {
        render.player(width, height, resolution, roadWidth, sprites, speed / maxSpeed,
                      cameraDepth / playerZ,
                      width / 2,
                      (height / 2) - (cameraDepth / playerZ * interpolate(playerSegment.p1.camera.y, playerSegment.p2.camera.y, playerPercent) * height / 2),
                      speed * (keyLeft ? -1 : keyRight ? 1 : 0),
                      playerSegment.p2.world.y - playerSegment.p1.world.y, spriteScale);
      }
    }
  }

  String worldName() {
    return "Final";
  }
}

color[] COLORS = new color[]{
  color(0),
  color(114, 215, 238),
  color(0, 81, 8),
  color(0, 81, 8),
};

color[] LIGHT = new color[]{
  color(107, 107, 107),
  color(16, 170, 16),
  color(85, 85, 85),
  color(204, 204, 204),
};

color[] DARK = new color[] {
  color(105, 105, 105),
  color(0, 154, 0),
  color(187, 187, 187),
};

color[] START = new color[]{
  color(255, 255, 255),
  color(255, 255, 255),
  color(255, 255, 255),
};

color[] FINISH = new color[]{
  color(0, 0, 0),
  color(0, 0, 0),
  color(0, 0, 0),
};

int ROAD = 0;
int GRASS = 1;
int RUMBLE = 2;
int LANE = 3;

Rect[] BACKGROUND = new Rect[]{
  new Rect(5, 5, 1280, 480), 
  new Rect(5, 495, 1280, 480), 
  new Rect(5, 985, 1280, 480), 
};

int HILLS = 0;
int SKY = 1;
int TREES = 2;
int FOG = 3;

Rect[] SPRITES = new Rect[] {
  new Rect(5, 5, 215, 540),
  new Rect(230, 5, 385, 265),
  new Rect(625, 5, 360, 360),
  new Rect(5, 555, 135, 332),
  new Rect(150, 555, 328, 282),
  new Rect(230, 280, 320, 220),
  new Rect(995, 5, 200, 315),
  new Rect(625, 375, 300, 170),
  new Rect(488, 555, 298, 190),
  new Rect(5, 897, 298, 190),
  new Rect(313, 897, 298, 190),
  new Rect(621, 897, 298, 140),
  new Rect(1205, 5, 282, 295),
  new Rect(1205, 310, 268, 170),
  new Rect(1205, 490, 150, 260),
  new Rect(1205, 760, 168, 248),
  new Rect(5, 1097, 240, 155),
  new Rect(929, 897, 235, 118),
  new Rect(255, 1097, 232, 152),
  new Rect(5, 1262, 230, 220),
  new Rect(245, 1262, 215, 220),
  new Rect(995, 330, 195, 140),
  new Rect(1365, 490, 122, 144),
  new Rect(1365, 644, 100, 78),
  new Rect(1383, 760, 88, 55),
  new Rect(1383, 825, 80, 59),
  new Rect(1383, 894, 80, 57),
  new Rect(1205, 1018, 80, 56),
  new Rect(1383, 961, 80, 45),
  new Rect(1295, 1018, 80, 45),
  new Rect(1385, 1018, 80, 45),
  new Rect(995, 480, 80, 41),
  new Rect(1085, 480, 80, 41),
  new Rect(995, 531, 80, 41),
};

int PALM_TREE = 0;
int BILLBOARD08 = 1;
int TREE1 = 2;
int DEAD_TREE1 = 3;
int BILLBOARD09 = 4;
int BOULDER3 = 5;
int COLUMN = 6;
int BILLBOARD01 = 7;
int BILLBOARD06 = 8;
int BILLBOARD05 = 9;
int BILLBOARD07 = 10;
int BOULDER2 = 11;
int TREE2 = 12;
int BILLBOARD04 = 13;
int DEAD_TREE2 = 14;
int BOULDER1 = 15;
int BUSH1 = 16;
int CACTUS = 17;
int BUSH2 = 18;
int BILLBOARD03 = 19;
int BILLBOARD02 = 20;
int STUMP = 21;
int SEMI = 22;
int TRUCK = 23;
int CAR03 = 24;
int CAR02 = 25;
int CAR04 = 26;
int CAR01 = 27;
int PLAYER_UPHILL_LEFT = 28;
int PLAYER_UPHILL_STRAIGHT = 29;
int PLAYER_UPHILL_RIGHT = 30;
int PLAYER_LEFT = 31;
int PLAYER_STRAIGHT = 32;
int PLAYER_RIGHT = 33;

int[] PLANTS = new int[]{TREE1, TREE2, DEAD_TREE1, DEAD_TREE2, PALM_TREE, BUSH1, BUSH2, CACTUS, STUMP, BOULDER1, BOULDER2, BOULDER3};
int[] BILLBOARDS = new int[]{BILLBOARD01, BILLBOARD02, BILLBOARD03, BILLBOARD04, BILLBOARD05, BILLBOARD06, BILLBOARD07, BILLBOARD08, BILLBOARD09};
int[] CARS = new int[]{CAR01, CAR02, CAR03, CAR04, SEMI, TRUCK};
