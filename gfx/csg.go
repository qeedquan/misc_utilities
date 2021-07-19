// ported from
// https://evanw.github.io/csg.js/
// https://github.com/dabroz/csgjs-cpp/
package main

import (
	"image"
	"image/color"
	"log"
	"math"
	"os"
	"runtime"

	"github.com/qeedquan/go-media/drc"
	"github.com/qeedquan/go-media/math/f64"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlimage/sdlcolor"
)

var (
	window     *sdl.Window
	renderer   *sdl.Renderer
	texture    *sdl.Texture
	canvas     *image.RGBA
	modelview  f64.Mat4
	projection f64.Mat4
	viewport   f64.Mat4
	dcg        *drc.Context
	arcball    Arcball
	camera     struct {
		position    f64.Vec3
		orientation f64.Quat
	}
	models []Model
)

func main() {
	runtime.LockOSThread()
	log.SetFlags(0)
	log.SetPrefix("csg: ")
	initSDL()
	reset()
	for {
		event()
		blit()
	}
}

func initSDL() {
	err := sdl.Init(sdl.INIT_VIDEO)
	ck(err)

	w, h := 1280, 800
	wflag := sdl.WINDOW_RESIZABLE
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, wflag)
	ck(err)

	window.SetTitle("CSG")

	resizeWindow(w, h)
}

func reset() {
	camera.position = f64.Vec3{0, 0, -1}
	camera.orientation = f64.Quat{}.FromAxisAngle(f64.Vec3{0, 1, 0}, math.Pi/4)

	a := MakeCube(f64.Vec3{}, 1)
	b := MakeSphere(f64.Vec3{}, 1.35, 12, 8)
	c := MakeCylinder(f64.Vec3{-1, 0, 0}, f64.Vec3{1, 0, 0}, 0.8, 12)
	d := MakeCylinder(f64.Vec3{0, -1, 0}, f64.Vec3{0, 1, 0}, 0.8, 12)
	e := MakeCylinder(f64.Vec3{0, 0, -1}, f64.Vec3{0, 0, 1}, 0.8, 12)
	r := a.Intersect(b).Subtract(c.Union(d).Union(e))
	models = append(models, r)

	setupProjection()
}

func resizeWindow(w, h int) {
	var err error
	texture, err = renderer.CreateTexture(sdl.PIXELFORMAT_ABGR8888, sdl.TEXTUREACCESS_STREAMING, w, h)
	ck(err)

	canvas = image.NewRGBA(image.Rect(0, 0, w, h))
	dcg = drc.New(canvas)
}

func setupProjection() {
	w, h, _ := renderer.OutputSize()

	arcball.Window = f64.Rect(0, 0, float64(w), float64(h))

	var translate f64.Mat4
	orientation := camera.orientation.Mat4()
	translate.Translate3(camera.position)
	modelview.Identity()
	modelview.Mul(&translate, &orientation)

	projection.Perspective(f64.Deg2Rad(90), float64(w)/float64(h), 0.2, 1000)
	viewport.Viewport(0, 0, float64(w), float64(h))
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
			}
		case sdl.MouseButtonDownEvent:
			switch ev.Button {
			case sdl.BUTTON_LEFT:
				arcball.From = arcball.Vector(int(ev.X), int(ev.Y))
			}
		case sdl.MouseMotionEvent:
			if ev.State&sdl.BUTTON_LMASK != 0 {
				arcball.To = arcball.Vector(int(ev.X), int(ev.Y))
				deltaq := arcball.Arc()
				camera.orientation = deltaq.Mul(camera.orientation)
				setupProjection()
			}
		case sdl.WindowEvent:
			switch ev.Event {
			case sdl.WINDOWEVENT_RESIZED:
				w, h := int(ev.Data[0]), int(ev.Data[1])
				resizeWindow(w, h)
			}
		}
	}
}

func blit() {
	M := f64.Mat4{}
	M.Scale(0.25, 0.25, 0.25)
	M.Mul(&modelview, &M)
	M.Mul(&projection, &M)
	M.Mul(&viewport, &M)
	dcg.SetMatrix(M)

	renderer.Clear()
	dcg.SetBackground(color.RGBA{237, 237, 237, 255})
	dcg.Clear()
	for i := range models {
		blitModel(&models[i])
	}
	texture.Update(nil, canvas.Pix, canvas.Stride)
	renderer.Copy(texture, nil, nil)
	renderer.Present()
}

