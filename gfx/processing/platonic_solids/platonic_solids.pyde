# https://userpages.umbc.edu/~squire/reference/polyhedra.shtml

from math import *

class Vec4:
    def __init__(self, *args):
        if len(args) == 0:
            self.x, self.y, self.z, self.w = 0, 0, 0, 0
        elif len(args) == 4:
            self.x, self.y, self.z, self.w = args[0], args[1], args[2], args[3]
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
            return Vec4(float("inf"), float("inf"), float("inf"), 1)
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
            # divide by w on every multiplication, but only really need it for the
            # perspective matrix
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
    
    # map to screen coordinate -1 -> 0, 1 -> size_x or size_y
    # z gets mapped to 0 or 1, they can also correspond to near and far
    # plane values, so 0 -> near and 1 -> far
    def ndcToScreen(self):
        hw = width / 2.0
        hh = height / 2.0
        self.m = [[hw, 0, 0, hw],
                  [0, hh, 0, hh],
                  [0, 0, .5, .5],
                  [0, 0, 0, 1]]
        return self
    
    def rotateX(self, r):
        c = cos(r)
        s = sin(r)
        self.m = [[1, 0, 0, 0],
                  [0, c, -s, 0],
                  [0, s, c, 0],
                  [0, 0, 0, 1]]
        return self
    
    def rotateY(self, r):
        c = cos(r)
        s = sin(r)
        self.m = [[c, 0, s, 0],
                  [0, 1, 0, 0],
                  [-s, 0, c, 0],
                  [0, 0, 0, 1]]
        return self
    
    def rotateZ(self, r):
        c = cos(r)
        s = sin(r)
        self.m = [[c, -s, 0, 0],
                  [s, c, 0, 0],
                  [0, 0, 1, 0],
                  [0, 0, 0, 1]]
        return self
    
    def shearX(self, a, b, c, d):
        self.m = [[1, a, c, 0],
                  [0, 1, d, 0],
                  [0, b, 1, 0],
                  [0, 0, 0, 1]]
        return self
        
    def shearY(self, a, b, c, d):
        self.m = [[1, 0, c, 0],
                  [a, 1, d, 0],
                  [b, 0, 1, 0],
                  [0, 0, 0, 1]]
        return self
        
    def shearZ(self, a, b, c, d):
        self.m = [[1, c, 0, 0],
                  [a, 1, 0, 0],
                  [b, d, 1, 0],
                  [0, 0, 0, 1]]
        return self
        
    # construct basis vector that is considered to be the
    # "camera" viewing basis, it moves the world to the camera
    # so the eye parameter provides translation, center only provides
    # part of the forward basis and does not contribute to moving
    # the origin
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
    
    # all points project down the negative z axis, if our
    # z points are negative, so it maps to positive after we apply
    # this matrix and vice versa
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

cube = [
    # front
    Vec4(-1, -1, 1, 1),
    Vec4(1, -1, 1, 1),
    
    Vec4(1, -1, 1, 1),
    Vec4(1, 1, 1, 1),
    
    Vec4(1, 1, 1, 1),
    Vec4(-1, 1, 1, 1),
    
    Vec4(-1, 1, 1, 1),
    Vec4(-1, -1, 1, 1),
    
    # side
    Vec4(-1, -1, 1, 1),
    Vec4(-1, -1, -1, 1),
    
    Vec4(1, -1, 1, 1),
    Vec4(1, -1, -1, 1),
    
    Vec4(1, 1, 1, 1),
    Vec4(1, 1, -1, 1),
    
    Vec4(-1, 1, 1, 1),
    Vec4(-1, 1, -1, 1),
    
    # back
    Vec4(-1, -1, -1, 1),
    Vec4(1, -1, -1, 1),
    
    Vec4(1, -1, -1, 1),
    Vec4(1, 1, -1, 1),
    
    Vec4(1, 1, -1, 1),
    Vec4(-1, 1, -1, 1),
    
    Vec4(-1, 1, -1, 1),
    Vec4(-1, -1, -1, 1),
]

