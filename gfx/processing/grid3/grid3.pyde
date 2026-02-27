from math import *

class Vec4:
    def __init__(self, *args):
        if len(args) == 0:
            self.x, self.y, self.z, self.w = 0, 0, 0, 0
        elif len(args) == 4:
            self.x, self.y, self.z, self.w = args[0], args[1], args[2], args[3]
        elif len(args) == 3:
            self.x, self.y, self.z, self.w = args[0], args[1], args[2], 1
        elif len(args) == 1:
            self.x, self.y, self.z, self.w = args[0].x, args[0].y, args[0].z, args[0].w

    def __add__(self, v):
        if isinstance(v, float) or isinstance(v, int):
            return Vec4(self.x+v, self.y+v, self.z+v, self.w+v)
        return Vec4(self.x+v.x, self.y+v.y, self.z+v.z, self.w+v.w)
    
    def __sub__(self, v):
        return Vec4(self.x-v.x, self.y-v.y, self.z-v.z, self.w-v.w)
    
    def __mul__(self, v):
        if isinstance(v, float) or isinstance(v, int):
            return Vec4(self.x*v, self.y*v, self.z*v, self.w*v)
        raise
       
    def dot(self, v):
        return self.x*v.x + self.y*v.y + self.z*v.z + self.w*v.w
    
    def angle(self, v):
        a = self.length()
        b = self.length()
        c = self.dot(v)
        return acos(c/(a*b))
    
    def cross(self, v):
        x = self.y*v.z - self.z*v.y
        y = self.z*v.x - self.x*v.z
        z = self.x*v.y - self.y*v.x
        return Vec4(x, y, z, 0)
    
    def length(self):
        return sqrt(self.dot(self))
    
    def normalize(self):
        l = self.length()
        if l == 0:
            return Vec4(self.x, self.y, self.z, self.w)
        return Vec4(self.x/l, self.y/l, self.z/l, self.w/l)
    
    def __str__(self):
        return "Vec4(%.2f, %.2f, %.2f, %.2f)" % (self.x, self.y, self.z, self.w)
    
    def perspdiv(self):
        if self.w == 1:
            return self
        if self.w == 0:
            return Vec4(self.x, self.y, self.z, 0)
        return Vec4(self.x/self.w, self.y/self.w, self.z/self.w, 1)

