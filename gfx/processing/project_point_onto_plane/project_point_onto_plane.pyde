class Vec3:
    def __init__(self, *args):
        if len(args) == 3:
            self.x, self.y, self.z = args[0], args[1], args[2]
        else:
            raise
            
    def __add__(self, v):
        if isinstance(v, Vec3):
            return Vec3(self.x+v.x, self.y+v.y, self.z+v.z)
        else:
            raise
            
    def __sub__(self, v):
        if isinstance(v, Vec3):
            return Vec3(self.x-v.x, self.y-v.y, self.z-v.z)
        else:
            raise
            
    def __mul__(self, v):
        if isinstance(v, float) or isinstance(v, int):
            return Vec3(self.x*v, self.y*v, self.z*v)
        else:
            raise
    
    def __repr__(self):
        return "Vec3(%.2f, %.2f, %.2f)" % (self.x, self.y, self.z)
    
    def dot(self, v):
        return self.x*v.x + self.y*v.y + self.z*v.z
    
    def cross(self, v):
        x = self.y*v.z - self.z*v.y
        y = self.z*v.x - self.x*v.z
        z = self.x*v.y - self.y*v.x
        return Vec3(x, y, z)
    
    def length(self):
        return sqrt(self.dot(self))
    
    def normalize(self):
        l = self.length()
        return Vec3(self.x/l, self.y/l, self.z/l)

class Vec4:
    def __init__(self, *args):
        if len(args) == 4:
            self.x, self.y, self.z, self.w = args[0], args[1], args[2], args[3]
        else:
            raise

class Mat4:
    def __init__(self, *args):
        if len(args) == 1:
            self.copy(args[0])
        elif len(args) == 3:
            if isinstance(args[0], Vec3):
                X, Y, Z = args[0], args[1], args[2]
                self.m = [[X.x, Y.x, Z.x, 0],
                          [X.y, Y.y, Z.y, 0],
                          [X.z, Y.z, Z.z, 0],
                          [0, 0, 0, 1]]
        elif len(args) == 4:
            if isinstance(args[0], Vec3):
                X, Y, Z, O = args[0], args[1], args[2], args[3]
                self.m = [[X.x, Y.x, Z.x, O.x],
                          [X.y, Y.y, Z.y, O.y],
                          [X.z, Y.z, Z.z, O.z],
                          [0, 0, 0, 1]]
        else:
            self.zero()
    
    def copy(self, m):
        for i in range(4):
            for j in range(4):
                self.m[i][j] = m.m[i][j]
        return self
    
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
    
    def scale(self, *args):
        if len(args) == 1:
            sx, sy, sz = args[0], args[0], args[0]
        elif len(args) == 3:
            sx, sy, sz = args[0], args[1], args[2]
        else:
            raise
            
        self.m = [[sx, 0, 0, 0],
                  [0, sy, 0, 0],
                  [0, 0, sz, 0],
                  [0, 0, 0, 1]]
        return self
        
    def translate(self, *args):
        if len(args) == 3:
            tx, ty, tz = args[0], args[1], args[2]
        
        self.m = [[1, 0, 0, tx],
                  [0, 1, 0, ty],
                  [0, 0, 1, tz],
                  [0, 0, 0, 1]]
        return self
        
    def ndc2scr(self):
        global width
        global height
        hw = width / 2.0
        hh = height / 2.0
        n = 1.0
        f = 1000.0
        self.m = [[hw, 0, 0, hw],
                  [0, hh, 0, hh],
                  [0, 0, (f-n)/2, (f-n)/2],
                  [0, 0, 0, 1]]
        return self
    
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
        
    def __mul__(self, p):
        if isinstance(p, int) or isinstance(p, float):
            m = Mat4(self)
            for i in range(4):
                for j in range(4):
                    m.m[i][j] *= p
            return m
        elif isinstance(p, Vec3):
            v = Vec4(p.x, p.y, p.z, 1)
            v = self.__mul__(v)
            return Vec3(v.x, v.y, v.z)
        elif isinstance(p, Vec4):
            x = self.m[0][0]*p.x + self.m[0][1]*p.y + self.m[0][2]*p.z + p.w*self.m[0][3]
            y = self.m[1][0]*p.x + self.m[1][1]*p.y + self.m[1][2]*p.z + p.w*self.m[1][3]
            z = self.m[2][0]*p.x + self.m[2][1]*p.y + self.m[2][2]*p.z + p.w*self.m[2][3]
            w = self.m[3][0]*p.x + self.m[3][1]*p.y + self.m[3][2]*p.z + p.w*self.m[3][3]
            if w == 0:
                x, y, z, w = float("inf"), float("inf"), float("inf"), 1
            return Vec4(x/w, y/w, z/w, 1)
        elif isinstance(p, Mat4):
            m = Mat4()
            for i in range(4):
                for j in range(4):
                    for k in range(4):
                        m.m[i][j] += self.m[i][k]*p.m[k][j]
            return m
        else:
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

