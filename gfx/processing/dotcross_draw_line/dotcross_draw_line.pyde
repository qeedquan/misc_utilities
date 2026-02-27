from math import *

class Vec2:
    def __init__(self, x, y):
        self.x, self.y = x, y
        
    def __sub__(self, v):
        return Vec2(self.x-v.x, self.y-v.y)
        
    def length(self):
        return sqrt(self.x*self.x + self.y*self.y)
    
    def normalize(self):
        l = self.length()
        if l == 0:
            return Vec2(self.x, self.y)
        return Vec2(self.x/l, self.y/l)

p1 = Vec2(0, 0)
p2 = Vec2(0, 0)

def setup():
    size(1280, 800)

def draw():
    global p1
    global p2
    
    background(100)
    dline(100, 100, 200, 200)
    dline(500, 500, 600, 500)
    dline(500, 500, 500, 600)
    dline(600, 100, 800, 500, 500)
    dline(700, 340, 565, 500, 150)
    dline(304, 453, 245, 10, 2000)

    fill(255)
    textSize(16)
    buf = "(%d, %d)-(%d, %d)" % (p1.x, p1.y, p2.x, p2.y)
    text(buf, 32, 32)
    eline(p1.x, p1.y, p2.x, p2.y)

# lame way to draw a line using cross/dot products
# to check if the point is on a line
# we just loop through the bounding rectangle of the
# line and do a point on line test. to control the width
# adjust the epsilon, since many lines have points on fractional values
# so to match those we need an epsilon area to compare against
def dline(x1, y1, x2, y2, w=1):
    stroke(0)
    sx = min(x1, x2)
    ex = max(x1, x2)
    sy = min(y1, y2)
    ey = max(y1, y2)

    a = Vec2(x1, y1)
    b = Vec2(x2, y2)
    
    for y in range(sy, ey+1, 1):
        for x in range(sx, ex+1, 1):
            p = Vec2(x, y)
            if online(a, b, p, w):
                point(x, y)

# another lame way to draw a line
# we can calculate the distance
# from a point to a line. When deciding to step in x/y direction
# just choose the x/y step such that the distance between the new point
# and the line itself is the smallest, we can skip on using floating point
# if we just use the numerator in the distance formula, since the denominator
# can be treated as a scaling constant (it preserves ordering)
def eline(x1, y1, x2, y2):
    stroke(255)
    
    sx, sy = 1, 1
    if x1 > x2:
        sx = -1
    if y1 > y2:
        sy = -1
    x, y = x1, y1
     
    while True:
        point(x, y)
        if x == x2 and y == y2:
            break
        
        d1 = approx_distance(x1, y1, x2, y2, x+sx, y)
        d2 = approx_distance(x1, y1, x2, y2, x, y+sy)
        d3 = approx_distance(x1, y1, x2, y2, x+sx, y+sy)
        
        ix, iy = 0, 0
        if d1 <= d2 and d1 <= d3:
            ix, iy = sx, 0
        if d2 <= d1 and d2 <= d3:
            ix, iy = 0, sy
        if d3 <= d1 and d3 <= d2:
            ix, iy = sx, sy
        if ix == 0 and iy == 0:
            raise
        x += ix
        y += iy

# https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line
def distance(x1, y1, x2, y2, x, y):
    a = (y2-y1)*x - (x2-x1)*y + x2*y1 - y2*x1
    b = sqrt((y2-y1)*(y2-y1) + (x2-x1)*(x2-x1))
    return abs(a)/b

def approx_distance(x1, y1, x2, y2, x, y):
    a = (y2-y1)*x - (x2-x1)*y + x2*y1 - y2*x1
    return abs(a)

# https://stackoverflow.com/questions/328107/how-can-you-determine-a-point-is-between-two-other-points-on-a-line-segment
def online(a, b, c, eps=1):
    # see if cross product of (b-a) and (c-a) is 0.
    # if it is, then they are parallel to each other and we know that this point
    # is in aligned
    cp = (c.y - a.y) * (b.x - a.x) - (c.x - a.x) * (b.y - a.y)    
    if abs(cp) > eps:
        return False

    # check orientation of the vector, if it is positive the point is to the right of the line
    dp = (c.x - a.x) * (b.x - a.x) + (c.y - a.y)*(b.y - a.y)
    if dp < 0:
        return False

    # now that we know that the point is on the right side, check to see if the distance squared from (a,b) is
    # greater than the dot product, that will tell us if the point is to the left of the end point
    sp = (b.x - a.x)*(b.x - a.x) + (b.y - a.y)*(b.y - a.y)
    if dp > sp:
        return False

    return True

def mousePressed(ev):
    global p1
    global p2
    if ev.button == LEFT:
        p1 = Vec2(mouseX, mouseY)
        p2 = Vec2(p1.x, p1.y)

def mouseDragged(ev):
    global p2
    if ev.button == LEFT:
        p2 = Vec2(mouseX, mouseY)
