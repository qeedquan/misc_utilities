// based on https://github.com/ssloy/tinyrenderer
package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	"image/draw"
	"log"
	"math"
	"os"
	"runtime"

	"github.com/qeedquan/go-media/image/obj"
	"github.com/qeedquan/go-media/math/f64"
	"github.com/qeedquan/go-media/sdl"
)

type Shader struct {
	XYZ f64.Mat4
	UV  f64.Mat4
	NRM f64.Mat4
	NDC f64.Mat4
}

var (
	window           *sdl.Window
	renderer         *sdl.Renderer
	canvas           *image.RGBA
	zbuffer          []float64
	texture          *sdl.Texture
	models           []*obj.Model
	eye              f64.Vec3
	center           f64.Vec3
	modelview        f64.Mat4
	projection       f64.Mat4
	viewport         f64.Mat4
	lightDir         f64.Vec3
	orientation      f64.Quat
	rotStart, rotEnd f64.Vec3

	enableTexture bool
	enableBump    bool
	wireframe     bool
)

func main() {
	runtime.LockOSThread()
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}
	models = loadModels(flag.Args())
	initSDL()
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
	w, h := 800, 800
	wflag := sdl.WINDOW_RESIZABLE
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, wflag)
	ck(err)

	window.SetTitle("Obj Viewer")

	reset()
	computeScreenProjections(w, h)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: objviewer [options] model.obj ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func loadModels(names []string) []*obj.Model {
	var m []*obj.Model
	for _, name := range names {
		f, err := os.Open(name)
		if err != nil {
			fmt.Fprintln(os.Stderr, "objviewer: ", err)
			continue
		}
		p, err := obj.Load(name, f)
		f.Close()

		if err != nil {
			fmt.Fprintln(os.Stderr, "objviewer: ", err)
			continue
		}
		m = append(m, p)
	}
	return m
}

func reset() {
	eye = f64.Vec3{1, 1, 3}
	center = f64.Vec3{0, 0, 0}
	lightDir = f64.Vec3{1, 1, 1}
	orientation = f64.Quat{0, 0, 0, 1}

	w, h, _ := renderer.OutputSize()
	computeScreenProjections(w, h)
	enableTexture = true
	enableBump = true
}

func computeScreenProjections(w, h int) {
	var err error
	up := f64.Vec3{0, 1, 0}
	modelview = LookAt(eye, center, up)
	projection = Perspective(-1 / eye.Sub(center).Len())
	viewport = Viewport(w/8, h/8, w*3/4, h*3/4)
	canvas = image.NewRGBA(image.Rect(0, 0, w, h))
	zbuffer = make([]float64, w*h)
	texture, err = renderer.CreateTexture(sdl.PIXELFORMAT_ABGR8888, sdl.TEXTUREACCESS_STREAMING, w, h)
	ck(err)
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
			case sdl.K_1:
				enableTexture = !enableTexture
			case sdl.K_2:
				enableBump = !enableBump
			case sdl.K_w:
				wireframe = !wireframe
			case sdl.K_SPACE:
				reset()
			}
		case sdl.MouseButtonDownEvent:
			rotStart = computeSphereVector(int(ev.X), int(ev.Y))
		case sdl.MouseMotionEvent:
			if ev.State&(sdl.BUTTON_LMASK|sdl.BUTTON_RMASK) != 0 {
				rotEnd = computeSphereVector(int(ev.X), int(ev.Y))
				cross := rotStart.Cross(rotEnd)
				dot := rotStart.Dot(rotEnd)
				deltaOrientation := f64.Quat{cross.X, cross.Y, cross.Z, dot}
				if ev.State&sdl.BUTTON_LMASK != 0 {
					orientation = deltaOrientation.Mul(orientation)
				} else if ev.State&sdl.BUTTON_RMASK != 0 {
					M := deltaOrientation.Mat4()
					lightDir = M.Transform3(lightDir)
				}
				rotStart = rotEnd
			}

		case sdl.WindowEvent:
			switch ev.Event {
			case sdl.WINDOWEVENT_RESIZED:
				w, h := int(ev.Data[0]), int(ev.Data[1])
				computeScreenProjections(w, h)
			}
		}
	}
}

func blit() {
	for i := range zbuffer {
		zbuffer[i] = -math.MaxFloat32
	}
	renderer.Clear()
	draw.Draw(canvas, canvas.Bounds(), image.NewUniform(color.RGBA{120, 120, 120, 255}), image.ZP, draw.Over)
	for i := range models {
		var shader Shader
		shader.Init()
		blitModel(&shader, models[i])
	}
	texture.Update(nil, canvas.Pix, canvas.Stride)
	renderer.Copy(texture, nil, nil)
	renderer.Present()
}

func abs(x int) int {
	if x < 0 {
		return -x
	}
	return x
}