tetrahedron = [
    # base
    Vec4(1, -1, -1, 1),
    Vec4(-1, 1, -1, 1),
    
    Vec4(-1, 1, -1, 1),
    Vec4(-1, -1, 1, 1),
    
    Vec4(-1, -1, 1, 1),
    Vec4(1, -1, -1, 1),
    
    # all base verts connecting to tip
    Vec4(1, -1, -1, 1),
    Vec4(1, 1, 1, 1),
    
    Vec4(-1, 1, -1, 1),
    Vec4(1, 1, 1, 1),
    
    Vec4(-1, -1, 1, 1),
    Vec4(1, 1, 1, 1),
]

octahedron = [
  Vec4(0.00, 1.00, 0.00, 1.00),
  Vec4(0.00, 0.00, 1.00, 1.00),
  Vec4(0.00, 0.00, 1.00, 1.00),
  Vec4(1.00, 0.00, 0.00, 1.00),
  Vec4(1.00, 0.00, 0.00, 1.00),
  Vec4(0.00, 1.00, 0.00, 1.00),

  Vec4(0.00, 1.00, 0.00, 1.00),
  Vec4(0.00, 0.00, 1.00, 1.00),
  Vec4(0.00, 0.00, 1.00, 1.00),
  Vec4(-1.00, 0.00, 0.00, 1.00),
  Vec4(-1.00, 0.00, 0.00, 1.00),
  Vec4(0.00, 1.00, 0.00, 1.00),

  Vec4(0.00, 1.00, 0.00, 1.00),
  Vec4(0.00, 0.00, -1.00, 1.00),
  Vec4(0.00, 0.00, -1.00, 1.00),
  Vec4(-1.00, 0.00, 0.00, 1.00),
  Vec4(-1.00, 0.00, 0.00, 1.00),
  Vec4(0.00, 1.00, 0.00, 1.00),

  Vec4(0.00, 1.00, 0.00, 1.00),
  Vec4(0.00, 0.00, -1.00, 1.00),
  Vec4(0.00, 0.00, -1.00, 1.00),
  Vec4(1.00, 0.00, 0.00, 1.00),
  Vec4(1.00, 0.00, 0.00, 1.00),
  Vec4(0.00, 1.00, 0.00, 1.00),

  Vec4(0.00, -1.00, 0.00, 1.00),
  Vec4(0.00, 0.00, 1.00, 1.00),
  Vec4(0.00, 0.00, 1.00, 1.00),
  Vec4(-1.00, 0.00, 0.00, 1.00),
  Vec4(-1.00, 0.00, 0.00, 1.00),
  Vec4(0.00, -1.00, 0.00, 1.00),

  Vec4(0.00, -1.00, 0.00, 1.00),
  Vec4(0.00, 0.00, 1.00, 1.00),
  Vec4(0.00, 0.00, 1.00, 1.00),
  Vec4(1.00, 0.00, 0.00, 1.00),
  Vec4(1.00, 0.00, 0.00, 1.00),
  Vec4(0.00, -1.00, 0.00, 1.00),

  Vec4(0.00, -1.00, 0.00, 1.00),
  Vec4(0.00, 0.00, -1.00, 1.00),
  Vec4(0.00, 0.00, -1.00, 1.00),
  Vec4(1.00, 0.00, 0.00, 1.00),
  Vec4(1.00, 0.00, 0.00, 1.00),
  Vec4(0.00, -1.00, 0.00, 1.00),

  Vec4(0.00, -1.00, 0.00, 1.00),
  Vec4(0.00, 0.00, -1.00, 1.00),
  Vec4(0.00, 0.00, -1.00, 1.00),
  Vec4(-1.00, 0.00, 0.00, 1.00),
  Vec4(-1.00, 0.00, 0.00, 1.00),
  Vec4(0.00, -1.00, 0.00, 1.00),
]

