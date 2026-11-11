# http://members.chello.at/~easyfilter/bresenham.html

from math import *

class Grid:
    def __init__(self):
        self.size = 32
        self.x0, self.y0 = -1, -1
        self.x1, self.y1 = -1, -1
        
    def pix2cell(self, x, y):
        return x/self.size, y/self.size
    
    def point(self, x, y, err = None):
        fill(34+x*2, 40+y*2, 50)
        rect(x*self.size, y*self.size, self.size, self.size)
        
        if err == None:
            return
        y += 1
        fill(255)
        str = "%d" % (err)
        textSize(16)
        text(str, x*self.size, y*self.size)
    
    # derivation:
    # y = (y1-y0)/(x1-x0)(x-x0) - y0
    # y(x1-x0) = (y1-y0)(x-x0) - y0(x1-x0)
    # (y-y0)(x1-x0) - (y1-y0)(x-x0) = 0
    # now we have an implicit equation that gives us 0
    # whenever (x, y) is on the line, we can use this to calculate
    # the error term
    # e = (y-y0)(x1-x0) - (y1-y0)(x-x0)
    # if we define dx = (x1-x0) and dy = (y1-y0)
    # we can write it as
    # e = (y-y0)dx - (x-x0)dy
    # when we move to the next point, there are 2 choices we can make
    # either go in the x direction or y direction, x will always increase
    # since we are moving in a line, so the decision whether or not to increment
    # y or not depends on the error
    # let e_x be the error if we just increase x and e_xy if we move in both x and y direction
    # e_x = (y-y0)dx - (x+1-x0)dy
    # e_xy = (y+1-y0)dx - (x+1-x0)dy
    # we can rewrite this in term of the previous error
    # e_x = (y-y0)dx - (x-x0)dy + dx ->
    # e_x = (y+1-y0)dx - (x-x0)dy ->
    # e_x = e + dx
    # e_xy = (y-y0)dx - (x-x0)dy + dx - dy ->
    # e_xy = (y+1-y0)dx - (x+1-x0)dy ->
    # e_xy = e + dx - dy
    # dy is treated as negative in the code because y coordinate is flipped in window space
    # the initial e0 is defined to be e_xy
    # so we keep updating the error term when we decide to move in x or in xy direction
    def line(self, x0, y0, x1, y1):
        dx = abs(x1-x0)
        dy = -abs(y1-y0)
        sx = 1 if x0 < x1 else -1
        sy = 1 if y0 < y1 else -1
        err = dx+dy
        while True:
            self.point(x0, y0, err)
             
            if x0 == x1 and y0 == y1:
                break
            e2 = 2*err
            if e2 >= dy:
                err += dy
                x0 += sx
            if e2 <= dx:
                err += dx
                y0 += sy
    
    def thickline(self, x0, y0, x1, y1, wd):
        dx = abs(x1-x0)
        dy = abs(y1-y0)
        sx = 1 if x0 < x1 else -1
        sy = 1 if y0 < y1 else -1
        err = dx-dy
        ed = 1 if dx+dy == 0 else sqrt(dx*dx*1.0+dy*dy*1.0)
        
        wd = (wd+1)/2
        while True:
            self.point(x0, y0, err)
            e2 = err
            x2 = x0
            if 2*e2 >= -dx:
                e2 += dy
                y2 = y0
                while e2 < ed*wd and (y1 != y2 or dx > dy):
                    y2 += sy
                    # if want AA, then make color max(0,255*(abs(e2)/ed-wd+1)
                    # so it changes colors around the surroundings
                    self.point(x0, y2, err)
                    e2 += dx
                if x0 == x1:
                    break
                e2 = err
                err -= dy
                x0 += sx
            if 2*e2 <= dy:
                e2 = dx-e2
                while e2 < ed*wd and (x1 != x2 or dx < dy):
                    x2 += sx
                    self.point(x2, y0, err)
                    e2 += dy
                if y0 == y1:
                    break
                err += dx
                y0 += sy
                
    def ellipse(self, x0, y0, x1, y1):
        # value of diameter
        a = abs(x1-x0)
        b = abs(y1-y0)
        b1 = b&1
        # error increment
        dx = 4*(1-a)*b*b
        dy = 4*(b1+1)*a*a
        # error of 1 step
        err = dx+dy+b1*a*a
        
        if x0 > x1:
            x0 = x1
            x1 += a
        if y0 > y1:
            y0 = y1
        
        # starting pixel
        y0 += (b+1)/2
        y1 = y0-b1
        a *= 8*a
        b1 = 8*b*b
        
        while True:
            # make it readable as error value is too large for nice printing
            e = map(err, -100000, 100000, -50, 50)
            self.point(x1, y0, e)
            self.point(x0, y0, e)
            self.point(x0, y1, e)
            self.point(x1, y1, e)

            e2 = 2*err
            if e2 <= dy:
                y0 += 1
                y1 -= 1
                dy += a
                err += dy
            if e2 >= dx or 2*err > dy:
                x0 += 1
                x1 -= 1
                dx += b1
                err += dx
            if x0 > x1:
                break
        
        while y0-y1 < b:
            self.point(x0-1, y0)
            self.point(x1+1, y0)
            self.point(x0-1, y1)
            self.point(x0+1, y1)
     
            y0 += 1
            y1 -= 1
    
    def circle(self, xm, ym, r):
        x = -r
        y = 0
        err = 2-2*r
        while True:
            self.point(xm-x, ym+y, err)
            self.point(xm-y, ym-x, err)
            self.point(xm+x, ym-y, err)
            self.point(xm+y, ym+x, err)
            r = err
            if r <= y:
                y += 1
                err += y*2+1
            if r > x or err > y:
                x += 1
                err += x*2+1
            if x >= 0:
                break
    
    def quadbezier(self, x0, y0, x1, y1, x2, y2):
        sx = x2 - x1
        sy = y2 - y1
        xx = x0 - x1
        yy = y0 - y1
        cur = xx*sy - yy*sx
        if not (xx*sx <= 0 and yy*sy <= 0):
            print('bezier curve gradient non-monotonic', xx*sx, yy*sy)
            return

        if sx*sx+sy*sy > xx*xx+yy*yy:
            x2 = x0
            x0 = sx+x1
            y2 = y0
            y0 = sy+y1
            cur = -cur
            
        if cur != 0:
            xx += sx
            sx = 1 if x0 < x2 else -1
            xx *= sx
            yy += sy
            sy = 1 if y0 < y2 else -1
            yy *= sy
            xy = 2*xx*yy
            xx *= xx
            yy *= yy
            if cur*sx*sy < 0:
                xx = -xx
                yy = -yy
                xy = -xy
                cur = -cur
            dx = 4.0*sy*cur*(x1-x0)+xx-xy;
            dy = 4.0*sx*cur*(y0-y1)+yy-xy
            xx += xx
            yy += yy
            err = dx+dy+xy
            while True:
                self.point(x0, y0, err)
                if x0 == x2 and y0 == y2:
                    return
                y1 = 2*err < dx
                if 2*err > dy:
                    x0 += sx
                    dx -= xy
                    dy += yy
                    err += dy
                if y1:
                    y0 += sy
                    dy -= xy
                    dx += xx
                    err += dx
                if dy >= dx:
                    break
            self.line(x0, y0, x2, y2)
        
    def draw(self):
        s = self.size
        y = 0
        while y < height:
            x = 0
            while x < width:
                fill(150, 150, 150)
                rect(x, y, s, s)
                x += s
            y += s
        textSize(24)
        fill(255)
        if mode == 0:
            text('line', 32, 32)
            self.line(self.x0, self.y0, self.x1, self.y1)
        elif mode == 1:
            text('thick line', 32, 32)
            self.thickline(self.x0, self.y0, self.x1, self.y1, 5)
        elif mode == 2:
            text('ellipse', 32, 32)
            self.ellipse(self.x0, self.y0, self.x1, self.y1)
        elif mode == 3:
            text('circle', 32, 32)
            x0, x1 = self.x0, self.x1
            y0, y1 = self.y0, self.y1
            if x1 < x0:
                x0, x1 = x1, x0
            if y1 < y0:
                y0, y1 = y1, y0
            self.circle((x0+x1)/2, (y0+y1)/2, (x1-x0)/2)
        elif mode == 4:
            text('quadratic bezier', 32, 32)
            x0, x1 = self.x0, self.x1
            y0, y1 = self.y0, self.y1
            if x1 < x0:
                x1 = x0
            elif y1 < y0:
                y0 = y1
            self.quadbezier(x0, y0, (x0+x1)/2, (y0+y1)/2, x1, y1)

grid = None
mode = 0

def setup():
    global grid
    size(1280, 800)
    grid = Grid()

def draw():
    background(100)
    grid.draw()

def keyPressed(ev):
    global mode
    if '1' <= key and key <= '9':
        mode = int(key) - int('1')

def mousePressed(ev):
    global grid
    if ev.button == LEFT:
        grid.x0, grid.y0 = grid.pix2cell(mouseX, mouseY)
        grid.x1, grid.y1 = grid.x0, grid.y0

def mouseDragged(ev):
    global grid
    if ev.button == LEFT:
        grid.x1, grid.y1 = grid.pix2cell(mouseX, mouseY)