func line(s *Shader, m *obj.Model, p0, p1 [2]float64) {
	x0, y0 := int(p0[0]), int(p0[1])
	x1, y1 := int(p1[0]), int(p1[1])
	sx, sy := -1, -1
	dx := abs(x1 - x0)
	dy := -abs(y1 - y0)
	if x0 < x1 {
		sx = 1
	}
	if y0 < y1 {
		sy = 1
	}
	e := dx + dy
	for {
		canvas.Set(x0, y0, color.White)
		if x0 == x1 && y0 == y1 {
			break
		}
		e2 := 2 * e
		if e2 >= dy {
			e += dy
			x0 += sx
		}
		if e2 <= dx {
			e += dx
			y0 += sy
		}
	}
}

func triangle(s *Shader, m *obj.Model) {
	var M f64.Mat4
	M.Identity()
	M.Mul(&M, &viewport)
	M.Mul(&M, &s.XYZ)
	M.Transpose()

	var pts [3][2]float64
	for i := 0; i < 3; i++ {
		for j := 0; j < 2; j++ {
			pts[i][j] = M[i][j] / M[i][3]
		}
	}

	if wireframe {
		line(s, m, pts[0], pts[1])
		line(s, m, pts[1], pts[2])
		line(s, m, pts[2], pts[0])
		return
	}

	w, h, _ := renderer.OutputSize()
	clamp := [2]float64{float64(w) - 1, float64(h) - 1}
	bmin := [2]float64{math.MaxFloat32, math.MaxFloat32}
	bmax := [2]float64{-math.MaxFloat32, -math.MaxFloat32}
	for i := 0; i < 3; i++ {
		for j := 0; j < 2; j++ {
			bmin[j] = math.Max(0, math.Min(bmin[j], pts[i][j]))
			bmax[j] = math.Min(clamp[j], math.Max(bmax[j], pts[i][j]))
		}
	}

	for x := int(bmin[0]); x <= int(bmax[0]); x++ {
		for y := int(bmin[1]); y <= int(bmax[1]); y++ {
			p0 := f64.Vec2{pts[0][0], pts[0][1]}
			p1 := f64.Vec2{pts[1][0], pts[1][1]}
			p2 := f64.Vec2{pts[2][0], pts[2][1]}
			p := f64.Vec2{float64(x), float64(y)}
			bc_screen := f64.TriangleBarycentric(p, p0, p1, p2)
			bc_clip := f64.Vec3{bc_screen.X / M[0][3], bc_screen.Y / M[1][3], bc_screen.Z / M[2][3]}
			bc_clip = bc_clip.Scale(1 / (bc_clip.X + bc_clip.Y + bc_clip.Z))
			frag_depth := s.XYZ.Row(2).XYZ().Dot(bc_clip)

			if bc_screen.X < 0 || bc_screen.Y < 0 || bc_screen.Z < 0 || zbuffer[x+y*w] > frag_depth {
				continue
			}
			discard, col := s.Fragment(bc_clip, m)
			if !discard {
				zbuffer[x+y*w] = frag_depth
				canvas.Set(x, y, col)
			}
		}
	}
}

func blitModel(s *Shader, m *obj.Model) {
	for i := range m.Faces {
		for j := 0; j < 3; j++ {
			s.Vertex(m, i, j)
		}
		triangle(s, m)
	}
}

func (s *Shader) Init() {
	s.XYZ.Identity()
	s.NDC.Identity()
	s.UV.Identity()
	s.NRM.Identity()
}

func (s *Shader) Vertex(m *obj.Model, face, vert int) {
	var v, t, n f64.Vec4

	i := m.Faces[face][vert]
	if j := i[0]; 0 <= j && j < len(m.Verts) {
		v = m.Verts[j]
	}
	if j := i[1]; 0 <= j && j < len(m.Coords) {
		t = m.Coords[j]
	}
	if j := i[2]; 0 <= j && j < len(m.Normals) {
		n = m.Normals[j]
	}

	R := orientation.Mat4()
	M := f64.Mat4{
		{1, 0, 0, 0},
		{0, -1, 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1},
	}
	M.Mul(&M, &R)
	M.Mul(&M, &projection)
	M.Mul(&M, &modelview)
	MI := M
	MI.Inverse()
	MI.Transpose()
	v = M.Transform(v)
	vd := v.PerspectiveDivide()
	vt := t
	vn := MI.Transform(n)

	s.XYZ.SetCol(vert, v)
	s.NDC.SetCol(vert, vd)
	s.UV.SetCol(vert, vt)
	s.NRM.SetCol(vert, vn)
}

