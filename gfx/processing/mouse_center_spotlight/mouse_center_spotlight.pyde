from math import *

class Vec2:
    def __init__(self, *args):
        if len(args) == 0:
            self.x, self.y = 0, 0
        elif len(args) == 2:
            self.x, self.y, = args[0], args[1]
        else:
            raise
    
    def __sub__(self, v):
        if isinstance(v, Vec2):
            return Vec2(self.x-v.x, self.y-v.y)
        raise
    
    def length(self):
        return sqrt(self.x*self.x + self.y*self.y)
        
    def distance(self, v):
        return (self - v).length()
    
    def normalizeMap(self):
        x = map(self.x, 0, width, 0, 1)
        y = map(self.y, 0, height, 0, 1)
        return Vec2(x, y)

mode = '1'

def setup():
    size(1280, 800)

def draw():
    drawGrid()

def drawGrid():
    t = 32
    y = 0
    while y < height:
        x = 0
        while x < width:
            mp = Vec2(mouseX, mouseY).normalizeMap()
            cp = Vec2(x, y).normalizeMap()
            l = mp.distance(cp)
            l = map(l, 0, sqrt(2), 0, 1)            

            if mode == '1':
                colorMode(HSB, 1.0)
                fill(l, .5, .7)
            elif mode == '2':
                colorMode(RGB, 1.0)
                fill(.2, .2, l)
            elif mode == '3':
                colorMode(RGB, 1.0)
                fill(l, .3, .3)
            elif mode == '4':
                colorMode(RGB, 1.0)
                fill(.3, l, .5)
            elif mode == '5':
                colorMode(HSB, 1.0)
                fill(.5, .5, 1-l)
            elif mode == '6':
                colorMode(HSB, 1.0)
                fill(.5, l, l)
            
            rect(x, y, t, t)
            x += t
        y += t

def keyPressed():
    global mode
    mode = chr(keyCode)
