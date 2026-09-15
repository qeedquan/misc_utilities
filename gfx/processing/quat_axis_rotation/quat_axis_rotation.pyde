# Specify an axis of rotation/angle for a quaternion, convert that
# quaternion into a 4x4 matrix. The columns of the 4x4 matrix
# represent the 3 orthonormal bases (X, Y, Z) axes) of that vector,
# ie, if you do X.dot(axis), Y.dot(axis), Z.dot(axis), you will get back
# the coordinates of the (x, y, z) for that axis.

# So when you specify a quaternion with an axis
# you are also implicitly specifying 3 orthornomal bases that comes with it
# and when you play with that axis of vector, you also affect the 3 implicit orthonormal
# bases at the same time, changing the angle would also affect them.

class Vec3:
    def __init__(self, *args):
        if len(args) == 3:
            self.x, self.y, self.z = args[0], args[1], args[2]
        elif len(args) == 0:
            self.x, self.y, self.z = 0, 0, 0
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
        if l == 0:
            l = 1e-6
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
    
    def basis(self):
        X = Vec3(self.m[0][0], self.m[1][0], self.m[2][0])
        Y = Vec3(self.m[0][1], self.m[1][1], self.m[2][1])
        Z = Vec3(self.m[0][2], self.m[1][2], self.m[2][2])
        W = Vec3(self.m[0][3], self.m[1][3], self.m[2][3])
        X = X.normalize()
        Y = Y.normalize()
        Z = Z.normalize()
        W = W.normalize()
        return [X, Y, Z, W]
    
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
        a = a.normalize()
        s = sin(t/2)
        c = cos(t/2)
        self.x = a.x*s
        self.y = a.y*s
        self.z = a.z*s
        self.w = c
        return self

def randv3():
    return Vec3(random(-1, 1), random(-1, 1), random(-10, -5)).normalize()

eye = Vec3(0, 0, -4)
center = Vec3(0, 0, 0)
axis = randv3()
theta = random(0, 2*PI)
zbuffer = []

def setup():
    size(1280, 800)

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

def draw():
    global zbuffer
    global axis
    global theta
    
    zbuffer = [float("inf")]*(width*height)
    
    background(100)

    N = Mat4().ndc2scr()
    P = Mat4().perspective(radians(90), width*1.0/height, 1, 1000)
    C = Mat4().lookAt(eye, center, Vec3(0, 1, 0))
    M = N * P * C
    
    o = M * Vec3(0, 0, 1)
    a = M * axis
    line3(o, a, color(20, 56, 45))
    
    q = Quat()
    q.rotate(axis, theta)
    B = q.mat4()
    S = Mat4().scale(0.5)
    M = N * P * C * B * S
    x = M * Vec3(1, 0, 0)
    y = M * Vec3(0, 1, 0)
    z = M * Vec3(0, 0, 1)

    line3(o, x, color(255, 0, 0))
    line3(o, y, color(0, 255, 0))
    line3(o, z, color(0, 0, 255))
    
    printinfo(axis, theta, B)

def printinfo(a, t, m):
    global eye
    global center
    
    s = 24
    textSize(s)
    
    X, Y, Z, _ = m.basis()
    x, y = 32, 32
    text("Rotation Axis " + str(a) + " Angle " + str(degrees(t)), x, y)
    y += s
    text("X Axis " +  str(X), x, y)
    y += s
    text("Y Axis " +  str(Y), x, y)
    y += s
    text("Z Axis " +  str(Z), x, y)
    y += s
    buf = "Dot Product (%.2f, %.2f, %.2f)" % (X.dot(a), Y.dot(a), Z.dot(a))
    text(buf, x, y)
    y += s
    
    text("Eye " + str(eye), x, y)
    y += s
    text("Center " + str(center), x, y)
    
def keyPressed():
    global axis
    global theta
    if key == ' ':
        axis = randv3()
        theta = random(0, 2*PI)
    elif key == '1':
        theta -= 0.01
    elif key == '2':
        theta += 0.01

def mouseDragged(ev):
    global theta
    global axis
    
    dx = mouseX - pmouseX
    dx = clamp(dx, -0.3, 0.3)
    
    if ev.button == LEFT:
        axis = Quat().rotate(Vec3(1, 0, 0), dx) * axis
    elif ev.button == CENTER:
        axis = Quat().rotate(Vec3(0, 0, 1), dx) * axis
    elif ev.button == RIGHT:
        axis = Quat().rotate(Vec3(0, 1, 0), dx) * axis
    
def mouseWheel(ev):
    eye.z -= ev.count*0.3
    if eye.z >= -1.2:
        eye.z = -1.2
        
def clamp(a, s, e):
    if a < s:
        a = s
    if a > e:
        a = e
    return a
