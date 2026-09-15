// ported from http://www.math.tamu.edu/~romwell/arcball_js/index.html

/*
   ARCBALL INTERFACE EXAMPLE

   USAGE:
   - Left drag to rotate the scene.
   - Right drag to have it spinning.
   - Left click to stop spinning.

   HOW IT ALL WORKS: briefly, the way arcball works is as follows:

   - User clicks are projected on a sphere around the object that the user is rotating, to the
     hemisphere facing the user;
   - Thus, whenever user drags the mouse, we obtain an arc on that sphere (hence "arcball");
   - The object then is rotated along that arc by _twice_ the angle of the arc;
   - Quaternion multiplication is used to compute compositions of such rotations, since this is what,
     in a sense, quaternion multiplication is;
   - Because of the math (see Wikipedia), whenever the user traces a closed loop, the result is no rotation.


   SOME THEORY:

   The space of unit quaternions is a sphere in R^4 (set of points (x,y,z,w) with x^2 + y^2 + z^2 + w^2 = 1)
   with a multiplication rule (which we describe later).

   There is a homomorphism (in fact, a double cover) from the space of unit quaternions to the space of rotations.
   What this means is that you can think of quaternion multiplication as composition of rotations; in practical terms, this
   accounts for the fact that the Arcball interface is path-independent (closed loops result in no rotation).

   The homomorphism is as follows:

    (cos(alpha), sin(alpha)x, sin(alpha)y, sin(alpha)z) --> rotation about the axis (x,y,z) by angle alpha

   It is therefore convenient to store quaternion as a pair of a scalar and a vector; so we have

   (cos(alpha), sin(alpha) u)--> rotation about the axis u by angle alpha

   Note that the quaternions (x,u) and (-x,-u) correspond to the same rotation, and that these are the only
   unit quaternions corresponding to that rotation; any rotation in corresponds to exactly two quaternions in this way
   (hence the term "double cover").

   Thus, the quaternion corresponding to rotating the vector U on the unit sphere to vector V on the unit sphere is

         Q = (U . V, U x V)   (again, . and x are dot and cross products, respectively).

   The multiplication of quaternions can be defined in terms of these parts:

                (x,u)  (y,v) = (xy - u . v, xv + yu + u X v),

   where . stands for the dot product, and X stands for the vector cross product.

   One can derive this formula starting from another definition: write a quaternion (w,x,y,z) =  w + xi + yj + zk,
   and then multiply quaternions using the rules i^2 = j^2 = k^2 = ijk = -1 and distribution laws. For the purposes
   of the arcball, however, this is not a convenient defintition.

   Quaternions are also used to interpolate between rotations. Given two quaternions, A and B, representing the
   two rotations we want to interpolate between, we can compute the quaternion Q = BA^-1, which represents the
   rotation from A to B. Then Q^tA, for t in [0..1], provides a homotopy between these rotations. The power
   of the quaternion (cos(alpha), sin(alpha)u) is defined as the quaternion q^t := (cos(talpha), sin(talpha) u)
   (check that this definition is consistent with the definition of multiplication of quaternions).

   In this procedure there is one step to note: quaternions Q and -Q both represent the rotation from A to B,
   but their powers represent different homotopies: namely, rotation through the small and the big arc for the
   circle. While either may be used, we choose the small arc, which is the shortest path from A to B.

   Note that all such rotations will occur in a fixed time.
*/

package main

import (
	"image/color"
	"log"
	"math"
	"math/rand"
	"os"
	"runtime"
	"sort"
	"time"

	"github.com/qeedquan/go-media/image/chroma"
	"github.com/qeedquan/go-media/math/ga"
	"github.com/qeedquan/go-media/math/ga/mat4"
	"github.com/qeedquan/go-media/math/ga/quat"
	"github.com/qeedquan/go-media/math/ga/vec3"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
)