class Mat4:
    def __init__(self, *args):
        self.zero()
        if len(args) == 1:
            if isinstance(args[0], Mat4):
                self.copy(args[0])
        
    def __mul__(self, p):
        if isinstance(p, int) or isinstance(p, float):
            m = Mat4(self)
            for i in range(4):
                for j in range(4):
                    m.m[i][j] *= p
            return m
        elif isinstance(p, Vec4):
            x = self.m[0][0]*p.x + self.m[0][1]*p.y + self.m[0][2]*p.z + p.w*self.m[0][3]
            y = self.m[1][0]*p.x + self.m[1][1]*p.y + self.m[1][2]*p.z + p.w*self.m[1][3]
            z = self.m[2][0]*p.x + self.m[2][1]*p.y + self.m[2][2]*p.z + p.w*self.m[2][3]
            w = self.m[3][0]*p.x + self.m[3][1]*p.y + self.m[3][2]*p.z + p.w*self.m[3][3]
            return Vec4(x, y, z, w).perspdiv()
        elif isinstance(p, Mat4):
            m = Mat4()
            for i in range(4):
                for j in range(4):
                    for k in range(4):
                        m.m[i][j] += self.m[i][k]*p.m[k][j]
            return m
        raise
    
    def __str__(self):
        return """Mat4[
 %.2f %.2f %.2f %.2f
 %.2f %.2f %.2f %.2f
 %.2f %.2f %.2f %.2f
 %.2f %.2f %.2f %.2f]""" % (self.m[0][0], self.m[0][1], self.m[0][2], self.m[0][3],
                   self.m[1][0], self.m[1][1], self.m[1][2], self.m[1][3],
                   self.m[2][0], self.m[2][1], self.m[2][2], self.m[2][3],
                   self.m[3][0], self.m[3][1], self.m[3][2], self.m[3][3])
    
    def copy(self, p):
        for i in range(4):
            for j in range(4):
                self.m[i][j] = p.m[i][j]
    
    def zero(self):
        self.m = [[0, 0, 0, 0],
                  [0, 0, 0, 0],
                  [0, 0, 0, 0],
                  [0, 0, 0, 0]]
        return self
    
    def identity(self):
        self.m = [[1, 0, 0, 0],
                  [0, 1, 0, 0],
                  [0, 0, 1, 0],
                  [0, 0, 0, 1]]
        return self
        
    def translate(self, *args):
        if len(args) == 1:
            if isinstance(args[0], float) or isinstance(args[0], int):
                tx = ty = tz = args[0]
            elif isinstance(args[0], Vec4):
                tx, ty, tz = args[0].x, args[0].y, args[0].z
        elif len(args) == 3:
            tx, ty, tz = args[0], args[1], args[2]
        self.m = [[1, 0, 0, tx],
                  [0, 1, 0, ty],
                  [0, 0, 1, tz],
                  [0, 0, 0, 1]]
        return self
        
    def scale(self, *args):
        if len(args) == 1:
            sx = sy = sz = args[0]
        elif len(args) == 3:
            sx = sy = sz = args[0], args[1], args[2]
        self.m = [[sx, 0, 0, 0],
                  [0, sy, 0, 0],
                  [0, 0, sz, 0],
                  [0, 0, 0, 1]]
        return self
    
    def ndcToScreen(self):
        hw = width / 2.0
        hh = height / 2.0
        n = 0
        f = 1
        self.m = [[hw, 0, 0, hw],
                  [0, hh, 0, hh],
                  [0, 0, (f-n)/2, (f-n)/2],
                  [0, 0, 0, 1]]
        return self
    
    def rotate(self, axis, angle):
        c = cos(angle)
        s = sin(angle)
        w = axis.normalize()
        self.zero()
        self.m[0][0] = c + w.x*w.x*(1-c)
        self.m[0][1] = w.x*w.y*(1-c) - w.z*s
        self.m[0][2] = w.y*s + w.x*w.z*(1-c)
        self.m[1][0] = w.z*s + w.x*w.y*(1-c)
        self.m[1][1] = c + w.y*w.y*(1-c)
        self.m[1][2] = -w.x*s + w.y*w.z*(1-c)
        self.m[2][0] = -w.y*s + w.x*w.z*(1-c)
        self.m[2][1] = w.x*s + w.y*w.z*(1-c)
        self.m[2][2] = c + w.z*w.z*(1-c)
        self.m[3][3] = 1

    def lookAt(self, eye, center, up):
        f = center - eye
        f = f.normalize()
        
        s = f.cross(up)
        s = s.normalize()
                
        u = s.cross(f)
        self.m = [[s.x, s.y, s.z, 0],
                  [u.x, u.y, u.z, 0],
                  [-f.x, -f.y, -f.z, 0],
                  [0, 0, 0, 1]]
        T = Mat4().translate(-eye.x, -eye.y, -eye.z)
        return T * self
    
    def perspective(self, fovy, aspect, znear, zfar):
        f = 1 / tan(fovy/2)
        a = aspect
        z1 = (zfar+znear) / (znear-zfar)
        z2 = (2*zfar*znear) / (znear-zfar)
        self.m = [[f/a, 0, 0, 0],
                  [0, f, 0, 0],
                  [0, 0, z1, z2],
                  [0, 0, -1, 0]]
        return self

def line3(x0, y0, z0, x1, y1, z1, col):          
    if (x0 < 0 or x0 > width) and (x1 < 0 and x1 > width):
        return
    if (y0 < 0 or y0 > height) and (y1 < 0 and y1 > height):
        return
    if (z0 < 0 or z1 > 1):
        return
      
    dx = abs(x1-x0)
    dy = abs(y1-y0)
    dz = abs(z1-z0)
    sx = 1 if x0 < x1 else -1
    sy = 1 if y0 < y1 else -1 
    sz = 1 if z0 < z1 else -1
    sz = sz*1.0 / (width*height)
    dm = max(dx, dy, dz)
    i = dm
    x1 = y1 = z1 = dm/2
    while True:
        if z0 < 0 or z0 > 1:
            print(z0, z1)
            break
        stroke(col)
        strokeWeight(3)
        point(x0, y0)
        i -= 1
        if i <= 0:
            break
        x1 -= dx
        if x1 < 0:
            x1 += dm
            x0 += sx
        y1 -= dy
        if y1 < 0:
            y1 += dm
            y0 += sy
        z1 -= dz
        if z1 < 0:
            z1 += dm
            z0 += sz