icosahedron = [
  Vec4(0.85, 0.00, 0.53, 1.00),
  Vec4(0.85, 0.00, -0.53, 1.00),
  Vec4(0.85, 0.00, -0.53, 1.00),
  Vec4(0.53, 0.85, 0.00, 1.00),
  Vec4(0.53, 0.85, 0.00, 1.00),
  Vec4(0.85, 0.00, 0.53, 1.00),

  Vec4(0.85, 0.00, 0.53, 1.00),
  Vec4(0.53, -0.85, 0.00, 1.00),
  Vec4(0.53, -0.85, 0.00, 1.00),
  Vec4(0.85, 0.00, -0.53, 1.00),
  Vec4(0.85, 0.00, -0.53, 1.00),
  Vec4(0.85, 0.00, 0.53, 1.00),

  Vec4(-0.85, 0.00, -0.53, 1.00),
  Vec4(-0.85, 0.00, 0.53, 1.00),
  Vec4(-0.85, 0.00, 0.53, 1.00),
  Vec4(-0.53, 0.85, 0.00, 1.00),
  Vec4(-0.53, 0.85, 0.00, 1.00),
  Vec4(-0.85, 0.00, -0.53, 1.00),

  Vec4(-0.85, 0.00, 0.53, 1.00),
  Vec4(-0.85, 0.00, -0.53, 1.00),
  Vec4(-0.85, 0.00, -0.53, 1.00),
  Vec4(-0.53, -0.85, 0.00, 1.00),
  Vec4(-0.53, -0.85, 0.00, 1.00),
  Vec4(-0.85, 0.00, 0.53, 1.00),

  Vec4(0.53, 0.85, 0.00, 1.00),
  Vec4(-0.53, 0.85, 0.00, 1.00),
  Vec4(-0.53, 0.85, 0.00, 1.00),
  Vec4(0.00, 0.53, 0.85, 1.00),
  Vec4(0.00, 0.53, 0.85, 1.00),
  Vec4(0.53, 0.85, 0.00, 1.00),

  Vec4(-0.53, 0.85, 0.00, 1.00),
  Vec4(0.53, 0.85, 0.00, 1.00),
  Vec4(0.53, 0.85, 0.00, 1.00),
  Vec4(0.00, 0.53, -0.85, 1.00),
  Vec4(0.00, 0.53, -0.85, 1.00),
  Vec4(-0.53, 0.85, 0.00, 1.00),

  Vec4(0.00, -0.53, -0.85, 1.00),
  Vec4(0.00, 0.53, -0.85, 1.00),
  Vec4(0.00, 0.53, -0.85, 1.00),
  Vec4(0.85, 0.00, -0.53, 1.00),
  Vec4(0.85, 0.00, -0.53, 1.00),
  Vec4(0.00, -0.53, -0.85, 1.00),

  Vec4(0.00, 0.53, -0.85, 1.00),
  Vec4(0.00, -0.53, -0.85, 1.00),
  Vec4(0.00, -0.53, -0.85, 1.00),
  Vec4(-0.85, 0.00, -0.53, 1.00),
  Vec4(-0.85, 0.00, -0.53, 1.00),
  Vec4(0.00, 0.53, -0.85, 1.00),

  Vec4(0.53, -0.85, 0.00, 1.00),
  Vec4(-0.53, -0.85, 0.00, 1.00),
  Vec4(-0.53, -0.85, 0.00, 1.00),
  Vec4(0.00, -0.53, -0.85, 1.00),
  Vec4(0.00, -0.53, -0.85, 1.00),
  Vec4(0.53, -0.85, 0.00, 1.00),

  Vec4(-0.53, -0.85, 0.00, 1.00),
  Vec4(0.53, -0.85, 0.00, 1.00),
  Vec4(0.53, -0.85, 0.00, 1.00),
  Vec4(0.00, -0.53, 0.85, 1.00),
  Vec4(0.00, -0.53, 0.85, 1.00),
  Vec4(-0.53, -0.85, 0.00, 1.00),

  Vec4(0.00, 0.53, 0.85, 1.00),
  Vec4(0.00, -0.53, 0.85, 1.00),
  Vec4(0.00, -0.53, 0.85, 1.00),
  Vec4(0.85, 0.00, 0.53, 1.00),
  Vec4(0.85, 0.00, 0.53, 1.00),
  Vec4(0.00, 0.53, 0.85, 1.00),

  Vec4(0.00, -0.53, 0.85, 1.00),
  Vec4(0.00, 0.53, 0.85, 1.00),
  Vec4(0.00, 0.53, 0.85, 1.00),
  Vec4(-0.85, 0.00, 0.53, 1.00),
  Vec4(-0.85, 0.00, 0.53, 1.00),
  Vec4(0.00, -0.53, 0.85, 1.00),

  Vec4(0.53, 0.85, 0.00, 1.00),
  Vec4(0.85, 0.00, -0.53, 1.00),
  Vec4(0.85, 0.00, -0.53, 1.00),
  Vec4(0.00, 0.53, -0.85, 1.00),
  Vec4(0.00, 0.53, -0.85, 1.00),
  Vec4(0.53, 0.85, 0.00, 1.00),

  Vec4(0.85, 0.00, 0.53, 1.00),
  Vec4(0.53, 0.85, 0.00, 1.00),
  Vec4(0.53, 0.85, 0.00, 1.00),
  Vec4(0.00, 0.53, 0.85, 1.00),
  Vec4(0.00, 0.53, 0.85, 1.00),
  Vec4(0.85, 0.00, 0.53, 1.00),

  Vec4(-0.85, 0.00, -0.53, 1.00),
  Vec4(-0.53, 0.85, 0.00, 1.00),
  Vec4(-0.53, 0.85, 0.00, 1.00),
  Vec4(0.00, 0.53, -0.85, 1.00),
  Vec4(0.00, 0.53, -0.85, 1.00),
  Vec4(-0.85, 0.00, -0.53, 1.00),

  Vec4(-0.53, 0.85, 0.00, 1.00),
  Vec4(-0.85, 0.00, 0.53, 1.00),
  Vec4(-0.85, 0.00, 0.53, 1.00),
  Vec4(0.00, 0.53, 0.85, 1.00),
  Vec4(0.00, 0.53, 0.85, 1.00),
  Vec4(-0.53, 0.85, 0.00, 1.00),

  Vec4(0.85, 0.00, -0.53, 1.00),
  Vec4(0.53, -0.85, 0.00, 1.00),
  Vec4(0.53, -0.85, 0.00, 1.00),
  Vec4(0.00, -0.53, -0.85, 1.00),
  Vec4(0.00, -0.53, -0.85, 1.00),
  Vec4(0.85, 0.00, -0.53, 1.00),

  Vec4(0.53, -0.85, 0.00, 1.00),
  Vec4(0.85, 0.00, 0.53, 1.00),
  Vec4(0.85, 0.00, 0.53, 1.00),
  Vec4(0.00, -0.53, 0.85, 1.00),
  Vec4(0.00, -0.53, 0.85, 1.00),
  Vec4(0.53, -0.85, 0.00, 1.00),

  Vec4(-0.85, 0.00, -0.53, 1.00),
  Vec4(0.00, -0.53, -0.85, 1.00),
  Vec4(0.00, -0.53, -0.85, 1.00),
  Vec4(-0.53, -0.85, 0.00, 1.00),
  Vec4(-0.53, -0.85, 0.00, 1.00),
  Vec4(-0.85, 0.00, -0.53, 1.00),

  Vec4(-0.85, 0.00, 0.53, 1.00),
  Vec4(-0.53, -0.85, 0.00, 1.00),
  Vec4(-0.53, -0.85, 0.00, 1.00),
  Vec4(0.00, -0.53, 0.85, 1.00),
  Vec4(0.00, -0.53, 0.85, 1.00),
  Vec4(-0.85, 0.00, 0.53, 1.00),
]