var (
	window   *sdl.Window
	renderer *sdl.Renderer
	cubes    []*Cube

	curq   ga.Quatd
	newq   ga.Quatd
	deltaq ga.Quatd

	slinit  time.Time
	slstart float64
	sllast  float64
	sltotal float64

	rotslowdown float64
	rotstart    ga.Vec3d
	rotend      ga.Vec3d
	rotating    bool

	goal  int
	goals = []ga.Quatd{
		{0, math.Sqrt(2) / 2, 0, -math.Sqrt(2) / 2},
		{math.Sqrt(2) / 2, 0, 0, math.Sqrt(2) / 2},
		{0, 0, 0, 1},
		{1, 0, 0, 0},
		{0, math.Sqrt(2) / 2, 0, math.Sqrt(2) / 2},
		{math.Sqrt(2) / 2, 0, 0, -math.Sqrt(2) / 2},
	}
)

func main() {
	runtime.LockOSThread()
	log.SetFlags(0)
	log.SetPrefix("arcball: ")
	rand.Seed(time.Now().UnixNano())
	initSDL()
	reset()
	for {
		event()
		blit()
	}
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func initSDL() {
	err := sdl.Init(sdl.INIT_VIDEO | sdl.INIT_TIMER)
	ck(err)

	sdl.SetHint(sdl.HINT_RENDER_SCALE_QUALITY, "best")
	w, h := 1280, 800
	wflag := sdl.WINDOW_RESIZABLE
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, wflag)
	ck(err)

	window.SetTitle("Arcball")

	slinit = time.Now()
}

func reset() {
	cubes = cubes[:0]
	cubes = append(cubes, newCube(ga.Vec3d{0, 0, 0}, 100, chroma.RandRGB(), chroma.RandRGB()))
	cubes = append(cubes, newCube(ga.Vec3d{150, 0, 0}, 60, chroma.RandRGB(), chroma.RandRGB()))
	cubes = append(cubes, newCube(ga.Vec3d{0, 150, 0}, 60, chroma.RandRGB(), chroma.RandRGB()))
	cubes = append(cubes, newCube(ga.Vec3d{0, 0, 150}, 60, chroma.RandRGB(), chroma.RandRGB()))
	cubes = append(cubes, newCube(ga.Vec3d{-150, 0, 0}, 60, chroma.RandRGB(), chroma.RandRGB()))
	cubes = append(cubes, newCube(ga.Vec3d{0, -150, 0}, 60, chroma.RandRGB(), chroma.RandRGB()))
	cubes = append(cubes, newCube(ga.Vec3d{0, 0, -150}, 60, chroma.RandRGB(), chroma.RandRGB()))

	goal = -1
	rotating = false
	curq = ga.Quatd{0.15, 0.15, 0, math.Sqrt(0.955)}
	deltaq = ga.Quatd{0, 0, 0, 1}
	sltotal = 500
	slstart = millis()
	sllast = slstart

	rotstart = ga.Vec3d{}
	rotend = ga.Vec3d{}
	rotslowdown = slstart
}

func event() {
	for {
		ev := sdl.PollEvent()
		if ev == nil {
			break
		}
		switch ev := ev.(type) {
		case sdl.QuitEvent:
			os.Exit(0)
		case sdl.KeyDownEvent:
			switch ev.Sym {
			case sdl.K_ESCAPE:
				os.Exit(0)
			case sdl.K_SPACE:
				reset()
			case sdl.K_1:
				setGoal(0)
			case sdl.K_2:
				setGoal(1)
			case sdl.K_3:
				setGoal(2)
			case sdl.K_4:
				setGoal(3)
			case sdl.K_5:
				setGoal(4)
			case sdl.K_6:
				setGoal(5)
			}
		case sdl.MouseButtonDownEvent:
			rotating = false
			rotstart = computeSphereVector(int(ev.X), int(ev.Y))
		case sdl.MouseMotionEvent:
			if ev.State&(sdl.BUTTON_LMASK|sdl.BUTTON_RMASK) != 0 {
				if goal == -1 {
					rotend = computeSphereVector(int(ev.X), int(ev.Y))

					// cross and dot represents sin(t), cos(t) in this case because it is normalized
					cross := vec3.Cross(rotstart, rotend)
					dot := vec3.Dot(rotstart, rotend)
					deltaq = ga.Quatd{cross.X, cross.Y, cross.Z, dot}
					curq = quat.Mul(deltaq, curq)
					rotstart = rotend
					if ev.State&sdl.BUTTON_RMASK == 0 {
						rotating = false
					}
				}
			}
		case sdl.MouseButtonUpEvent:
			if ev.Button == sdl.BUTTON_RIGHT || goal > -1 {
				rotating = true
			}
		}
	}
}

