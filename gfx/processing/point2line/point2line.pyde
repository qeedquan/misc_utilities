from math import *

W = 1280
H = 800

def closestPoint(l, x, y):
    n = l.length()
    ax = x-ln.x0
    ay = y-ln.y0
    bx = l.x1-l.x0
    by = l.y1-l.y0
    
    t = (ax*bx + ay*by)/(n*n)
    t = min(max(t, 0), 1)

    cx = l.x0 + t*(l.x1-l.x0)
    cy = l.y0 + t*(l.y1-l.y0)
    return cx, cy

def rotate(r, x, y):
    c = cos(r)
    s = sin(r)
    nx = c*x - s*y
    ny = s*x + c*y
    return nx, ny

def normdeg(r):
    d = degrees(r)
    if d < 0:
        d += 360
    d = 360 - d
    if isclose(d, 360):
        d -= 360
    return abs(d)

def isclose(a, b, rel_tol = 1e-09, abs_tol = 0.0):
    return abs(a - b) <= max(rel_tol * max(abs(a), abs(b)), abs_tol)

class Line:
    def __init__(self, x0 = 0, y0 = 0, x1 = 0, y1 = 0, col = 0, weight = 5):
        self.x0 = x0
        self.y0 = y0
        self.x1 = x1
        self.y1 = y1
        self.col = col
        self.weight = weight
    
    def draw(self):
        stroke(self.col)
        strokeWeight(self.weight)
        line(self.x0, self.y0, self.x1, self.y1)

        strokeWeight(self.weight*2)
        stroke(255, 0, 0)
        point(self.x0, self.y0)
        stroke(0, 0, 255)
        point(self.x1, self.y1)
    
    def midpoint(self):
        return (self.x0+self.x1)/2, (self.y0+self.y1)/2
    
    def setMidpoint(self, x, y):
        r = self.orientation()
        l = self.length()
        self.x0, self.y0 = x - l/2, y
        self.x1, self.y1 = x + l/2, y
        self.rotate(r)
    
    def orientation(self):
        x = self.x1 - self.x0
        y = self.y1 - self.y0
        r = atan2(y, x)
        return r
    
    def length(self):
        x = self.x1 - self.x0
        y = self.y1 - self.y0
        return sqrt(x*x + y*y)
    
    def rotate(self, r):
        x, y = self.midpoint()
        self.rotateAround(r, x, y)
    
    def rotateAround(self, r, x, y):
        self.x0, self.y0 = rotate(r, self.x0-x, self.y0-y)
        self.x1, self.y1 = rotate(r, self.x1-x, self.y1-y)

        self.x0, self.y0 = self.x0+x, self.y0+y
        self.x1, self.y1 = self.x1+x, self.y1+y

lp = []
ln = Line(W/2-100, H/2, W/2+100, H/2)

def setup():
    textSize(24)
    size(W, H)
    
def draw():
    background(150)
    ln.draw()
    for p in lp:
        p.draw()
    
    d = normdeg(ln.orientation())        
    x, y = ln.midpoint()
    s = "%f (%d, %d)" % (d, x, y)
    text(s, 25, 25)

def keyPressed(ev):
    global lp
    if keyCode == 32:
        lp = lp[:0]

def mousePressed(ev):
    if ev.button == LEFT:
        cx, cy = ln.midpoint()
        dx, dy = mouseX - cx, mouseY - cy
        for p in lp:
            p.x0 += dx
            p.y0 += dy
    
            p.x1 += dx
            p.y1 += dy
    
        ln.setMidpoint(mouseX, mouseY)

    elif ev.button == RIGHT:
        x0, y0 = mouseX, mouseY
        x1, y1 = closestPoint(ln, x0, y0)
        lp.append(Line(x0, y0, x1, y1, col=200))

def mouseWheel(ev):
    r = radians(ev.count*15)
    ln.rotate(r)
    cx, cy = ln.midpoint()
    for p in lp:
        p.rotateAround(r, cx, cy)