dodecahedron = [
  Vec4(0.61, 0.00, 0.79, 1.00),
  Vec4(0.19, 0.58, 0.79, 1.00),
  Vec4(0.19, 0.58, 0.79, 1.00),
  Vec4(-0.49, 0.36, 0.79, 1.00),
  Vec4(-0.49, -0.36, 0.79, 1.00),
  Vec4(0.19, -0.58, 0.79, 1.00),
  Vec4(0.19, -0.58, 0.79, 1.00),
  Vec4(0.61, 0.00, 0.79, 1.00),
  Vec4(0.61, 0.00, 0.79, 1.00),
  Vec4(0.19, 0.58, 0.79, 1.00),
  Vec4(0.19, 0.58, 0.79, 1.00),
  Vec4(0.30, 0.93, 0.19, 1.00),
  Vec4(0.79, 0.58, -0.19, 1.00),
  Vec4(0.98, 0.00, 0.19, 1.00),
  Vec4(0.98, 0.00, 0.19, 1.00),
  Vec4(0.61, 0.00, 0.79, 1.00),
  Vec4(0.19, 0.58, 0.79, 1.00),
  Vec4(-0.49, 0.36, 0.79, 1.00),
  Vec4(-0.49, 0.36, 0.79, 1.00),
  Vec4(-0.79, 0.58, 0.19, 1.00),
  Vec4(-0.30, 0.93, -0.19, 1.00),
  Vec4(0.30, 0.93, 0.19, 1.00),
  Vec4(0.30, 0.93, 0.19, 1.00),
  Vec4(0.19, 0.58, 0.79, 1.00),
  Vec4(-0.49, 0.36, 0.79, 1.00),
  Vec4(-0.49, -0.36, 0.79, 1.00),
  Vec4(-0.49, -0.36, 0.79, 1.00),
  Vec4(-0.79, -0.58, 0.19, 1.00),
  Vec4(-0.98, 0.00, -0.19, 1.00),
  Vec4(-0.79, 0.58, 0.19, 1.00),
  Vec4(-0.79, 0.58, 0.19, 1.00),
  Vec4(-0.49, 0.36, 0.79, 1.00),
  Vec4(-0.49, -0.36, 0.79, 1.00),
  Vec4(0.19, -0.58, 0.79, 1.00),
  Vec4(0.19, -0.58, 0.79, 1.00),
  Vec4(0.30, -0.93, 0.19, 1.00),
  Vec4(-0.30, -0.93, -0.19, 1.00),
  Vec4(-0.79, -0.58, 0.19, 1.00),
  Vec4(-0.79, -0.58, 0.19, 1.00),
  Vec4(-0.49, -0.36, 0.79, 1.00),
  Vec4(0.19, -0.58, 0.79, 1.00),
  Vec4(0.61, 0.00, 0.79, 1.00),
  Vec4(0.61, 0.00, 0.79, 1.00),
  Vec4(0.98, 0.00, 0.19, 1.00),
  Vec4(0.79, -0.58, -0.19, 1.00),
  Vec4(0.30, -0.93, 0.19, 1.00),
  Vec4(0.30, -0.93, 0.19, 1.00),
  Vec4(0.19, -0.58, 0.79, 1.00),
  Vec4(0.49, 0.36, -0.79, 1.00),
  Vec4(-0.19, 0.58, -0.79, 1.00),
  Vec4(-0.19, 0.58, -0.79, 1.00),
  Vec4(-0.30, 0.93, -0.19, 1.00),
  Vec4(0.30, 0.93, 0.19, 1.00),
  Vec4(0.79, 0.58, -0.19, 1.00),
  Vec4(0.79, 0.58, -0.19, 1.00),
  Vec4(0.49, 0.36, -0.79, 1.00),
  Vec4(-0.19, 0.58, -0.79, 1.00),
  Vec4(-0.61, 0.00, -0.79, 1.00),
  Vec4(-0.61, 0.00, -0.79, 1.00),
  Vec4(-0.98, 0.00, -0.19, 1.00),
  Vec4(-0.79, 0.58, 0.19, 1.00),
  Vec4(-0.30, 0.93, -0.19, 1.00),
  Vec4(-0.30, 0.93, -0.19, 1.00),
  Vec4(-0.19, 0.58, -0.79, 1.00),
  Vec4(-0.61, 0.00, -0.79, 1.00),
  Vec4(-0.19, -0.58, -0.79, 1.00),
  Vec4(-0.19, -0.58, -0.79, 1.00),
  Vec4(-0.30, -0.93, -0.19, 1.00),
  Vec4(-0.79, -0.58, 0.19, 1.00),
  Vec4(-0.98, 0.00, -0.19, 1.00),
  Vec4(-0.98, 0.00, -0.19, 1.00),
  Vec4(-0.61, 0.00, -0.79, 1.00),
  Vec4(-0.19, -0.58, -0.79, 1.00),
  Vec4(0.49, -0.36, -0.79, 1.00),
  Vec4(0.49, -0.36, -0.79, 1.00),
  Vec4(0.79, -0.58, -0.19, 1.00),
  Vec4(0.30, -0.93, 0.19, 1.00),
  Vec4(-0.30, -0.93, -0.19, 1.00),
  Vec4(-0.30, -0.93, -0.19, 1.00),
  Vec4(-0.19, -0.58, -0.79, 1.00),
  Vec4(0.49, -0.36, -0.79, 1.00),
  Vec4(0.49, 0.36, -0.79, 1.00),
  Vec4(0.49, 0.36, -0.79, 1.00),
  Vec4(0.79, 0.58, -0.19, 1.00),
  Vec4(0.98, 0.00, 0.19, 1.00),
  Vec4(0.79, -0.58, -0.19, 1.00),
  Vec4(0.79, -0.58, -0.19, 1.00),
  Vec4(0.49, -0.36, -0.79, 1.00),
  Vec4(0.49, 0.36, -0.79, 1.00),
  Vec4(-0.19, 0.58, -0.79, 1.00),
  Vec4(-0.19, 0.58, -0.79, 1.00),
  Vec4(-0.61, 0.00, -0.79, 1.00),
  Vec4(-0.19, -0.58, -0.79, 1.00),
  Vec4(0.49, -0.36, -0.79, 1.00),
  Vec4(0.49, -0.36, -0.79, 1.00),
  Vec4(0.49, 0.36, -0.79, 1.00),
]