func (s *Shader) Fragment(bar f64.Vec3, m *obj.Model) (discard bool, col color.RGBA) {
	br := f64.Vec4{bar.X, bar.Y, bar.Z, 1}
	uv := s.UV.Transform(br)
	bn := s.NRM.Transform(br).XYZ().Normalize()

	n1 := s.NDC.Col(0)
	n2 := s.NDC.Col(1)
	n3 := s.NDC.Col(2)
	a1 := n2.Sub(n1)
	a2 := n3.Sub(n1)
	a3 := bn
	A := f64.Mat3{
		{a1.X, a1.Y, a1.Z},
		{a2.X, a2.Y, a2.Z},
		{a3.X, a3.Y, a3.Z},
	}
	AI := A
	AI.Inverse()

	v1 := f64.Vec3{s.UV[0][1] - s.UV[0][0], s.UV[0][2] - s.UV[0][0], 0}
	v2 := f64.Vec3{s.UV[1][1] - s.UV[1][0], s.UV[1][2] - s.UV[1][0], 0}

	i := AI.Transform(v1)
	j := AI.Transform(v2)

	B := f64.Mat3{}
	B.SetCol(0, i.Normalize())
	B.SetCol(1, j.Normalize())
	B.SetCol(2, bn)

	diff := 1.0
	found, n := normal(m, uv)
	if enableBump && found {
		n = B.Transform(n).Normalize()
		diff = math.Max(0, n.Dot(lightDir))
	}

	col = f64.Vec4{uv.X, uv.Y, uv.Z, 1}.ToRGBA()
	found, diffuse_ := diffuse(m, uv)
	if enableTexture && found {
		col = diffuse_
	}
	col = scaleCol(col, diff)

	return
}

func scaleCol(col color.RGBA, s float64) color.RGBA {
	return color.RGBA{
		uint8(f64.Clamp(float64(col.R)*s, 0, 255)),
		uint8(f64.Clamp(float64(col.G)*s, 0, 255)),
		uint8(f64.Clamp(float64(col.B)*s, 0, 255)),
		col.A,
	}
}

func normal(m *obj.Model, uv f64.Vec4) (found bool, nrm f64.Vec3) {
	if len(m.Mats) == 0 || m.Mats[0].Texture.Bump == nil {
		return
	}
	im := m.Mats[0].Texture.Bump.Map
	if im != nil {
		r := im.Bounds()
		u := int(uv.X * float64(r.Dx()))
		v := int((1 - uv.Y) * float64(r.Dy()))
		col := im.RGBAAt(u, v)
		nrm = f64.Vec3{
			float64(col.R)/255*2 - 1,
			float64(col.G)/255*2 - 1,
			float64(col.B)/255*2 - 1,
		}
		found = true
	}
	return
}

func diffuse(m *obj.Model, uv f64.Vec4) (found bool, col color.RGBA) {
	if len(m.Mats) == 0 || m.Mats[0].Texture.Diffuse == nil {
		return
	}
	im := m.Mats[0].Texture.Diffuse.Map
	if im != nil {
		r := im.Bounds()
		u := int(uv.X * float64(r.Dx()))
		v := int((1 - uv.Y) * float64(r.Dy()))
		col = im.RGBAAt(u, v)
		found = true
	}
	return
}

func computeSphereVector(x, y int) f64.Vec3 {
	// set the radius of the arcball, controls speed of rotation
	iwidth, iheight, _ := renderer.OutputSize()
	width, height := float64(iwidth), float64(iheight)
	gizmoR := width / 2.0

	pX := (float64(x) - width/2.0) / gizmoR
	pY := (float64(y) - height/2.0) / gizmoR
	L2 := pX*pX + pY*pY
	if L2 >= 1 {
		// interpret the click as being on the boundary
		return f64.Vec3{pX, pY, 0}.Normalize()
	} else {
		pZ := math.Sqrt(1 - L2)
		return f64.Vec3{pX, pY, pZ}
	}
}

// LookAt function translates to the center, not the eye.
// opengl lookat function does the opposite where it translates based on eye
func LookAt(eye, center, up f64.Vec3) f64.Mat4 {
	z := eye.Sub(center).Normalize()
	x := up.Cross(z).Normalize()
	y := z.Cross(x)
	t := Translate(center.Neg())
	m := f64.Mat4{
		{x.X, x.Y, x.Z, 0},
		{y.X, y.Y, y.Z, 0},
		{z.X, z.Y, z.Z, 0},
		{0, 0, 0, 1},
	}
	m.Mul(&m, &t)
	return m
}

func Perspective(coeff float64) f64.Mat4 {
	w, h, _ := renderer.OutputSize()
	aspect := float64(w) / float64(h)
	return f64.Mat4{
		{1 / aspect, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{0, 0, coeff, 1},
	}
}

func Viewport(x, y, w, h int) f64.Mat4 {
	hw := float64(w) / 2
	hh := float64(h) / 2
	return f64.Mat4{
		{hw, 0, 0, float64(x) + hw},
		{0, hh, 0, float64(y) + hh},
		{0, 0, 1, 0},
		{0, 0, 0, 1},
	}
}

func Translate(v f64.Vec3) f64.Mat4 {
	return f64.Mat4{
		{1, 0, 0, v.X},
		{0, 1, 0, v.Y},
		{0, 0, 1, v.Z},
		{0, 0, 0, 1},
	}
}
