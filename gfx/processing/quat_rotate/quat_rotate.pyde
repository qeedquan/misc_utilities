from math import *

class Vec3:
    def __init__(self, x, y, z):
        self.x, self.y, self.z = x, y, z
    
    def __add__(self, v):
        return Vec3(self.x+v.x, self.y+v.y, self.z+v.z)
    
    def __sub__(self, v):
        return Vec3(self.x-v.x, self.y-v.y, self.z-v.z)
    
    def __mul__(self, v):
        if isinstance(v, int) or isinstance(v, float):
            return Vec3(self.x*v, self.y*v, self.z*v)
        raise
    
    def __str__(self):
        return "Vec3(%f, %f, %f)" % (self.x, self.y, self.z)
    
    def dot(self, v):
        return self.x*v.x + self.y*v.y + self.z*v.z
    
    def cross(self, v):
        return Vec3(self.y*v.z - self.z*v.y, self.z*v.x - self.x*v.z, self.x*v.y - self.y*v.x)
    
    def length(self):
        return sqrt(self.dot(self))
    
    def normalize(self):
        l = self.length()
        return Vec3(self.x/l, self.y/l, self.z/l)

class Quat:
    def __init__(self, s = 0, x = 0, y = 0, z = 0):
        self.s, self.x, self.y, self.z = s, x, y, z
    
    def __add__(self, q):
        return Quat(self.s+q.s, self.x+q.x, self.y+q.y, self.z+q.z)
    
    def __sub__(self, q):
        return Quat(self.s-q.s, self.x-q.x, self.y-q.y, self.z-q.z)
    
    def __mul__(self, q):
        if isinstance(q, Quat):
            s = self.s*q.s - self.x*q.x - self.y*q.y - self.z*q.z
            x = self.s*q.x + q.s*self.x + self.y*q.z - q.y*self.z
            y = self.s*q.y + q.s*self.y + self.z*q.x - q.z*self.x
            z = self.s*q.z + q.s*self.z + self.x*q.y - q.x*self.y
            return Quat(s, x, y, z)
        elif isinstance(q, int) or isinstance(q, float):
            return Quat(self.s*q, self.x*q, self.y*q, self.z*q)
        raise
    
    def inverse(self):
        s, x, y, z = self.s*1.0, -self.x*1.0, -self.y*1.0, -self.z*1.0
        l = s**2 + x**2 + y**2 + z**2
        return Quat(s/l, x/l, y/l, z/l)
    
    def __str__(self):
        return "Quat(%f,%f,%f,%f)" % (self.s, self.x, self.y, self.z)

class Mat4:
    def __init__(self, p=None):
        self.zero()
        if isinstance(p, Mat4):
            self.copy(p)
        
    def __mul__(self, p):
        if isinstance(p, int) or isinstance(p, float):
            m = Mat4(self)
            for i in range(4):
                for j in range(4):
                    m.m[i][j] *= p
            return m
        elif isinstance(p, Vec3):
            x = self.m[0][0]*p.x + self.m[0][1]*p.y + self.m[0][2]*p.z + self.m[0][3]
            y = self.m[1][0]*p.x + self.m[1][1]*p.y + self.m[1][2]*p.z + self.m[1][3]
            z = self.m[2][0]*p.x + self.m[2][1]*p.y + self.m[2][2]*p.z + self.m[2][3]
            return Vec3(x, y, z)
        raise
    
    def __str__(self):
        return """Mat4[
 %f %f %f %f
 %f %f %f %f
 %f %f %f %f
 %f %f %f %f]""" % (self.m[0][0], self.m[0][1], self.m[0][2], self.m[0][3],
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
    
    def identity(self):
        self.m = [[1, 0, 0, 0],
                  [0, 1, 0, 0],
                  [0, 0, 1, 0],
                  [0, 0, 0, 1]]
        
    def translate(self, tx = 0, ty = 0, tz = 0):
        self.m = [[1, 0, 0, tx],
                  [0, 1, 0, ty],
                  [0, 0, 1, tz],
                  [0, 0, 0, 1]]
        
    def scale(self, sx = 1, sy = 1, sz = 1):
        self.m = [[sx, 0, 0, 0],
                  [0, sy, 0, 0],
                  [0, 0, sz, 0],
                  [0, 0, 0, 1]]

def setup():
    size(1280, 800)

def draw():
    background(102)

    x = 30
    y = 30
    theta = 0
    for i in range(1, 15):
        star(x, y, 10, 20, theta, i)
        star(x, y+100, 10, 20, theta, i*2)
        star(x, y+200, 10, 20, theta, i*4)
        star(x, y+300, 20*cos(i*10), 10*sin(i*20), theta, i*4)
        star(x, y+500, 20*tan(i*10), 10*sin(i*cos(i)), theta, i)
        x += 100
        theta = frameCount/60.0

# quaternion rotation:
# 1. convert point P (centered at origin) to quaternion 
# 2. define axis of rotation unit vector u
# 3. define transform quaternion [cos(t/2), sin(t/2)*u]
# 4. define inverse quaternion [cos(t/2), -sin(t/2)*u]
# 5. p' = q*p*q^-1
def rotate(center, pos, axis, theta):
    p = pos - center
    c = cos(theta)
    s = sin(theta)
    u = axis.normalize()

    q = Quat(c, s*u.x, s*u.y, s*u.z)
    qi = Quat(c, -s*u.x, -s*u.y, -s*u.z)
    qp = Quat(0, p.x, p.y, p.z)
    qr = qi * qp * q
    
    return Vec3(qr.x + center.x, qr.y + center.y, qr.z + center.z)

def star(x, y, r1, r2, theta, np):
    a = 2*PI / np
    h = a/2
    c = Vec3(x, y, 0)
    u = Vec3(0, 0, 1)
    
    beginShape()
    stroke(255)
    strokeWeight(1)
    i = 0
    while i < 2*PI:
        p = Vec3(x + r2*cos(i), y + r2*sin(i), 0)
        p = rotate(c, p, u, theta)        
        vertex(p.x, p.y)
        
        p = Vec3(x + r1*cos(i+h), y + r1*sin(i+h), 0)
        p = rotate(c, p, u, theta)
        vertex(p.x, p.y)
        
        i += a
    endShape(CLOSE)
    
    stroke(34, 45, 62)
    strokeWeight(5)
    point(x, y)