colors = [
    color(255, 0, 0),
    color(0, 0, 255),
    color(0, 255, 0),
    color(255, 255, 0),
    color(92, 4, 38),
    color(0, 0, 48),
    color(34, 38, 0),
    color(4, 100, 34),
    color(62, 6, 88),
    color(56, 0, 208),
    color(34, 67, 0),
    color(200, 19, 92),
    color(224, 66, 2),
    color(0, 4, 25),
    color(0, 25, 0),
    color(65, 15, 0),
    color(86, 4, 41),
    color(0, 0, 48),
    color(34, 38, 0),
    color(4, 100, 34),
    color(62, 78, 88),
    color(56, 52, 208),
    color(102, 67, 0),
    color(180, 56, 192),
    color(255, 0, 0),
    color(0, 0, 255),
    color(0, 255, 0),
    color(255, 255, 0),
    color(92, 4, 38),
    color(0, 0, 48),
    color(34, 38, 0),
    color(4, 100, 34),
    color(62, 6, 88),
    color(56, 0, 208),
    color(34, 67, 0),
    color(200, 19, 92),
    color(224, 66, 2),
    color(0, 4, 25),
    color(0, 25, 0),
    color(65, 15, 0),
    color(86, 4, 41),
    color(0, 0, 48),
    color(34, 38, 0),
    color(4, 100, 34),
    color(62, 78, 88),
    color(56, 52, 208),
    color(102, 67, 0),
    color(180, 56, 192),
    color(92, 4, 38),
    color(0, 0, 48),
    color(34, 38, 0),
    color(4, 100, 34),
    color(62, 6, 88),
    color(56, 0, 208),
    color(34, 67, 0),
    color(200, 19, 92),
    color(224, 66, 2),
    color(0, 4, 25),
    color(0, 25, 0),
    color(65, 15, 0),
    color(86, 4, 41),
    color(0, 0, 48),
    color(34, 38, 0),
    color(4, 100, 34),
    color(62, 78, 88),
    color(56, 52, 208),
    color(102, 67, 0),
    color(180, 56, 192),
]

