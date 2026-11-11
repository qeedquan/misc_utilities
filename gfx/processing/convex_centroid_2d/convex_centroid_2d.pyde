# calculates the centroid of a convex shape
class Vec2:
    def __init__(self, *args):
        if len(args) == 2:
            self.x, self.y = args[0], args[1]
            
    def __add__(self, v):
        if isinstance(v, Vec2):
            return Vec2(self.x+v.x, self.y+v.y)
        raise
        
    def __sub__(self, v):
        if isinstance(v, Vec2):
            return Vec2(self.x-v.x, self.y-v.y)
        raise

    def __mul__(self, v):
        if isinstance(v, float) or isinstance(v, int):
            return Vec2(self.x*v, self.y*v)
        raise

polygon = []

def setup():
    size(1280, 800)

def draw():
    background(80)
    drawPolygon(polygon)

def drawPolygon(p):
    strokeWeight(5)
    i = 0
    while i < len(p):
        j = (i + 1) % len(p)
        line(p[i].x, p[i].y, p[j].x, p[j].y)
        stroke(i*30, i+35, i+10)
        i += 1
    
    c = polygonCentroid(p)
    buf = "(%.2f, %.2f)" % (c.x, c.y)
    text(buf, 10, 10)
    if c.x != -1 and c.y != -1:
        stroke(34, 35, 69)
        strokeWeight(10)
        point(c.x, c.y)

def keyPressed():
    global polygon
    ch = chr(keyCode)
    if ch == ' ':
        polygon = []

def mousePressed(ev):
    global polygon
    if ev.button == LEFT:
        polygon.append(Vec2(mouseX, mouseY))
        if len(polygon) > 3 and not isConvex(polygon):
            polygon.pop()
    elif ev.button == RIGHT:
        if len(polygon) > 0:
            polygon.pop()

def mouseWheel(ev):
    center = polygonCentroid(polygon)
    rotatePolygonAroundCentroid(polygon, center, ev.count*0.3)
    
def mouseMoved(ev):
    if key == 0xffff:
        i = 0
        while i < len(polygon):
            polygon[i] += Vec2(mouseX-pmouseX, mouseY-pmouseY)
            i += 1

def rotatePolygonAroundCentroid(polygon, center, theta):
    p = polygon
    s = sin(theta)
    c = cos(theta)
    i = 0
    while i < len(p):
        l = p[i] - center
        x = l.x*c - l.y*s
        y = l.x*s + l.y*c
        p[i] = Vec2(x, y) + center
        i += 1

# https://stackoverflow.com/questions/471962/how-do-i-efficiently-determine-if-a-polygon-is-convex-non-convex-or-complex/45372025#45372025
def isConvex(polygon):
    """Return True if the polynomial defined by the sequence of 2D
    points is 'strictly convex': points are valid, side lengths non-
    zero, interior angles are strictly between zero and a straight
    angle, and the polygon does not intersect itself.

    NOTES:  1.  Algorithm: the signed changes of the direction angles
                from one side to the next side must be all positive or
                all negative, and their sum must equal plus-or-minus
                one full turn (2 pi radians). Also check for too few,
                invalid, or repeated points.
            2.  No check is explicitly done for zero internal angles
                (180 degree direction-change angle) as this is covered
                in other ways, including the `n < 3` check.
    """
    TWO_PI = 2 * PI
    try:  # needed for any bad points or direction changes
        # Check for too few points
        if len(polygon) < 3:
            return False
        # Get starting information
        old_x, old_y = polygon[-2].x, polygon[-2].y
        new_x, new_y = polygon[-1].x, polygon[-1].y
        new_direction = atan2(new_y - old_y, new_x - old_x)
        angle_sum = 0.0
        # Check each point (the side ending there, its angle) and accum. angles
        for ndx, newpoint in enumerate(polygon):
            # Update point coordinates and side directions, check side length
            old_x, old_y, old_direction = new_x, new_y, new_direction
            new_x, new_y = newpoint.x, newpoint.y
            new_direction = atan2(new_y - old_y, new_x - old_x)
            if old_x == new_x and old_y == new_y:
                return False  # repeated consecutive points
            # Calculate & check the normalized direction-change angle
            angle = new_direction - old_direction
            if angle <= -PI:
                angle += TWO_PI  # make it in half-open interval (-Pi, Pi]
            elif angle > PI:
                angle -= TWO_PI
            if ndx == 0:  # if first time through loop, initialize orientation
                if angle == 0.0:
                    return False
                orientation = 1.0 if angle > 0.0 else -1.0
            else:  # if other time through loop, check orientation is stable
                if orientation * angle <= 0.0:  # not both pos. or both neg.
                    return False
            # Accumulate the direction-change angle
            angle_sum += angle
        # Check that the total number of full turns is plus-or-minus 1
        return abs(round(angle_sum / TWO_PI)) == 1
    except (ArithmeticError, TypeError, ValueError):
        return False  # any exception means not a proper convex polygon

# https://en.wikipedia.org/wiki/Centroid#Of_a_polygon
# works only for convex shapes, which we guarantee
def polygonCentroid(polygon):
    p = polygon
    if len(p) < 1:
        return Vec2(-1, -1)
    if len(p) == 1:
        return p[0]
    if len(p) == 2:
        return p[0]*0.5 + p[1]*0.5
    
    A = 0.0
    i = 0
    while i < len(p):
        j = (i + 1) % len(p)
        A += p[i].x*p[j].y - p[j].x*p[i].y
        i += 1
    A *= 0.5
    
    if A == 0.0:
        return Vec2(-1, -1)
    
    cx = 0.0
    cy = 0.0
    i = 0
    while i < len(p):
        j = (i + 1) % len(p)
        cx += (p[i].x+p[j].x) * (p[i].x*p[j].y - p[j].x*p[i].y)
        cy += (p[i].y+p[j].y) * (p[i].x*p[j].y - p[j].x*p[i].y)
        i += 1
    
    cx *= 1/(6*A)
    cy *= 1/(6*A)
    return Vec2(cx, cy)
