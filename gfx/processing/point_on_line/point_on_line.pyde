from math import *

def isclose(a, b, rel_tol=1e-06, abs_tol=0.0):
    return abs(a-b) <= max(rel_tol * max(abs(a), abs(b)), abs_tol)

class Vec2:
    def __init__(self, x = 0, y = 0):
        self.x, self.y = x, y
        
    def __add__(self, p):
        return Vec2(self.x+p.x, self.y+p.y)

    def __sub__(self, p):
        return Vec2(self.x-p.x, self.y-p.y)
    
    def __mul__(self, p):
        if isinstance(p, int) or isinstance(p, float):
            return Vec2(self.x*p, self.y*p)
        raise
    
    def __eq__(self, p):
        return self.x == p.x and self.y == p.y

    def perp(self):
        return Vec2(-self.y, self.x)
    
    def dot(self, p):
        return self.x*p.x + self.y*p.y
    
    def length(self):
        return sqrt(self.x*self.x + self.y*self.y)
    
    def normalize(self):
        l = self.length()
        return Vec2(self.x/l, self.y/l)
    
    def __str__(self):
        return "Vec2(%f,%f)" % (self.x, self.y)

class Mat3:
    def __init__(self):
        self.zero()
    
    def zero(self):
        self.m = [[0, 0, 0],
                  [0, 0, 0],
                  [0, 0, 0]]
        return self
    
    def copy(self, m):
        for i in range(3):
            for j in range(3):
                self.m[i][j] = m.m[i][j]
    
    def identity(self):
        self.m = [[1, 0, 0],
                  [0, 1, 0],
                  [0, 0, 1]]
        return self
    
    def setBasis(self, X, Y, O):
        X = X.normalize()
        Y = Y.normalize()
        self.m[0][0] = X.x
        self.m[1][0] = X.y
        self.m[2][0] = 0
        
        self.m[0][1] = Y.x
        self.m[1][1] = Y.y
        self.m[2][1] = 0
        
        self.m[0][2] = O.x
        self.m[1][2] = O.y
        self.m[2][2] = 1
    
    def __mul__(self, p):
        if isinstance(p, Vec2):
            x = self.m[0][0]*p.x + self.m[0][1]*p.y + self.m[0][2]
            y = self.m[1][0]*p.x + self.m[1][1]*p.y + self.m[1][2]
            return Vec2(x, y)
        elif isinstance(p, Mat3):
            r = Mat3()
            for i in range(3):
                for j in range(3):
                    for k in range(3):
                        r.m[i][j] += self.m[i][k]*p.m[k][j]
            return r
        raise
    
    def det(self):
        d = 0
        for i in range(3):
             d = d + (self.m[0][i] * (self.m[1][(i+1)%3] * self.m[2][(i+2)%3] - self.m[1][(i+2)%3] * self.m[2][(i+1)%3]));
        return d
    
    def inverse(self):
        d = self.det()
        r = Mat3()
        for i in range(3):
            for j in range(3):
                 r.m[i][j] = ((self.m[(j+1)%3][(i+1)%3] * self.m[(j+2)%3][(i+2)%3]) - (self.m[(j+1)%3][(i+2)%3] * self.m[(j+2)%3][(i+1)%3])) / d
        return r
    
    def __str__(self):
        return """Mat3(
%f %f %f
%f %f %f
%f %f %f
)""" % (self.m[0][0], self.m[0][1], self.m[0][2],
        self.m[1][0], self.m[1][1], self.m[1][2],
        self.m[2][0], self.m[2][1], self.m[2][2])

class Line:
    def __init__(self, p0 = Vec2(), p1 = Vec2()):
        self.p0, self.p1 = p0, p1
        self.radius = 10
        
    def draw(self):
        p0, p1 = self.p0, self.p1
        if p0 == p1:
            return
        strokeWeight(self.radius)

        v = p1 - p0
        n = v.perp()
        n = n.normalize()
        l = v.length()
        
        mp = Vec2(mouseX, mouseY)
        if self.contains(mp):
            stroke(0, 255, 0)
        else:
            stroke(0)
        line(p0.x, p0.y, p1.x, p1.y)
        
        stroke(255)
        line(p0.x, p0.y, p0.x + n.x*l, p0.y + n.y*l)
        
        M = Mat3()
        M.setBasis(v, n, p0)

        a0 = M * Vec2(0, 0)
        a1 = M * Vec2(100, 0)
        stroke(255, 0, 0)
        line(a0.x, a0.y, a1.x, a1.y)
        
        stroke(0, 0, 255)
        a0 = M * Vec2(0, 0)
        a1 = M * Vec2(0, 100)
        line(a0.x, a0.y, a1.x, a1.y)
    
    # check if point is on line
    # create a basis matrix centered at origin of the line
    # then we take the inverse of the basis matrix and multiply
    # it with the mouse coordinate to get where the mouse coordinate is
    # relative to that basis. The X coordinate system in that basis represents
    # the line, so we check if the transformed mouse coordinate is between
    # start and end of the line segment and then for stroke weight, we just check
    # if it is between the radius size
    def contains(self, p):
        v = self.p1 - self.p0
        l = v.length()
        n = v.perp()
        M = Mat3()
        M.setBasis(v, n, self.p0)
        N = M.inverse()
        
        r0 = p
        r0 = N * r0
        if (0 <= r0.x and r0.x <= l) and (abs(r0.y) <= self.radius/2):
            return True
        return False

lines = []
wline = Line()
wstate = 0

def setup():
    size(1280, 800)

def draw():
    background(100)
    for l in lines:
        l.draw()
    wline.draw()

def keyPressed():
    global lines
    if key == 'u':
        lines = lines[:len(lines)-1]
    elif key == 'r':
        lines = []

def mouseMoved():
    if wstate == 1:
        wline.p1 = Vec2(mouseX, mouseY)

def mousePressed(ev):
    global wline
    global wstate
    global lines
    if ev.button == LEFT:
        if wstate == 0:
            wline.p0 = Vec2(mouseX, mouseY)
            wline.p1 = wline.p0
            wstate = 1
        elif wstate == 1:
            wline.p1 = Vec2(mouseX, mouseY)
            lines.append(wline)
            wline = Line()
            wstate = 0
            