# http://fabiensanglard.net/doom3_documentation/37726-293748.pdf
class Quat:
    def __init__(self, *args):
        if len(args) == 0:
            self.x, self.y, self.z, self.w = 0, 0, 0, 1
        elif len(args) == 1:
            self.x, self.y, self.z, self.w = args[0].x, args[0].y, args[0].z, args[0].w
        elif len(args) == 4:
            self.x, self.y, self.z, self.w = args[0], args[1], args[2], args[3]
        else:
            raise
    
    def mat4(self):
        x, y, z, w = self.x, self.y, self.z, self.w
        
        x2 = x + x
        y2 = y + y
        z2 = z + z
        
        xx = x * x2
        xy = x * y2
        xz = x * z2
        
        yy = y * y2
        yz = y * z2
        zz = z * z2

        wx = w * x2
        wy = w * y2
        wz = w * z2

        m = Mat4()        
        m.m = [[1.0 - (yy + zz), xy - wz,         xz + wy,         0],
               [xy + wz,         1.0 - (xx + zz), yz - wx,         0],
               [xz - wy,         yz + wx,         1.0 - (xx + yy), 0],
               [0,               0,               0,               1]]
        return m
    
    def inverse(self):
        return Quat(-self.x, -self.y, -self.z, self.w)
    
    def length(self):
        l = self.x*self.x + self.y*self.y + self.z*self.z + self.w*self.w
        return sqrt(l)
    
    def __mul__(self, p):
        if isinstance(p, Vec3):
            return self.mat4() * p
        else:
            raise
    
    def rotate(self, a, t):
        s = sin(t/2)
        c = cos(t/2)
        self.x = a.x*s
        self.y = a.y*s
        self.z = a.z*s
        self.w = c
        return self

def rand3s(n):
    p = []
    for i in range(n):
        p.append(Vec3(random(-1, 1), random(-1, 1), random(-5, -1)))
    return p

def point3(x, y, z, col):
    global zbuffer
    
    if not (0 <= x and x < width):
        return
    if not (0 <= y and y < height):
        return
    
    idx = int(y*width + x)
    if idx >= len(zbuffer) or zbuffer[idx] < z:
        return
    zbuffer[idx] = z

    stroke(col)
    strokeWeight(5)
    point(x, y)

def line3(p0, p1, col):
    x0, y0, z0 = p0.x, p0.y, p0.z
    x1, y1, z1 = p1.x, p1.y, p1.z                
    dx = abs(x1-x0)
    dy = abs(y1-y0)
    dz = abs(z1-z0)
    sx = 1 if x0 < x1 else -1
    sy = 1 if y0 < y1 else -1 
    sz = 1 if z0 < z1 else -1
    dm = max(dx, dy, dz)
    i = dm
    x1 = y1 = z1 = dm/2
    while True:
        point3(x0, y0, z0, col)
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

# to calculate the basis of a plane,
# we use 3 points (3 points define a plane)
# and get the vector direction between them
# then cross them to get the normal, then finally
# cross the normal with the plane vector to get the other basis
def plnbasis(p):
    a = (p[1] - p[0]).normalize()
    b = (p[2] - p[0]).normalize()
    c = a.cross(b).normalize()
    a = b.cross(c).normalize()
    return [a, b, c]

def printpln(p, x, y):
    s = 16
    textSize(s)
    text("Points", x, y)
    y += s
    for i in range(len(p)):
        text(str(p[i]), x, y)
        y += s
    
    text("Basis", x, y)
    y += s
    basis = plnbasis(p)
    for i in range(len(basis)):
        text(str(basis[i]), x, y)
        y += s

