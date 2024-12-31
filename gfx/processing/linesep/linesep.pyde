from math import *

W = 1200
H = 800

class Vec2:
    def __init__(self, x = 0, y = 0):
        self.x = x
        self.y = y
    
    def add(self, p):
        return Vec2(self.x+p.x, self.y+p.y)
    
    def sub(self, p):
        return Vec2(self.x-p.x, self.y-p.y)
    
    def dot(self, p):
        return self.x*p.x + self.y*p.y

    def length(self):
        return sqrt(self.dot(self))
    
    def __str__(self):
        return "{%.2f, %.2f}" % (self.x, self.y)

class Mat3:
    def __init__(self):
        self.zero()
    
    def zero(self):
        self.m = [[0, 0, 0],
                  [0, 0, 0],
                  [0, 0, 0]]

    def identity(self):
        self.m = [[1, 0, 0],
                  [0, 1, 0],
                  [0, 0, 1]]
    
    def rotate(self, r):
        s = sin(r)
        c = cos(r)
        self.m = [[c, -s, 0],
                  [s, c, 0],
                  [0, 0, 1]]
    
    def translate(self, tx, ty):
        self.m = [[1, 0, tx],
                  [0, 1, ty],
                  [0, 0, 1]]
    
    def mul(self, p):
        r = Mat3()
        for i in range(3):
            for j in range(3):
                for k in range(3):
                    r.m[i][j] += self.m[i][k]*p.m[k][j]
        self.m = r.m
        
    def rotateAround(self, r, x, y):
        m1 = Mat3()
        m2 = Mat3()
        m3 = Mat3()
        m1.translate(-x, -y)
        m2.rotate(r)
        m3.translate(x, y)
        
        m = Mat3()
        m.identity()
        m.mul(m3)
        m.mul(m2)
        m.mul(m1)
        self.m = m.m
    
    def transform(self, p):
        m = self.m
        x = m[0][0]*p.x + m[0][1]*p.y + m[0][2]
        y = m[1][0]*p.x + m[1][1]*p.y + m[1][2]
        return Vec2(x, y)

class Event:
    def __init__(self):
        self.keyCode = None
        self.mouseX = None
        self.mouseY = None

class Separator:
    def __init__(self):
        self.linewidth = 300
        self.weight = 5
        self.orient = 0
        self.pos = Vec2(W/2, H/2)
        self.left = []
        self.right = []
        self.alg = 0
    
    def getEndPoints(self):
        m = Mat3()
        m.rotateAround(self.orient, self.pos.x, self.pos.y)
        p0 = Vec2(self.pos.x, self.pos.y - self.linewidth/2)
        p1 = Vec2(self.pos.x, self.pos.y + self.linewidth/2)

        p0 = m.transform(p0)
        p1 = m.transform(p1)
        return p0, p1
    
    def draw(self):
        for p in self.left:
            strokeWeight(self.weight)
            stroke(255, 0, 0)
            point(p.x, p.y)
        for p in self.right:
            strokeWeight(self.weight)
            stroke(0, 255, 0)
            point(p.x, p.y)
        
        p0, p1 = self.getEndPoints()
        
        strokeWeight(self.weight)
        stroke(200)
        line(p0.x, p0.y, p1.x, p1.y)
        
        strokeWeight(self.weight*2)
        stroke(0, 0, 255)
        point(self.pos.x, self.pos.y)

evt = Event()
sep = Separator()

def setup():
    size(W, H)

def draw():
    background(100)
    sep.draw()
    if evt.keyCode == ALT:
        dx = mouseX - sep.pos.x
        dy = mouseY - sep.pos.y
        delta = Vec2(dx, dy)
        sep.pos = Vec2(mouseX, mouseY)
        
        for p in sep.left:
            p.x += dx
            p.y += dy
        for p in sep.right:
            p.x += dx
            p.y += dy

def mousePressed():
    if evt.keyCode != SHIFT:
        p = Vec2(mouseX, mouseY)
        a = sep.pos.sub(p)
        
        p0, p1 = sep.getEndPoints()
        if sep.alg == 0:
            # if we use dot product, have to rotate 90 degrees
            # in order to get the right sign for our purposes
            m = Mat3()
            m.rotateAround(radians(90), sep.pos.x, sep.pos.y)
            p0 = m.transform(p0)
            p1 = m.transform(p1)
            b = p1.sub(p0)
            sign = a.dot(b)
        else:
            # otherwise if we use cross product to figure out the 2d
            # determinant and the sign of that will tell us the side
            # flip the sign because of our convention
            sign = -((p1.x-p0.x)*(p.y-p0.y) - (p1.y-p0.y)*(p.x-p0.x))

        if sign < 0:
            sep.left.append(p)
        else:
            sep.right.append(p)

def mouseWheel(ev):
    if evt.keyCode == SHIFT:
        rad = radians(ev.count*30)
        sep.orient += rad
        m = Mat3()
        m.rotateAround(rad, sep.pos.x, sep.pos.y)
        for i, p in enumerate(sep.left):
            sep.left[i] = m.transform(p)
        for i, p in enumerate(sep.right):
            sep.right[i] = m.transform(p)

def keyPressed():
    evt.keyCode = keyCode
    if key == ' ':
        sep.alg = 1 - sep.alg
        print("Separation Algorithm:", sep.alg)

def keyReleased():
    evt.keyCode = 0
