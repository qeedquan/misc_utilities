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
    
    def ortho(self, left, right, bottom, top, near, far):
        sx = 2 / (right - left)
        sy = 2 / (top - bottom)
        sz = -2 / (far - near)
        
        tx = (right + left) / (right - left)
        ty = (top + bottom) / (top - bottom)
        tz = (far + near) / (far - near)
        
        self.m = [[sx, 0, 0, tx],
                  [0, sy, 0, ty],
                  [0, 0, sz, tz],
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
        s = sin(t/2)
        c = cos(t/2)
        self.x = a.x*s
        self.y = a.y*s
        self.z = a.z*s
        self.w = c
        return self

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

zbuffer = []
shape = 0
proj = 0
dim = 2.0
modelview = Mat4()
projection = Mat4()
aspect = 0.0
fov = radians(55)
ph, th = 0.0, 0.0
steps = radians(5.0)

def setup():
    global plane
    global zbuffer
    global proj
    global aspect
    size(1280, 800)
    aspect = width*1.0 / height

def eyesight():
    global modelview
    global ph
    global th
    
    if proj == 0:
        st = sin(th)
        ct = cos(th)
        sp = sin(ph)
        cp = cos(ph)
        X = -2*dim * st * cp
        Y = 2*dim * sp
        Z = 2*dim * ct * cp
        
        eye = Vec3(X, Y, Z)
        center = Vec3()
        up = Vec3(0, cp, 0)
        modelview = Mat4().lookAt(eye, center, up)
    else:
        X = Quat().rotate(Vec3(1, 0, 0), ph)
        Y = Quat().rotate(Vec3(0, 1, 0), th)
        modelview = Y * X    

def project():
    global projection
    global proj
    global dim
    
    if proj == 0:
        projection = Mat4().perspective(fov, aspect, dim/4, 4*dim)
    elif proj == 1:
        projection = Mat4().ortho(-dim*aspect, dim*aspect, -dim, dim, -dim, dim)

def draw():
    global zbuffer
    global shape
    
    zbuffer = [float("inf")]*(width*height)
    background(100)

    eyesight()
    project()
    
    drawShape(shape)

def drawAxis():
    global modelview
    global projection

    T = Mat4().translate(-2.5, -1, -1)    
    M = Mat4().ndc2scr() * projection * T * modelview
    a = M * Vec3()
    b = M * Vec3(1, 0, 0)
    line3(a, b, color(255, 0, 0))
    
    b = M * Vec3(0, 1, 0)
    line3(a, b, color(0, 255, 0))
    
    b = M * Vec3(0, 0, 1)
    line3(a, b, color(0, 0, 255))
            
def drawShape(shape):
    if shape == 0:
        drawSphere()
    elif shape == 1:
        drawCylinder()
    elif shape == 2:
        drawCone()

def drawPoly(verts):
    M = Mat4().ndc2scr() * projection * modelview
    for i in range(len(verts)):
        j = (i + 1) % len(verts)
        a = M * verts[i]
        b = M * verts[j]
        line3(a, b, color(23, 45, 50))    

def drawSphere():
    global steps
    verts = []
    ph2 = radians(-90)
    while ph2 < radians(90):
        th2 = 0
        while th2 <= radians(360):
            v = spherical(th2, ph2)
            verts.append(v)
            
            v = spherical(th2, ph2+steps)
            verts.append(v)
            
            if len(verts) == 4:
                drawPoly(verts)
            if len(verts) >= 4:
                verts.pop(0)
                verts.pop(0)
            
            th2 += 2*steps
        ph2 += steps

def drawCone():
    global steps
    i = 0
    verts = []
    while i <= radians(360):
        v = Vec3(0, 0, 1)
        verts.append(v)
        
        v = Vec3(cos(i), sin(i), 0)
        verts.append(v)
        
        v = Vec3(cos(i+steps), sin(i+steps), 0)
        verts.append(v)
        
        drawPoly(verts)
        verts = []
        
        i += steps
        
    R = Quat().rotate(Vec3(1, 0, 0), radians(90))
    i = 0
    while i <= radians(360):
        v = R * Vec3(0, 0, 0)
        verts.append(v)
        
        v = R * Vec3(cos(i), 0, sin(i))
        verts.append(v)
        
        v = R * Vec3(cos(i+steps), 0, sin(i+steps))
        verts.append(v)
        
        drawPoly(verts)
        verts = []
        
        i += steps    

def drawCylinder():
    global steps
    verts = []
    i = 0
    while i <= radians(360):
        v = Vec3(cos(i), 1, sin(i))
        verts.append(v)
        
        v = Vec3(cos(i), -1, sin(i))
        verts.append(v)
        
        if len(verts) == 4:
                drawPoly(verts)
        if len(verts) >= 4:
                verts.pop(0)
                verts.pop(0)

        i += steps

    i = 1
    while i >= -1:
        verts = []
        v = Vec3(0, i, 0)
        verts.append(v)
        
        j = 0
        while j <= radians(360):
            v = Vec3(i*cos(j), i, sin(j))
            verts.append(v)
            
            if len(verts) == 3:
                drawPoly(verts)
                verts.pop()
                verts.pop()
            j += steps
        i -= 2
        

def spherical(th2, ph2):
    x = sin(th2)*cos(ph2)
    y = cos(th2)*cos(ph2)
    z = sin(ph2)
    return Vec3(x, y, z)

def keyPressed():
    global th
    global ph
    global dim
    global fov
    global shape
    global steps
    
    if keyCode == RIGHT:
        th += radians(5)
    elif keyCode == LEFT:
        th -= radians(5)
    elif keyCode == UP:
        ph += radians(5)
    elif keyCode == DOWN:
        ph -= radians(5)
    elif key == '1':
        fov -= radians(1)
    elif key == '2':
        fov += radians(1)
    elif key == 'a':
        dim += 0.1
    elif key == 's':
        dim -= 0.1
    elif key == 'z':
        shape = (shape - 1) % 3
    elif key == 'x':
        shape = (shape + 1) % 3
    elif key == '3':
        steps -= radians(5)
    elif key == '4':
        steps += radians(5)
    if steps <= 0.0:
        steps = 1