def drawpln(p):
    printpln(p, 10, 30)
    
    P = Mat4().perspective(radians(70), width*1.0/height, 0.1, 1000)
    N = Mat4().ndc2scr()
    M = N * P
    i = 0
    while i < len(p):
        j = (i + 1) % len(p)
        a = M * p[i]
        b = M * p[j]
        line3(a, b, color(255))
        i += 1
    center = centroidpln(p)
    c = M * center
    point3(c.x, c.y, c.z, color(0, 0, 50))
    
    basis = plnbasis(p)
    B = Mat4(basis[0], basis[1], basis[2], Vec3(0.8, -0.3, 1))
    S = Mat4().scale(0.2)
    M = N * P * B * S
    
    O = Vec3(0, 0, 0)
    X = Vec3(1, 0, 0)
    Y = Vec3(0, 1, 0)
    Z = Vec3(0, 0, 1)
    
    O = M * O
    X = M * X
    Y = M * Y
    Z = M * Z
    line3(O, X, color(255, 0, 0))
    line3(O, Y, color(0, 255, 0))
    line3(O, Z, color(0, 0, 255))
    
    # draw a square on a plane
    B = Mat4(basis[0], basis[1], basis[2], center)
    S = Mat4().scale(1.2)
    M = N * P * B * S
    square = [Vec3(-1, -1, 0),
              Vec3(1, -1, 0),
              Vec3(1, 1, 0),
              Vec3(-1, 1, 0)]
    for i in range(len(square)):
        j = (i + 1) % len(square)
        a = square[i]
        b = square[j]
        a = M * a
        b = M * b
        colorMode(RGB, 1.0)
        line3(a, b, color(random(0, 1), random(0, 1), random(0, 1)))
    
    
    # https://stackoverflow.com/questions/9605556/how-to-project-a-point-onto-a-plane-in-3d
    # this works by getting the vector from the point we want to project to the origin of
    # the plane (in this case the centroid, calculate distance against this vector with the normal
    # by taking the dot product
    # finally project point is the projected_point = point - dist*normal
    # this basically finds a point on the plane where the distance is minimized between
    # the point and the plane, since the original vector where we do v = point - origin
    # would not be the minimum distance
    S = Mat4().scale(0.05)
    M = N * P * S
    for i in range(len(proj)):
        v = proj[i] - center
        d = v.dot(basis[2])
        pp = proj[i] - basis[2]*d
        pp = M * pp
        colorMode(RGB, 1.0)
        point3(pp.x, pp.y, pp.z, color(proj[i].x, proj[i].y, proj[i].z))
    
    colorMode(RGB, 255)
    
# centroid of 3d works the same as 2d, except we need to split it up
# one for x-y plane and the other for x-z plane        
def centroidpln(p):
    A0 = 0.0
    A1 = 0.0
    i = 0
    while i < len(p):
        j = (i + 1) % len(p)
        A0 += p[i].x*p[j].y - p[j].x*p[i].y
        A1 += p[i].x*p[j].z - p[j].x*p[i].z
        i += 1
    A0 *= 0.5
    A1 *= 0.5
    
    cx = 0.0
    cy = 0.0
    cz = 0.0
    i = 0
    while i < len(p):
        j = (i + 1) % len(p)
        cx += (p[i].x+p[j].x) * (p[i].x*p[j].y - p[j].x*p[i].y)
        cy += (p[i].y+p[j].y) * (p[i].x*p[j].y - p[j].x*p[i].y)
        cz += (p[i].z+p[j].z) * (p[i].x*p[j].z - p[j].x*p[i].z)
        i += 1

    # if the area is zero, then all the points are degenerate
    # for that axis, so we just use the first point on that axis
    if A0 != 0:    
        cx *= 1/(6*A0)
        cy *= 1/(6*A0)
    elif len(p) > 0:
        cx = p[0].x
        cy = p[0].y
        
    if A1 != 0:
        cz *= 1/(6*A1)
    elif len(p) > 0:
        cz = p[0].z
        
    return Vec3(cx, cy, cz)

plane = []
proj = []
zbuffer = []

def setup():
    global plane
    global zbuffer
    global proj
    size(1280, 800)
    plane = rand3s(3)
    proj = rand3s(256)

def draw():
    global zbuffer
    zbuffer = [float("inf")]*(width*height)
    background(100)
    drawpln(plane)

def keyPressed():
    global plane

    rot = False
    basis = plnbasis(plane)
    if key == ' ':
        plane = rand3s(3)
        proj = rand3s(256)
    elif keyCode == LEFT or keyCode == RIGHT:
        rot = True
        axis = basis[0]
        theta = .1 if keyCode == LEFT else -.1
    elif keyCode == DOWN or keyCode == UP:
        rot = True
        axis = basis[1]
        theta = .1 if keyCode == DOWN else -.1
    elif key == '1' or key == '2':
        rot = True
        axis = basis[2]
        theta = .1 if key == '1' else -.1

    if rot:
        Q = Quat().rotate(axis, theta)
        c = centroidpln(plane)
        for i in range(len(plane)):
            a = plane[i] - c
            a = Q * a
            plane[i] = a + c
    