func blit() {
	drawStartTime := millis()
	renderer.SetDrawColor(sdl.Color{64, 64, 64, 255})
	renderer.Clear()

	// if there is a goal quaternion, rotate towards it
	dorot := false
	if 0 <= goal && goal < len(goals) {
		// if we passed the duration limit of rotation, just
		// set it to the end desired position
		dt := millis() - slstart
		if dt >= sltotal {
			curq = goals[goal]
			goal = -1
			rotating = false
		} else {
			t := (millis() - sllast) / sltotal
			deltaq = quat.Powu(newq, t)
		}
		dorot = true
	} else {
		if millis()-rotslowdown > 10 {
			rotslowdown = millis()
			dorot = true
		}
	}

	// continue rotating the scene if the rotation flag is set (via multiplying by delta quaternion on the left).
	// q_1*q_2*q_3...q_n is how one concatenates quaternion rotations together, so this rotates the current
	// quaternion
	if rotating && dorot {
		curq = quat.Mul(deltaq, curq)
	}

	f := ga.Vec3d{0, 0, 600}
	u := ga.Vec3d{0, 0, 1}
	for i := range cubes {
		cubes[i].Reset()
		cubes[i].Rotate(curq)
	}
	sortByDist(cubes[:], u)
	for i := range cubes {
		cubes[i].Blit(u, f)
	}
	renderer.Present()
	sllast = drawStartTime
}

func setGoal(n int) {
	goal = n
	if 0 <= goal && goal < len(goals) {
		newq = quat.Mul(goals[goal], quat.Inv(curq))
		if newq.W < 0 {
			newq = quat.Scale(newq, -1.0)
		}
		slstart = millis()
		rotating = true
	}
}

/*
 * This function computes the projection of the user click at (x,y) onto the unit
 * hemishpere facing the user (aka "the arcball"). The ball is centered in the
 * middle of the screen and has radius equal to gizmoR. All clicks outside this radius
 * are interpreted as being on the boundary of the disk facing the user.
 *
 * To compute pX and pY (x and y)coordinates of the projection, we subtract the center of the sphere
 * and scale down by gizmoR.
 * Since the projection is on a unit sphere, we know that pX^2 + pY^2 + pZ^2 = 1, so we can compute
 * the Z coordinate from this formula. This won't work if pX^2 + pY^2 > 1, i.e. if the user clicked
 * outside of the ball; this is why we interpret these clicks as being on the  boundary.
 */
func computeSphereVector(x, y int) ga.Vec3d {
	// set the radius of the arcball
	iwidth, iheight, _ := renderer.OutputSize()
	width, height := float64(iwidth), float64(iheight)
	gizmoR := width / 4.0

	pX := (float64(x) - width/2.0) / gizmoR
	pY := (float64(y) - height/2.0) / gizmoR
	L2 := pX*pX + pY*pY
	if L2 >= 1 {
		// interpret the click as being on the boundary
		return vec3.Normalize(ga.Vec3d{pX, pY, 0})
	} else {
		pZ := math.Sqrt(1 - L2)
		return ga.Vec3d{pX, pY, pZ}
	}
}

// Computes the face normal via the first 3 vertices.
func faceNormal(p []ga.Vec3d) ga.Vec3d {
	u := vec3.Sub(p[1], p[0])
	v := vec3.Sub(p[2], p[0])
	N := vec3.Cross(u, v)
	N = vec3.Normalize(N)
	return N
}

// Tells whether a face is not facing in the direction of a vector u.
func isBackFacing(p []ga.Vec3d, u ga.Vec3d) bool {
	n := faceNormal(p)
	return vec3.Dot(n, u) <= 0
}

/*
 * Naive sorting of cubes with respect to their distance in the direction u.
 * Used in a quick hack to avoid Painter's algorithm: simply paint cubes
 * whose center is closest first. This generally works if one only paints
 * convex bodies.
 */
func sortByDist(cubes []*Cube, u ga.Vec3d) {
	sort.Slice(cubes, func(i, j int) bool {
		return vec3.Dot(cubes[i].Pos, u) < vec3.Dot(cubes[j].Pos, u)
	})
}