func blitModel(m *Model) {
	polygons := m.Polygons()
	for _, p := range polygons {
		for i := 0; i < len(p.Vertices); i += 3 {
			blitTriangle(p.Vertices[i], p.Vertices[i+1], p.Vertices[i+2])
		}
	}
}

func blitTriangle(a, b, c Vertex) {
	dcg.SetStroke(sdlcolor.Black)
	dcg.SetLineWidth(1)
	dcg.Line3p(a.Pos, b.Pos)
	dcg.Line3p(b.Pos, c.Pos)
	dcg.Line3p(c.Pos, a.Pos)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

type Vertex struct {
	Pos    f64.Vec3
	Normal f64.Vec3
	UV     f64.Vec3
}

type Model struct {
	Vertices []Vertex
	Indices  []int
}

type Plane struct {
	Normal f64.Vec3
	W      float64
}

type Polygon struct {
	Vertices []Vertex
	Plane    Plane
}

type Node struct {
	Polygons []Polygon
	Front    *Node
	Back     *Node
	Plane    Plane
}

func MakeModel(polygons []Polygon) Model {
	m := Model{}
	idx := 0
	for _, p := range polygons {
		for j := 2; j < len(p.Vertices); j++ {
			m.Vertices = append(m.Vertices, p.Vertices[0])
			m.Vertices = append(m.Vertices, p.Vertices[j-1])
			m.Vertices = append(m.Vertices, p.Vertices[j])
			m.Indices = append(m.Indices, idx)
			m.Indices = append(m.Indices, idx+1)
			m.Indices = append(m.Indices, idx+2)
			idx += 3
		}
	}
	return m
}

func (m *Model) Polygons() []Polygon {
	var list []Polygon
	for i := 0; i < len(m.Indices); i += 3 {
		var triangle []Vertex
		for j := 0; j < 3; j++ {
			v := m.Vertices[m.Indices[i+j]]
			triangle = append(triangle, v)
		}
		list = append(list, MakePolygon(triangle))
	}
	return list
}

func (m Model) Union(n Model) Model {
	return m.op(0, n)
}

func (m Model) Intersect(n Model) Model {
	return m.op(1, n)
}

func (m Model) Subtract(n Model) Model {
	return m.op(2, n)
}

func (m Model) op(op int, n Model) Model {
	a := MakeNode(m.Polygons())
	b := MakeNode(n.Polygons())
	var c *Node
	switch op {
	case 0:
		c = a.Union(b)
	case 1:
		c = a.Intersect(b)
	case 2:
		c = a.Subtract(b)
	}
	return MakeModel(c.AllPolygons())
}

// Invert all orientation-specific data (e.g. vertex normal). Called when the
// orientation of a polygon is flipped.
func (v Vertex) Flip() Vertex {
	v.Normal = v.Normal.Neg()
	return v
}

// Create a new vertex between this vertex and `other` by linearly
// interpolating all properties using a parameter of `t`. Subclasses should
// override this to interpolate additional properties.
func (v Vertex) Interpolate(t float64, u Vertex) Vertex {
	return Vertex{
		Pos:    v.Pos.Lerp(t, u.Pos),
		Normal: v.Normal.Lerp(t, u.Normal),
		UV:     v.UV.Lerp(t, u.UV),
	}
}

func MakePlane(a, b, c f64.Vec3) Plane {
	u := b.Sub(a)
	v := c.Sub(a)
	n := u.Cross(v)
	n = n.Normalize()

	return Plane{
		Normal: n,
		W:      n.Dot(a),
	}
}

func (p *Plane) Flip() {
	p.Normal = p.Normal.Neg()
	p.W = -p.W
}

func (p *Plane) OK() bool {
	return p.Normal.Len() > 0
}

// Split `polygon` by this plane if needed, then put the polygon or polygon
// fragments in the appropriate lists. Coplanar polygons go into either
// `coplanarFront` or `coplanarBack` depending on their orientation with
// respect to this plane. Polygons in front or in back of this plane go into
// either `front` or `back`.
func (p Plane) SplitPolygon(polygon Polygon) (coplanarFront, coplanarBack, front, back []Polygon) {
	const EPSILON = 0.00001
	const (
		COPLANAR = iota
		FRONT
		BACK
		SPANNING
	)

	// Classify each point as well as the entire polygon into one of the above
	// four classes.
	var (
		polygonType int
		types       []int
	)
	for _, v := range polygon.Vertices {
		t := p.Normal.Dot(v.Pos) - p.W
		typ := COPLANAR
		if t < -EPSILON {
			typ = BACK
		} else if t > EPSILON {
			typ = FRONT
		}
		polygonType |= typ
		types = append(types, typ)
	}

	// Put the polygon in the correct list, splitting it when necessary.
	switch polygonType {
	case COPLANAR:
		t := p.Normal.Dot(polygon.Plane.Normal)
		if t > 0 {
			coplanarFront = append(coplanarFront, polygon)
		} else {
			coplanarBack = append(coplanarBack, polygon)
		}
	case FRONT:
		front = append(front, polygon)
	case BACK:
		back = append(back, polygon)
	case SPANNING:
		var f, b []Vertex
		for i := range polygon.Vertices {
			j := (i + 1) % len(polygon.Vertices)
			ti := types[i]
			tj := types[j]
			vi := polygon.Vertices[i]
			vj := polygon.Vertices[j]
			if ti != BACK {
				f = append(f, vi)
			}
			if ti != FRONT {
				b = append(b, vi)
			}
			if (ti | tj) == SPANNING {
				nu := p.W - p.Normal.Dot(vi.Pos)
				dn := p.Normal.Dot(vj.Pos.Sub(vi.Pos))
				t := nu / dn
				v := vi.Interpolate(t, vj)
				f = append(f, v)
				b = append(b, v)
			}
		}
		if len(f) >= 3 {
			front = append(front, MakePolygon(f))
		}
		if len(b) >= 3 {
			back = append(back, MakePolygon(b))
		}
	}
	return
}

func MakePolygon(list []Vertex) Polygon {
	p := Polygon{
		Vertices: list,
	}
	p.Plane = MakePlane(p.Vertices[0].Pos, p.Vertices[1].Pos, p.Vertices[2].Pos)
	return p
}

func (p *Polygon) Flip() {
	for i := 0; i < len(p.Vertices)/2; i++ {
		p.Vertices[i], p.Vertices[len(p.Vertices)-1-i] = p.Vertices[len(p.Vertices)-1-i], p.Vertices[i]
	}
	for i := range p.Vertices {
		p.Vertices[i].Normal = p.Vertices[i].Normal.Neg()
	}
	p.Plane.Flip()
}

func MakeNode(polygon []Polygon) *Node {
	n := &Node{}
	n.Build(polygon)
	return n
}

// Return a new CSG solid representing space in either this solid or in the
// solid `csg`. Neither this solid nor the solid `csg` are modified.
func (n *Node) Union(m *Node) *Node {
	a := n.Clone()
	b := m.Clone()
	a.ClipTo(b)
	b.ClipTo(a)
	b.Invert()
	b.ClipTo(a)
	b.Invert()
	a.Build(b.AllPolygons())
	return MakeNode(a.AllPolygons())
}

// Return a new CSG solid representing space in this solid but not in the
// solid `csg`. Neither this solid nor the solid `csg` are modified.
func (n *Node) Subtract(m *Node) *Node {
	a := n.Clone()
	b := m.Clone()
	a.Invert()
	a.ClipTo(b)
	b.ClipTo(a)
	b.Invert()
	b.ClipTo(a)
	b.Invert()
	a.Build(b.AllPolygons())
	a.Invert()
	return MakeNode(a.AllPolygons())
}

// Return a new CSG solid representing space both this solid and in the
// solid `csg`. Neither this solid nor the solid `csg` are modified.
func (n *Node) Intersect(m *Node) *Node {
	a := n.Clone()
	b := m.Clone()
	a.Invert()
	b.ClipTo(a)
	b.Invert()
	a.ClipTo(b)
	b.ClipTo(a)
	a.Build(b.AllPolygons())
	a.Invert()
	return MakeNode(a.AllPolygons())
}

// Remove all polygons in this BSP tree that are inside the other BSP tree
// `bsp`.
func (n *Node) ClipTo(other *Node) {
	nodes := []*Node{n}
	for len(nodes) > 0 {
		me := nodes[0]
		nodes = nodes[1:]

		me.Polygons = other.ClipPolygons(me.Polygons)
		if me.Front != nil {
			nodes = append(nodes, me.Front)
		}
		if me.Back != nil {
			nodes = append(nodes, me.Back)
		}
	}
}

// Recursively remove all polygons in `polygons` that are inside this BSP
// tree.
func (n *Node) ClipPolygons(list []Polygon) []Polygon {
	var result []Polygon

	type clip struct {
		node    *Node
		polygon []Polygon
	}
	clips := []clip{{n, list}}
	for len(clips) > 0 {
		me := clips[0].node
		list := clips[0].polygon
		clips = clips[1:]

		if !me.Plane.OK() {
			result = append(result, list...)
			continue
		}

		var listFront, listBack []Polygon
		for i := range list {
			coplanarFront, coplanarBack, front, back := me.Plane.SplitPolygon(list[i])
			listFront = append(listFront, coplanarFront...)
			listFront = append(listFront, front...)
			listBack = append(listBack, coplanarBack...)
			listBack = append(listBack, back...)
		}
		if me.Front != nil {
			clips = append(clips, clip{me.Front, listFront})
		} else {
			result = append(result, listFront...)
		}
		if me.Back != nil {
			clips = append(clips, clip{me.Back, listBack})
		}
	}
	return result
}

// Convert solid space to empty space and empty space to solid space.
func (n *Node) Invert() {
	nodes := []*Node{n}
	for len(nodes) > 0 {
		me := nodes[0]
		nodes = nodes[1:]

		for i := range me.Polygons {
			me.Polygons[i].Flip()
		}
		me.Plane.Flip()
		me.Front, me.Back = me.Back, me.Front
		if me.Front != nil {
			nodes = append(nodes, me.Front)
		}
		if me.Back != nil {
			nodes = append(nodes, me.Back)
		}
	}
}

// Return a list of all polygons in this BSP tree.
func (n *Node) AllPolygons() []Polygon {
	var result []Polygon
	nodes := []*Node{n}
	for len(nodes) > 0 {
		me := nodes[0]
		nodes = nodes[1:]

		result = append(result, me.Polygons...)
		if me.Front != nil {
			nodes = append(nodes, me.Front)
		}
		if me.Back != nil {
			nodes = append(nodes, me.Back)
		}
	}
	return result
}

// Build a BSP tree out of `polygons`. When called on an existing tree, the
// new polygons are filtered down to the bottom of the tree and become new
// nodes there. Each set of polygons is partitioned using the first polygon
// (no heuristic is used to pick a good split).
func (n *Node) Build(list []Polygon) {
	if len(list) == 0 {
		return
	}

	type build struct {
		node    *Node
		polygon []Polygon
	}
	builds := []build{{n, list}}
	for len(builds) > 0 {
		me := builds[0].node
		list := builds[0].polygon
		builds = builds[1:]

		if !me.Plane.OK() {
			me.Plane = list[0].Plane
		}

		var listFront, listBack []Polygon
		for i := range list {
			coplanarFront, coplanarBack, front, back := me.Plane.SplitPolygon(list[i])
			listFront = append(listFront, front...)
			listBack = append(listBack, back...)
			me.Polygons = append(me.Polygons, coplanarFront...)
			me.Polygons = append(me.Polygons, coplanarBack...)
		}
		if len(listFront) > 0 {
			if me.Front == nil {
				me.Front = &Node{}
			}
			builds = append(builds, build{me.Front, listFront})
		}
		if len(listBack) > 0 {
			if me.Back == nil {
				me.Back = &Node{}
			}
			builds = append(builds, build{me.Back, listBack})
		}
	}
}

func (n *Node) Clone() *Node {
	ret := &Node{}
	nodes := [][2]*Node{{n, ret}}
	for len(nodes) > 0 {
		original := nodes[0][0]
		clone := nodes[0][1]
		nodes = nodes[1:]

		clone.Polygons = append(clone.Polygons, original.Polygons...)
		clone.Plane = original.Plane
		if original.Front != nil {
			clone.Front = &Node{}
			nodes = append(nodes, [2]*Node{original.Front, clone.Front})
		}
		if original.Back != nil {
			clone.Back = &Node{}
			nodes = append(nodes, [2]*Node{original.Back, clone.Back})
		}
	}
	return ret
}

func MakeCube(center f64.Vec3, radius float64) Model {
	c := center
	r := f64.Vec3{radius, radius, radius}
	indices := [][]int{
		{0, 4, 6, 2},
		{1, 3, 7, 5},
		{0, 1, 5, 4},
		{2, 6, 7, 3},
		{0, 2, 3, 1},
		{4, 5, 7, 6},
	}
	normals := []f64.Vec3{
		{-1, 0, 0},
		{+1, 0, 0},
		{0, -1, 0},
		{0, +1, 0},
		{0, 0, -1},
		{0, 0, +1},
	}

	var polygons []Polygon
	for j := range indices {
		var vertices []Vertex
		for _, i := range indices[j] {
			pos := f64.Vec3{
				c.X + r.X*(2*b2f((i&1) != 0)-1),
				c.Y + r.Y*(2*b2f((i&2) != 0)-1),
				c.Z + r.Z*(2*b2f((i&4) != 0)-1),
			}
			vert := Vertex{Pos: pos, Normal: normals[j]}
			vertices = append(vertices, vert)
		}
		polygons = append(polygons, MakePolygon(vertices))
	}
	return MakeModel(polygons)
}

func b2f(b bool) float64 {
	if b {
		return 1
	}
	return 0
}

func MakeSphere(center f64.Vec3, radius float64, slices, stacks int) Model {
	c := center
	r := radius

	var vertices []Vertex
	vertex := func(theta, phi float64) {
		theta *= 2 * math.Pi
		phi *= math.Pi
		dir := f64.Vec3{
			math.Cos(theta) * math.Sin(phi),
			math.Cos(phi),
			math.Sin(theta) * math.Sin(phi),
		}
		vertices = append(vertices, Vertex{Pos: c.Add(dir.Scale(r)), Normal: dir})
	}

	var polygons []Polygon
	for i := 0; i < slices; i++ {
		for j := 0; j < stacks; j++ {
			vertices = make([]Vertex, 0)
			vertex(float64(i)/float64(slices), float64(j)/float64(stacks))
			if j > 0 {
				vertex(float64(i+1)/float64(slices), float64(j)/float64(stacks))
			}
			if j < stacks-1 {
				vertex(float64(i+1)/float64(slices), float64(j+1)/float64(stacks))
			}
			vertex(float64(i)/float64(slices), float64(j+1)/float64(stacks))
			polygons = append(polygons, MakePolygon(vertices))
		}
	}
	return MakeModel(polygons)
}

func MakeCylinder(start, end f64.Vec3, radius float64, slices int) Model {
	s := start
	e := end
	ray := e.Sub(s)
	r := radius
	axisZ := ray.Normalize()
	isY := math.Abs(axisZ.Y) > 0.5
	axisX := f64.Vec3{b2f(isY), b2f(!isY), 0}.Cross(axisZ).Normalize()
	axisY := axisX.Cross(axisZ).Normalize()
	startV := Vertex{Pos: s, Normal: axisZ.Neg()}
	endV := Vertex{Pos: e, Normal: axisZ.Normalize()}
	point := func(stack, slice float64, normalBlend float64) Vertex {
		angle := slice * 2 * math.Pi
		out := axisX.Scale(math.Cos(angle)).Add(axisY.Scale(math.Sin(angle)))
		pos := s.Add(ray.Scale(float64(stack)).Add(out.Scale(r)))
		normal := out.Scale(1 - math.Abs(normalBlend)).Add(axisZ.Scale(normalBlend))
		return Vertex{Pos: pos, Normal: normal}
	}
	var polygons []Polygon
	for i := 0; i < slices; i++ {
		t0 := float64(i) / float64(slices)
		t1 := float64(i+1) / float64(slices)

		polygons = append(polygons, MakePolygon([]Vertex{startV, point(0, t0, -1), point(0, t1, -1)}))
		polygons = append(polygons, MakePolygon([]Vertex{point(0, t1, 0), point(0, t0, 0), point(1, t0, 0), point(1, t1, 0)}))
		polygons = append(polygons, MakePolygon([]Vertex{endV, point(1, t1, 1), point(1, t0, 1)}))
	}
	return MakeModel(polygons)
}

type Arcball struct {
	From, To f64.Vec3
	Window   f64.Rectangle
}

func (c *Arcball) Arc() f64.Quat {
	from := c.From
	to := c.To

	cross := from.Cross(to)
	dot := from.Dot(to)
	return f64.Quat{cross.X, cross.Y, cross.Z, dot}
}

func (c *Arcball) Vector(x, y int) f64.Vec3 {
	width, height := c.Window.Dx(), c.Window.Dy()
	gizmoR := width / 4.0

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