zbuffer = []
offxyz = Vec4(-3, -2, -4, 0)
rotxyz = Vec4()
pause = False
mono = True

def setup():
    size(1200, 800)

def draw():
    global zbuffer
    global rotxyz
    global offxyz
    global pause
    zbuffer = [0]*(width*height)

    background(100)
    off = Vec4(offxyz)
    for i in range(4):
        for j in range(5):
            if j == 0:
                drawShape(tetrahedron, colors, off, rotxyz)
            elif j == 1:
                drawShape(cube, colors, off, rotxyz)
            elif j == 2:
                drawShape(octahedron, colors, off, rotxyz)
            elif j == 3:
                drawShape(dodecahedron, colors, off, rotxyz)
            elif j == 4:
                drawShape(icosahedron, colors, off, rotxyz)
            off.x += 1.5
        off.y += 1.2
        off.x = offxyz.x
        
    if not pause:
        rotxyz.x += .1
        rotxyz.y += .2

def drawShape(pts, cols, off, rot):
    i = 0
    T = Mat4().translate(off)
    S = Mat4().scale(0.25)
    P = Mat4().perspective(radians(70), width*1.0/height, 0.1, 1000)
    D = Mat4().ndcToScreen()
    R = Mat4().rotateX(rot.x) * Mat4().rotateY(rot.y) * Mat4().rotateZ(rot.z)
    H = Mat4().shearX(1, 0, 0, 0) * Mat4().shearY(0, 1, 2, 0)
    
    while i < len(pts):
        col = cols[i/2]
        
        a = pts[i]
        b = pts[i+1]
        
        #a = H * a
        #b = H * b
        
        # rotate and translate it for viewing, this is what the lookat
        # matrix does, but since we want to do it explicitly instead of
        # based on a camera viewing basis, we would have to adjust the eye
        # coordinate of the camera to achieve the same effect here
        a = R * a
        b = R * b
        #print("rotate", str(R), str(pts[i]), str(pts[i+1]), str(a), str(b))
        
        # scale the coordinates so the shape fits in the window
        a = S * a
        b = S * b
        #print("scale", str(a), str(b))
        
        # translate it
        a = T * a
        b = T * b
        #print("translate", str(T), str(pts[i]), str(pts[i+1]), str(a), str(b))
        
        # when we project, usually (x, y) are still in [-1, 1]
        # z can be anywhere from [-1, -oo], as z gets more negative we zoom out,
        # as the perspective matrix we use treat -z as a direction to look forward
        # so -z will become positive after projection
        a = P * a
        b = P * b
        #print("project", str(P), str(pts[i]), str(pts[i+1]), str(a), str(b))
        
        # clip lines out of bounds
        if  ((a.y <= -1 or a.y >= 1) and (b.y <= -1 or b.y >= 1)) or ((a.x <= -1 or a.x >= 1) and (b.x <= -1 or b.x >= 1)):
            i += 2
            continue
        a.x = clamp(a.x, -1, 1)
        a.y = clamp(a.y, -1, 1)
        b.x = clamp(b.x, -1, 1)
        b.y = clamp(b.y, -1, 1)
        
        # if we just apply x/z, y/z, we will center to the left corner of the screen
        # because the standard origin for a screen is top-left, but we center it using
        # the matrix transform to place the origin at the middle of the screen,
        # in general if we want to place our vanishing point (center point of projection),
        # provide a O (origin) vector for translation
        a = D * a
        b = D * b
        #print("ndc", str(D), str(pts[i]), str(pts[i+1]), str(a), str(b))
        
        line3(a.x, a.y, a.z, b.x, b.y, b.z, col)
        i += 2

