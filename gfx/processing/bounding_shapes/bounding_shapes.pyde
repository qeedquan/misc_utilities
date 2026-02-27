class Rect:
    def __init__(self, *args):
        if len(args) == 4:
            self.x0, self.y0, self.x1, self.y1 = args[0], args[1], args[2], args[3]
        else:
            raise
            
    def dx(self):
        return self.x1 - self.x0
    
    def dy(self):
        return self.y1 - self.y0
    
    def cx(self):
        return (self.x0 + self.x1) / 2
    
    def cy(self):
        return (self.y0 + self.y1) / 2
    
    def diagonal(self):
        x = self.dx()
        y = self.dy()
        return sqrt(x*x + y*y)

class Circle:
    def __init__(self, *args):
        if len(args) == 3:
            self.x, self.y, self.r = args[0], args[1], args[2]
        else:
            raise

class Vec2:
    def __init__(self, *args):
        if len(args) == 2:
            self.x, self.y = args[0], args[1]
            
    def __add__(self, v):
        if isinstance(v, Vec2):
            return Vec2(self.x+v.x, self.y+v.y)
        raise
        
    def __sub__(self, v):
        if isinstance(v, Vec2):
            return Vec2(self.x-v.x, self.y-v.y)
        raise

    def __mul__(self, v):
        if isinstance(v, float) or isinstance(v, int):
            return Vec2(self.x*v, self.y*v)
        raise

boundMode = 0
shapeMode = 0
polygon = []

def setup():
    size(1280, 800)

def draw():
    global shapeMode
    global boundMode
    
    background(80)
    if shapeMode == 0:
        drawPolygon(polygon)

    noFill()
    stroke(255)
    if boundMode == 0:
        r = boundingRectOfPolygon(polygon)
        rect(r.x0, r.y0, r.dx(), r.dy())
    elif boundMode == 1:
        c = boundingCircleOfPolygon(polygon)
        ellipse(c.x, c.y, c.r, c.r)

def drawPolygon(p):
    strokeWeight(5)
    i = 0
    while i < len(p):
        j = (i + 1) % len(p)
        line(p[i].x, p[i].y, p[j].x, p[j].y)
        stroke(i*30, i+35, i+10)
        i += 1

# http://www.personal.kent.edu/~rmuhamma/Compgeometry/MyCG/CG-Applets/Center/centercli.htm
# this is not minimal, but it's simple
def boundingCircleOfPolygon(polygon):
    r = boundingRectOfPolygon(polygon)
    return Circle(r.cx(), r.cy(), r.diagonal())

def boundingRectOfPolygon(polygon):
    i = 0
    x0, y0 = float("inf"), float("inf")
    x1, y1 = float("-inf"), float("-inf")
    p = polygon
    while i < len(p):
        if x0 > p[i].x:
            x0 = p[i].x
        if x1 < p[i].x:
            x1 = p[i].x
        if y0 > p[i].y:
            y0 = p[i].y
        if y1 < p[i].y:
            y1 = p[i].y
        i += 1
    return Rect(x0, y0, x1, y1)

def keyPressed():
    global boundMode
    global polygon
    if key == '1':
        boundMode = 0
    elif key == '2':
        boundMode = 1
    elif key == ' ':
        polygon = []

def mousePressed(ev):
    global polygon
    if ev.button == LEFT:
        polygon.append(Vec2(mouseX, mouseY))
    elif ev.button == RIGHT:
        if len(polygon) > 0:
            polygon.pop()

 