type Cube struct {
	Pos    ga.Vec3d
	Center ga.Vec3d
	Stroke color.RGBA
	Fill   color.RGBA

	InitialVerts []ga.Vec3d
	Verts        []ga.Vec3d
	Faces        [][]int
}

func newCube(pos ga.Vec3d, width float64, stroke, fill color.RGBA) *Cube {
	c := &Cube{
		Pos:    pos,
		Center: pos,
		Stroke: stroke,
		Fill:   fill,
		Faces: [][]int{
			{0, 4, 6, 2},
			{1, 3, 7, 5},
			{0, 1, 5, 4},
			{2, 6, 7, 3},
			{0, 2, 3, 1},
			{4, 5, 7, 6},
		},
	}
	d := width / 2
	for i := uint(0); i < 8; i++ {
		c.Verts = append(c.Verts, ga.Vec3d{
			pos.X + d*c.bit(i, 0),
			pos.Y + d*c.bit(i, 1),
			pos.Z + d*c.bit(i, 2),
		})
	}
	c.InitialVerts = make([]ga.Vec3d, len(c.Verts))
	copy(c.InitialVerts, c.Verts)
	return c
}

func (c *Cube) bit(n, i uint) float64 {
	if (n>>i)&1 == 0 {
		return -1
	}
	return 1
}

func (c *Cube) Reset() {
	c.Pos = c.Center
	copy(c.Verts, c.InitialVerts)
}

func (c *Cube) Rotate(q ga.Quatd) {
	M := quat.Matrix(q)
	for i := range c.Verts {
		c.Verts[i] = mat4.Apply3(&M, c.Verts[i])
	}
	c.Pos = mat4.Apply3(&M, c.Pos)
}

/**
 * Paints the cube. toScreen is the vector normal to the screen pointing out (used to cull backfacing faces),
 * f is the focal point of the projection to the screen (screen is the XY plane).
 */
func (c *Cube) Blit(toScreen, f ga.Vec3d) {
	blitPolygons(projectPolygons(c.polygons(), f), toScreen, c.Stroke, c.Fill)
}

// Return the n'th face of the cube as an array of vertices
func (c *Cube) face(i int) []ga.Vec3d {
	p := make([]ga.Vec3d, 4)
	for j := range p {
		p[j] = c.Verts[c.Faces[i][j]]
	}
	return p
}

// Returns an array containing all faces of the cube
func (c *Cube) polygons() [][]ga.Vec3d {
	p := make([][]ga.Vec3d, 6)
	for i := range p {
		p[i] = c.face(i)
	}
	return p
}

// Computes the perspective projection of multiple polygons.
func projectPolygons(p [][]ga.Vec3d, f ga.Vec3d) [][]ga.Vec3d {
	a := make([][]ga.Vec3d, len(p))
	for i := range a {
		a[i] = make([]ga.Vec3d, len(p[i]))
		for j := range p[i] {
			a[i][j] = project(p[i][j], f)
		}
	}
	return a
}

/**
 * Returns the perspective projection of a vector v onto the XY plane with f as the focal point.
 * The z coordinate is left unchanged to compute visibility, etc.
 */
func project(v, f ga.Vec3d) ga.Vec3d {
	return ga.Vec3d{
		f.X + (v.X-f.X)*f.Z/(f.Z-v.Z),
		f.Y + (v.Y-f.Y)*f.Z/(f.Z-v.Z),
		v.Z,
	}
}

/**
 * Paints polygons (with 3D coordinates) on the screen with the current fill/stroke ignoring the Z coordinate
 * (orthographic projection onto the XY plane).
 * Backfacing polygons are not painted.
 */
func blitPolygons(p [][]ga.Vec3d, toScreen ga.Vec3d, stroke, fill color.RGBA) {
	width, height, _ := renderer.OutputSize()
	M := mat4.Translation(float64(width)/2, float64(height)/2, 0)

	for i := range p {
		var v []sdl.Point
		if !isBackFacing(p[i], toScreen) {
			for j := range p[i] {
				t := mat4.Apply3(&M, p[i][j])
				v = append(v, sdl.Point{
					int32(t.X),
					int32(t.Y),
				})
			}
		}
		sdlgfx.FilledPolygon(renderer, v, fill)
		sdlgfx.Polygon(renderer, v, stroke)
	}
}

func millis() float64 {
	return time.Since(slinit).Seconds() * 1000
}