def point3(x, y, z):
    idx = int(y*width + x)
    ooz = 1/z
    # use 1/z for zbuffer as it allows it to represent 0 as points at
    # infinity
    if 0 <= idx and idx < len(zbuffer) and ooz > zbuffer[idx]:
        zbuffer[idx] = ooz
        point(x, y)

# bresenham line algorithm in 3d
# http://members.chello.at/~easyfilter/bresenham.html
# one thing is that while x, y are in pixel, z is in ndc coordinates
# after perspective projection, so we need to convert it to pixels
# we just assume that the pixel coordinate of z is the max of the width or height
def line3(x0, y0, z0, x1, y1, z1, col):
    global mono
    dx = abs(x1-x0)
    dy = abs(y1-y0)
    dz = abs(z1-z0)
    sx = 1 if x0 < x1 else -1
    sy = 1 if y0 < y1 else -1 
    sz = 1 if z0 < z1 else -1
    sz = sz*1.0 / max(width, height)
    dm = max(dx, dy, dz)
    i = dm
    x1 = y1 = z1 = dm/2
    if mono:
        col = color(255)
    while True:
        stroke(col)
        strokeWeight(3)
        point3(x0, y0, z0)
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

def keyPressed():
    global pause
    global mono
    ch = chr(keyCode)
    if ch == ' ':
        pause = not pause
    if ch == '1':
        mono = not mono
 
def mouseDragged(ev):
    global offxyz
    dx = mouseX - pmouseX
    dy = mouseY - pmouseY
    offxyz.x += clamp(dx, -0.1, 0.1)
    offxyz.y += clamp(dy, -0.1, 0.1)

def mouseWheel(ev):
    global offxyz
    offxyz.z -= ev.count*0.3

def clamp(x, a, b):
    if x < a:
        x = a
    if x > b:
        x = b
    return x