def setup():
    size(1280, 800)

def draw():
    background(100)
    drawGrid(1, .1)
    textSize(16)
    y = 16
    val = "Eye " + str(eye)
    text(val, 0, y)

    y += 16
    val = "Center " + str(center)
    text(val, 0, y)

eye = Vec4(0, 0, -4.2)
center = Vec4(1.1, 4.2, 3)

# draw grid on the x-z plane and then an axis on top of it
def drawGrid(size, step):
    global eye
    global center
    S = Mat4().scale(2)
    C = Mat4().lookAt(eye, center, Vec4(0, 1, 0, 0))
    P = Mat4().perspective(radians(70), width*1.0/height, 1, 100)
    N = Mat4().ndcToScreen()
    i = 0
    strokeWeight(1)
    while i <= size:
        colorMode(HSB, 1.0)
        col = color(.5, .4, i)
        M = N * P * C * S 
        a = M * Vec4(-size, 0, i)
        b = M * Vec4(size, 0, i)
        line3(a.x, a.y, a.z, b.x, b.y, b.z, col)
        
        a = M * Vec4(-size, 0, -i)
        b = M * Vec4(size, 0, -i)
        line3(a.x, a.y, a.z, b.x, b.y, b.z, col)
        
        a = M * Vec4(i, 0, -size)
        b = M * Vec4(i, 0, size)
        line3(a.x, a.y, a.z, b.x, b.y, b.z, col)
        
        a = M * Vec4(-i, 0, -size)
        b = M * Vec4(-i, 0, size)
        line3(a.x, a.y, a.z, b.x, b.y, b.z, col)
        
        i += step
    
    l = -.2
    M = M * Mat4().translate(0, .6, 0)
    colorMode(RGB, 255)
    strokeWeight(20)
    
    a = M * Vec4(-l, 0, 0)
    b = M * Vec4(l, 0, 0)
    line3(a.x, a.y, a.z, b.x, b.y, b.z, color(255, 0, 0))
    
    a = M * Vec4(0, -l, 0)
    b = M * Vec4(0, l, 0)
    line3(a.x, a.y, a.z, b.x, b.y, b.z, color(0, 255, 0))

    a = M * Vec4(0, 0, -l)
    b = M * Vec4(0, 0, l)
    line3(a.x, a.y, a.z, b.x, b.y, b.z, color(0, 0, 255))

def keyPressed():
    step = 0.3
    if key == 'w':
        moveForward(step)
    elif key == 's':
        moveForward(-step)
    elif key == 'a':
        moveRight(-step)
    elif key == 'd':
        moveRight(step)

def mouseDragged(ev):
    global eye, center
    dx = mouseX - pmouseX
    dy = mouseY - pmouseY
    step = 0.1
    rotateRight(dx*step)
    rotateUp(dy*step)

def mouseWheel(ev):
    moveForward(ev.count*1.2)

def moveForward(distance):
    global eye, center
    forward = (center-eye).normalize()
    forward *= distance
    eye += forward
    center += forward

def moveRight(distance):
    global eye, center
    forward = (center-eye).normalize();
    right = forward.cross(Vec4(0, 1, 0, 0))
    right *= distance
    eye += right
    center += right

def rotateRight(angle):
    global eye, center
    R = Mat4()
    R.rotate(Vec4(0, 1, 0, 0), angle)
    forward = (center-eye).normalize()
    rotated = R * forward
    center = eye + rotated
    
def rotateUp(angle):
    global eye, center
    forward = (center-eye).normalize()
    right = forward.cross(Vec4(0, 1, 0, 0))
    R = Mat4()
    R.rotate(right, angle)
    rotated = R * forward
    center = eye + rotated
    
def clamp(x, a, b):
    if x < a:
        x = a
    elif x > b:
        x = b
    return x
