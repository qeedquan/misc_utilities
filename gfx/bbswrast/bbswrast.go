// ported from https://github.com/BennyQBD/3DSoftwareRenderer
package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	"log"
	"math"
	"os"
	"runtime"
	"runtime/pprof"
	"time"

	"github.com/qeedquan/go-media/image/imageutil"
	"github.com/qeedquan/go-media/image/obj"
	"github.com/qeedquan/go-media/math/ga"
	"github.com/qeedquan/go-media/math/ga/mat4"
	"github.com/qeedquan/go-media/math/ga/quat"
	"github.com/qeedquan/go-media/math/ga/vec3"
	"github.com/qeedquan/go-media/math/ga/vec4"
	"github.com/qeedquan/go-media/sdl"
)

var (
	run bool
)

func main() {
	runtime.LockOSThread()

	showDeltaTicks := flag.Bool("b", false, "show delta ticks")
	cpuProfile := flag.String("c", "", "profile cpu")
	flag.Parse()
	texture := loadImage("res/bricks.jpg")
	texture2 := loadImage("res/bricks2.jpg")

	monkeyMesh, err := LoadMesh("res/smoothMonkey0.obj")
	ck(err)

	terrainMesh, err := LoadMesh("res/terrain2.obj")
	ck(err)

	width, height := 800, 600
	display, err := NewDisplay(width, height, "Software Rendering")
	ck(err)

	monkeyTransform := DefaultTransform
	monkeyTransform.Position = ga.Vec4d{0, 0, 3, 0}

	terrainTransform := DefaultTransform
	terrainTransform.Position = ga.Vec4d{0, -1, 0, 0}

	projection := Perspective(ga.Deg2Rad(70.0), float64(width)/float64(height), 0.1, 1000, 1)
	camera := Camera{Transform: DefaultTransform, Projection: projection}

	if *cpuProfile != "" {
		f, err := os.Create(*cpuProfile)
		ck(err)
		defer f.Close()
		err = pprof.StartCPUProfile(f)
		ck(err)
		defer pprof.StopCPUProfile()
	}

	run = true
	paused := false
	target := display.Framebuffer
	previousTime := time.Now()
	for run {
		currentTime := time.Now()
		delta := float64(currentTime.Sub(previousTime).Nanoseconds()) / 1e9
		if *showDeltaTicks {
			fmt.Println(delta)
		}
		previousTime = currentTime

		display.Event(&camera, &paused, delta)

		if !paused {
			rot := quat.AxisAngle(ga.Vec3d{0, 1, 0}, delta)
			monkeyTransform = monkeyTransform.Rotate(rot)
		}

		vp := camera.GetViewProjection()
		monkeyMatrix := monkeyTransform.GetTransformation()
		terrainMatrix := terrainTransform.GetTransformation()

		display.Clear()
		target.Clear(color.RGBA{50, 50, 50, 255})
		target.ClearDepthBuffer()
		monkeyMesh.Draw(target, &vp, &monkeyMatrix, texture2)
		terrainMesh.Draw(target, &vp, &terrainMatrix, texture)
		display.Texture.Update(nil, target.Pix, target.Stride)
		display.Copy(display.Texture, nil, nil)
		display.Present()
	}
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func loadImage(name string) *image.RGBA {
	m, err := imageutil.LoadRGBAFile(name)
	ck(err)
	return m
}

type Display struct {
	*sdl.Window
	*sdl.Renderer
	Texture     *sdl.Texture
	Framebuffer *RenderContext
}

func NewDisplay(width, height int, title string) (*Display, error) {
	wflag := sdl.WINDOW_RESIZABLE
	window, renderer, err := sdl.CreateWindowAndRenderer(width, height, wflag)
	if err != nil {
		return nil, err
	}

	texture, err := renderer.CreateTexture(sdl.PIXELFORMAT_ABGR8888, sdl.TEXTUREACCESS_STREAMING, width, height)
	if err != nil {
		return nil, err
	}

	window.SetTitle(title)

	return &Display{
		Window:      window,
		Renderer:    renderer,
		Texture:     texture,
		Framebuffer: NewRenderContext(width, height),
	}, nil
}

func (d *Display) Event(camera *Camera, paused *bool, delta float64) {
	for {
		ev := sdl.PollEvent()
		if ev == nil {
			break
		}
		switch ev := ev.(type) {
		case sdl.QuitEvent:
			run = false
		case sdl.KeyDownEvent:
			switch ev.Sym {
			case sdl.K_ESCAPE:
				run = false
			case sdl.K_SPACE:
				*paused = !*paused
			default:
				camera.Update(&ev, delta)
			}
		}
	}
}

type Transform struct {
	Position ga.Vec4d
	Rotation ga.Quatd
	Scale    ga.Vec4d
}

var DefaultTransform = Transform{
	Rotation: ga.Quatd{0, 0, 0, 1},
	Scale:    ga.Vec4d{1, 1, 1, 1},
}

func (p *Transform) Rotate(rot ga.Quatd) Transform {
	return Transform{
		Position: p.Position,
		Rotation: quat.Normalize(quat.Mul(rot, p.Rotation)),
		Scale:    p.Scale,
	}
}

func (p *Transform) GetTransformation() ga.Mat4d {
	var (
		translation ga.Mat4d
		rotation    ga.Mat4d
		scale       ga.Mat4d
		result      ga.Mat4d
	)
	translation = mat4.Translation(p.Position.X, p.Position.Y, p.Position.Z)
	rotation = quat.Matrix(p.Rotation)
	scale = mat4.Scale(p.Scale.X, p.Scale.Y, p.Scale.Z)

	mat4.Mul(&result, &rotation, &scale)
	mat4.Mul(&result, &translation, &result)
	return result
}

type Camera struct {
	Transform  Transform
	Projection ga.Mat4d
}

func (c *Camera) Update(ev *sdl.KeyDownEvent, delta float64) {
	var (
		sensitivityX = 2.66 * delta
		sensitivityY = 2.0 * delta
		movAmt       = 5.0 * delta
	)

	forward, _, up, _, left, right := CardinalDirections(c.Transform.Rotation)

	switch ev.Sym {
	case sdl.K_w:
		c.Move(forward, movAmt)
	case sdl.K_s:
		c.Move(forward, -movAmt)
	case sdl.K_a:
		c.Move(left, movAmt)
	case sdl.K_d:
		c.Move(right, movAmt)

	case sdl.K_RIGHT:
		c.Rotate(up, sensitivityX)
	case sdl.K_LEFT:
		c.Rotate(up, -sensitivityX)
	case sdl.K_DOWN:
		c.Rotate(right, sensitivityY)
	case sdl.K_UP:
		c.Rotate(right, -sensitivityY)
	}
}

func (c *Camera) Move(dir ga.Vec4d, amt float64) {
	c.Transform.Position = vec4.Fmas(c.Transform.Position, dir, amt)
}

func (c *Camera) Rotate(axis ga.Vec4d, angle float64) {
	rot := quat.AxisAngle(vec4.Vec3(axis), angle)
	c.Transform = c.Transform.Rotate(rot)
}

func (c *Camera) GetViewProjection() ga.Mat4d {
	// camera view projection is the inverse matrix
	// of the viewpoint matrix, and since a
	// transform consists of a rotation + scale matrix, we do
	// an inverse on both of them and multiply them together
	// to get an inverse matrix

	var (
		translation ga.Mat4d
		result      ga.Mat4d
	)
	rotation := quat.Matrix(quat.Inv(c.Transform.Rotation))
	position := vec4.Scale(c.Transform.Position, -1)
	translation = mat4.Translation(position.X, position.Y, position.Z)

	mat4.Mul(&result, &rotation, &translation)
	mat4.Mul(&result, &c.Projection, &result)
	return result
}

type Vertex struct {
	Position ga.Vec4d
	Texcoord ga.Vec4d
	Normal   ga.Vec4d
}

func (v *Vertex) Get(i int) float64 {
	switch i {
	case 0:
		return v.Position.X
	case 1:
		return v.Position.Y
	case 2:
		return v.Position.Z
	case 3:
		return v.Position.W
	}
	panic("unreachable")
}

func (v *Vertex) InsideViewFrustum() bool {
	p := &v.Position
	return math.Abs(p.X) <= math.Abs(p.W) &&
		math.Abs(p.Y) <= math.Abs(p.W) &&
		math.Abs(p.Z) <= math.Abs(p.W)
}

func (v *Vertex) PerspectiveDivide() {
	// preserve the w instead of setting it to 1
	// so we have extra information for depth culling
	v.Position.X /= v.Position.W
	v.Position.Y /= v.Position.W
	v.Position.Z /= v.Position.W
}

func (v *Vertex) Transform(transform, normalTransform *ga.Mat4d) Vertex {
	return Vertex{
		Position: mat4.Apply(transform, v.Position),
		Texcoord: v.Texcoord,
		Normal:   vec4.Normalize(mat4.Apply(normalTransform, v.Normal)),
	}
}

func (a *Vertex) TriangleAreaTimesTwo(b, c *Vertex) float64 {
	x1 := b.Position.X - a.Position.X
	y1 := b.Position.Y - a.Position.Y

	x2 := c.Position.X - a.Position.X
	y2 := c.Position.Y - a.Position.Y

	return x1*y2 - x2*y1
}

func (v *Vertex) Lerp(other *Vertex, lerpAmt float64) *Vertex {
	return &Vertex{
		vec4.Lerp(lerpAmt, v.Position, other.Position),
		vec4.Lerp(lerpAmt, v.Texcoord, other.Texcoord),
		vec4.Lerp(lerpAmt, v.Normal, other.Normal),
	}
}

func (v Vertex) String() string {
	return fmt.Sprintf("%v %v %v", v.Position, v.Texcoord, v.Normal)
}

type RenderContext struct {
	*image.RGBA
	zbuffer []float64
	width   int
	height  int
}

func NewRenderContext(width, height int) *RenderContext {
	return &RenderContext{
		RGBA:    image.NewRGBA(image.Rect(0, 0, width, height)),
		zbuffer: make([]float64, width*height),
		width:   width,
		height:  height,
	}
}

func (c *RenderContext) Clear(col color.RGBA) {
	r := c.Bounds()
	l := r.Dx() * r.Dy() * 4
	s := c.Pix[0:l:l]
	for y := r.Min.Y; y < r.Max.Y; y++ {
		for x := r.Min.X; x < r.Max.X; x++ {
			s[0] = col.R
			s[1] = col.G
			s[2] = col.B
			s[3] = col.A
			s = s[4:]
		}
	}
}

func (c *RenderContext) ClearDepthBuffer() {
	for i := range c.zbuffer {
		c.zbuffer[i] = math.MaxFloat32
	}
}

func (c *RenderContext) DrawTriangle(v1, v2, v3 *Vertex, texture *image.RGBA) {
	if v1.InsideViewFrustum() && v2.InsideViewFrustum() && v3.InsideViewFrustum() {
		c.fillTriangle(v1, v2, v3, texture)
		return
	}
	vertices := []*Vertex{v1, v2, v3}

	var auxillaryList []*Vertex
	if c.clipPolygonAxis(&vertices, &auxillaryList, 0) &&
		c.clipPolygonAxis(&vertices, &auxillaryList, 1) &&
		c.clipPolygonAxis(&vertices, &auxillaryList, 2) {

		initialVertex := vertices[0]
		for i := 1; i < len(vertices)-1; i++ {
			c.fillTriangle(initialVertex, vertices[i], vertices[i+1], texture)
		}
	}
}

func (c *RenderContext) clipPolygonAxis(vertices *[]*Vertex, auxillaryList *[]*Vertex, componentIndex int) bool {
	c.clipPolygonComponent(vertices, componentIndex, 1, auxillaryList)
	*vertices = (*vertices)[:0]

	if len(*auxillaryList) == 0 {
		return false
	}

	c.clipPolygonComponent(auxillaryList, componentIndex, -1, vertices)
	*auxillaryList = (*auxillaryList)[:0]

	return len(*vertices) > 0
}

func (c *RenderContext) clipPolygonComponent(vertices *[]*Vertex, componentIndex int, componentFactor float64, result *[]*Vertex) {
	previousVertex := (*vertices)[len(*vertices)-1]
	previousComponent := previousVertex.Get(componentIndex) * componentFactor
	previousInside := previousComponent <= previousVertex.Position.W

	for _, currentVertex := range *vertices {
		currentComponent := currentVertex.Get(componentIndex) * componentFactor
		currentInside := currentComponent <= currentVertex.Position.W
		if currentInside && !previousInside || !currentInside && previousInside {
			lerpAmt := (previousVertex.Position.W - previousComponent) /
				((previousVertex.Position.W - previousComponent) -
					(currentVertex.Position.W - currentComponent))
			*result = append(*result, previousVertex.Lerp(currentVertex, lerpAmt))
		}

		if currentInside {
			*result = append(*result, currentVertex)
		}

		previousVertex = currentVertex
		previousComponent = currentComponent
		previousInside = currentInside
	}
}

func (c *RenderContext) fillTriangle(v1, v2, v3 *Vertex, texture *image.RGBA) {
	screenSpaceTransform := ScreenSpaceTransform(float64(c.width)/2, float64(c.height)/2)
	identity := mat4.Diagonal(1.0)

	minYVert := v1.Transform(&screenSpaceTransform, &identity)
	midYVert := v2.Transform(&screenSpaceTransform, &identity)
	maxYVert := v3.Transform(&screenSpaceTransform, &identity)

	minYVert.PerspectiveDivide()
	midYVert.PerspectiveDivide()
	maxYVert.PerspectiveDivide()

	// if the winding orientation is clockwise, don't draw
	if minYVert.TriangleAreaTimesTwo(&maxYVert, &midYVert) >= 0 {
		return
	}

	// sort edges by y coordinates
	if maxYVert.Position.Y < midYVert.Position.Y {
		maxYVert, midYVert = midYVert, maxYVert
	}
	if midYVert.Position.Y < minYVert.Position.Y {
		midYVert, minYVert = minYVert, midYVert
	}
	if maxYVert.Position.Y < midYVert.Position.Y {
		maxYVert, midYVert = midYVert, maxYVert
	}

	c.scanTriangle(&minYVert, &midYVert, &maxYVert,
		minYVert.TriangleAreaTimesTwo(&maxYVert, &midYVert) >= 0,
		texture)
}

func (c *RenderContext) scanTriangle(minYVert, midYVert, maxYVert *Vertex, handedness bool, texture *image.RGBA) {
	gradients := NewGradients(minYVert, midYVert, maxYVert)
	topToBottom := NewEdge(gradients, minYVert, maxYVert, 0)
	topToMiddle := NewEdge(gradients, minYVert, midYVert, 0)
	middleToBottom := NewEdge(gradients, midYVert, maxYVert, 1)

	c.scanEdges(gradients, topToBottom, topToMiddle, handedness, texture)
	c.scanEdges(gradients, topToBottom, middleToBottom, handedness, texture)
}

func (c *RenderContext) scanEdges(gradients *Gradients, a *Edge, b *Edge, handedness bool, texture *image.RGBA) {
	left := a
	right := b
	if handedness {
		left, right = right, left
	}

	yStart := b.yStart
	yEnd := b.yEnd
	for j := yStart; j < yEnd; j++ {
		c.drawScanLine(gradients, left, right, j, texture)
		left.Step()
		right.Step()
	}
}

func (c *RenderContext) drawScanLine(gradients *Gradients, left, right *Edge, j int, texture *image.RGBA) {
	xMin := int(math.Ceil(left.x))
	xMax := int(math.Ceil(right.x))
	xPrestep := float64(xMin) - left.x

	texCoordXXStep := gradients.GetTexCoordXXStep()
	texCoordYXStep := gradients.GetTexCoordYXStep()
	oneOverZXStep := gradients.GetOneOverZXStep()
	depthXStep := gradients.GetDepthXStep()
	lightAmtXStep := gradients.GetLightAmtXStep()

	texCoordX := left.texCoordX + texCoordXXStep*xPrestep
	texCoordY := left.texCoordY + texCoordYXStep*xPrestep
	oneOverZ := left.oneOverZ + oneOverZXStep*xPrestep
	depth := left.depth + depthXStep*xPrestep
	lightAmt := left.lightAmt + lightAmtXStep*xPrestep

	for i := xMin; i < xMax; i++ {
		index := i + j*c.width
		if depth < c.zbuffer[index] {
			c.zbuffer[index] = depth
			z := 1.0 / oneOverZ
			srcX := int((texCoordX*z)*float64(texture.Bounds().Dx()-1) + 0.5)
			srcY := int((texCoordY*z)*float64(texture.Bounds().Dy()-1) + 0.5)
			c.copyPixel(i, j, srcX, srcY, texture, lightAmt)
		}

		oneOverZ += oneOverZXStep
		texCoordX += texCoordXXStep
		texCoordY += texCoordYXStep
		depth += depthXStep
		lightAmt += lightAmtXStep
	}
}

func (c *RenderContext) copyPixel(destX, destY, srcX, srcY int, src *image.RGBA, lightAmt float64) {
	i := src.PixOffset(srcX, srcY)
	s := src.Pix[i : i+4 : i+4]

	j := c.PixOffset(destX, destY)
	d := c.Pix[j : j+4 : j+4]

	d[0] = uint8(float64(s[0]) * lightAmt)
	d[1] = uint8(float64(s[1]) * lightAmt)
	d[2] = uint8(float64(s[2]) * lightAmt)
	d[3] = uint8(float64(s[3]) * lightAmt)
}

type Edge struct {
	x             float64
	xStep         float64
	yStart        int
	yEnd          int
	texCoordX     float64
	texCoordXStep float64
	texCoordY     float64
	texCoordYStep float64
	oneOverZ      float64
	oneOverZStep  float64
	depth         float64
	depthStep     float64
	lightAmt      float64
	lightAmtStep  float64
}

func NewEdge(gradients *Gradients, minYVert, maxYVert *Vertex, minYVertIndex int) *Edge {
	c := &Edge{}
	c.yStart = int(math.Ceil(minYVert.Position.Y))
	c.yEnd = int(math.Ceil(maxYVert.Position.Y))

	yDist := maxYVert.Position.Y - minYVert.Position.Y
	xDist := maxYVert.Position.X - minYVert.Position.X

	yPrestep := float64(c.yStart) - minYVert.Position.Y
	c.xStep = xDist / yDist
	c.x = minYVert.Position.X + yPrestep*c.xStep
	xPrestep := c.x - minYVert.Position.X

	c.texCoordX = gradients.GetTexCoordX(minYVertIndex) +
		gradients.GetTexCoordXXStep()*xPrestep +
		gradients.GetTexCoordXYStep()*yPrestep
	c.texCoordXStep = gradients.GetTexCoordXYStep() + gradients.GetTexCoordXXStep()*c.xStep

	c.texCoordY = gradients.GetTexCoordY(minYVertIndex) +
		gradients.GetTexCoordYXStep()*xPrestep +
		gradients.GetTexCoordYYStep()*yPrestep
	c.texCoordYStep = gradients.GetTexCoordYYStep() + gradients.GetTexCoordYXStep()*c.xStep

	c.oneOverZ = gradients.GetOneOverZ(minYVertIndex) +
		gradients.GetOneOverZXStep()*xPrestep +
		gradients.GetOneOverZYStep()*yPrestep
	c.oneOverZStep = gradients.GetOneOverZYStep() + gradients.GetOneOverZXStep()*c.xStep

	c.depth = gradients.GetDepth(minYVertIndex) +
		gradients.GetDepthXStep()*xPrestep +
		gradients.GetDepthYStep()*yPrestep
	c.depthStep = gradients.GetDepthYStep() + gradients.GetDepthXStep()*c.xStep

	c.lightAmt = gradients.GetLightAmt(minYVertIndex) +
		gradients.GetLightAmtXStep()*xPrestep +
		gradients.GetLightAmtYStep()*yPrestep
	c.lightAmtStep = gradients.GetLightAmtYStep() + gradients.GetLightAmtXStep()*c.xStep
	return c
}

func (c *Edge) Step() {
	c.x += c.xStep
	c.texCoordX += c.texCoordXStep
	c.texCoordY += c.texCoordYStep
	c.oneOverZ += c.oneOverZStep
	c.depth += c.depthStep
	c.lightAmt += c.lightAmtStep
}

func (c *Edge) String() string {
	s := ""
	s += fmt.Sprintf("x %.3f ", c.x)
	s += fmt.Sprintf("xstep %.3f ", c.xStep)
	s += fmt.Sprintf("ystart %d ", c.yStart)
	s += fmt.Sprintf("yend %d ", c.yEnd)
	s += fmt.Sprintf("texcoordx %.3f ", c.texCoordX)
	s += fmt.Sprintf("texcoordxstep %.3f ", c.texCoordXStep)
	s += fmt.Sprintf("texcoordy %.3f ", c.texCoordY)
	s += fmt.Sprintf("texcoordystep %.3f ", c.texCoordYStep)
	s += fmt.Sprintf("oneoverz %.3f ", c.oneOverZ)
	s += fmt.Sprintf("oneoverzstep %.3f ", c.oneOverZStep)
	s += fmt.Sprintf("depth %.3f ", c.depth)
	s += fmt.Sprintf("depthstep %.3f ", c.depthStep)
	s += fmt.Sprintf("lightamt %.3f ", c.lightAmt)
	s += fmt.Sprintf("lightamtstep %.3f", c.lightAmtStep)
	return s
}

type Gradients struct {
	texCoordX [3]float64
	texCoordY [3]float64
	oneOverZ  [3]float64
	depth     [3]float64
	lightAmt  [3]float64

	texCoordXXStep float64
	texCoordXYStep float64
	texCoordYXStep float64
	texCoordYYStep float64
	oneOverZXStep  float64
	oneOverZYStep  float64
	depthXStep     float64
	depthYStep     float64
	lightAmtXStep  float64
	lightAmtYStep  float64
}

func (c *Gradients) GetTexCoordX(loc int) float64 { return c.texCoordX[loc] }
func (c *Gradients) GetTexCoordY(loc int) float64 { return c.texCoordY[loc] }
func (c *Gradients) GetOneOverZ(loc int) float64  { return c.oneOverZ[loc] }
func (c *Gradients) GetDepth(loc int) float64     { return c.depth[loc] }
func (c *Gradients) GetLightAmt(loc int) float64  { return c.lightAmt[loc] }

func (c *Gradients) GetTexCoordXXStep() float64 { return c.texCoordXXStep }
func (c *Gradients) GetTexCoordXYStep() float64 { return c.texCoordXYStep }
func (c *Gradients) GetTexCoordYXStep() float64 { return c.texCoordYXStep }
func (c *Gradients) GetTexCoordYYStep() float64 { return c.texCoordYYStep }
func (c *Gradients) GetOneOverZXStep() float64  { return c.oneOverZXStep }
func (c *Gradients) GetOneOverZYStep() float64  { return c.oneOverZYStep }
func (c *Gradients) GetDepthXStep() float64     { return c.depthXStep }
func (c *Gradients) GetDepthYStep() float64     { return c.depthYStep }
func (c *Gradients) GetLightAmtXStep() float64  { return c.lightAmtXStep }
func (c *Gradients) GetLightAmtYStep() float64  { return c.lightAmtYStep }

func (c *Gradients) calcXStep(values []float64, minYVert, midYVert, maxYVert *Vertex, oneOverdX float64) float64 {
	return ((values[1]-values[2])*(minYVert.Position.Y-maxYVert.Position.Y) -
		(values[0]-values[2])*(midYVert.Position.Y-maxYVert.Position.Y)) * oneOverdX
}

func (c *Gradients) calcYStep(values []float64, minYVert, midYVert, maxYVert *Vertex, oneOverdY float64) float64 {
	return ((values[1]-values[2])*(minYVert.Position.X-maxYVert.Position.X) -
		(values[0]-values[2])*(midYVert.Position.X-maxYVert.Position.X)) * oneOverdY
}

func NewGradients(minYVert, midYVert, maxYVert *Vertex) *Gradients {
	c := &Gradients{}

	oneOverdX := 1 /
		(((midYVert.Position.X - maxYVert.Position.X) * (minYVert.Position.Y - maxYVert.Position.Y)) -
			((minYVert.Position.X - maxYVert.Position.X) * (midYVert.Position.Y - maxYVert.Position.Y)))

	oneOverdY := -oneOverdX

	c.depth[0] = minYVert.Position.Z
	c.depth[1] = midYVert.Position.Z
	c.depth[2] = maxYVert.Position.Z

	lightDir := ga.Vec4d{0, 0, 1, 1}
	c.lightAmt[0] = ga.Saturate(vec4.Dot(minYVert.Normal, lightDir))*0.9 + 0.1
	c.lightAmt[1] = ga.Saturate(vec4.Dot(midYVert.Normal, lightDir))*0.9 + 0.1
	c.lightAmt[2] = ga.Saturate(vec4.Dot(maxYVert.Normal, lightDir))*0.9 + 0.1

	// W component is the perspective Z value
	// The Z component is the occlusion Z value
	c.oneOverZ[0] = 1 / minYVert.Position.W
	c.oneOverZ[1] = 1 / midYVert.Position.W
	c.oneOverZ[2] = 1 / maxYVert.Position.W

	c.texCoordX[0] = minYVert.Texcoord.X * c.oneOverZ[0]
	c.texCoordX[1] = midYVert.Texcoord.X * c.oneOverZ[1]
	c.texCoordX[2] = maxYVert.Texcoord.X * c.oneOverZ[2]

	c.texCoordY[0] = minYVert.Texcoord.Y * c.oneOverZ[0]
	c.texCoordY[1] = midYVert.Texcoord.Y * c.oneOverZ[1]
	c.texCoordY[2] = maxYVert.Texcoord.Y * c.oneOverZ[2]

	c.texCoordXXStep = c.calcXStep(c.texCoordX[:], minYVert, midYVert, maxYVert, oneOverdX)
	c.texCoordXYStep = c.calcYStep(c.texCoordX[:], minYVert, midYVert, maxYVert, oneOverdY)
	c.texCoordYXStep = c.calcXStep(c.texCoordY[:], minYVert, midYVert, maxYVert, oneOverdX)
	c.texCoordYYStep = c.calcYStep(c.texCoordY[:], minYVert, midYVert, maxYVert, oneOverdY)

	c.oneOverZXStep = c.calcXStep(c.oneOverZ[:], minYVert, midYVert, maxYVert, oneOverdX)
	c.oneOverZYStep = c.calcYStep(c.oneOverZ[:], minYVert, midYVert, maxYVert, oneOverdY)

	c.depthXStep = c.calcXStep(c.depth[:], minYVert, midYVert, maxYVert, oneOverdX)
	c.depthYStep = c.calcYStep(c.depth[:], minYVert, midYVert, maxYVert, oneOverdY)

	c.lightAmtXStep = c.calcXStep(c.lightAmt[:], minYVert, midYVert, maxYVert, oneOverdX)
	c.lightAmtYStep = c.calcYStep(c.lightAmt[:], minYVert, midYVert, maxYVert, oneOverdY)
	return c
}

func (c *Gradients) String() string {
	s := ""
	s += fmt.Sprintf("lightamt %.3f %.3f %.3f ", c.lightAmt[0], c.lightAmt[1], c.lightAmt[2])
	s += fmt.Sprintf("texcoordxxstep %.3f ", c.texCoordXXStep)
	s += fmt.Sprintf("texcoordxystep %.3f ", c.texCoordXYStep)
	s += fmt.Sprintf("texcoordyxstep %.3f ", c.texCoordYXStep)
	s += fmt.Sprintf("texcoordyystep %.3f ", c.texCoordYYStep)
	s += fmt.Sprintf("oneoverzxstep %.3f ", c.oneOverZXStep)
	s += fmt.Sprintf("oneoverzystep %.3f ", c.oneOverZYStep)
	s += fmt.Sprintf("depthxstep %.3f ", c.depthXStep)
	s += fmt.Sprintf("depthystep %.3f ", c.depthYStep)
	s += fmt.Sprintf("lightamtxstep %.3f ", c.lightAmtXStep)
	s += fmt.Sprintf("lightamtystep %.3f", c.lightAmtYStep)
	return s
}

type IndexedModel struct {
	Positions []ga.Vec4d
	Texcoords []ga.Vec4d
	Normals   []ga.Vec4d
	Tangents  []ga.Vec4d
	Indices   []int
}

func NewIndexedModel(model *obj.Model) *IndexedModel {
	result := &IndexedModel{}
	normalModel := &IndexedModel{}
	resultIndexMap := make(map[[3]int]int)
	normalIndexMap := make(map[int]int)
	indexMap := make(map[int]int)

	for _, faces := range model.Faces {
		for _, face := range faces {
			var (
				currentPosition ga.Vec4d
				currentTexcoord ga.Vec4d
				currentNormal   ga.Vec4d
			)

			if 0 <= face[0] && face[0] < len(model.Verts) {
				currentPosition = model.Verts[face[0]]
			}
			if 0 <= face[1] && face[1] < len(model.Coords) {
				currentTexcoord = model.Coords[face[1]]
				currentTexcoord.Y = 1 - currentTexcoord.Y
			}
			if 0 <= face[2] && face[2] < len(model.Normals) {
				currentNormal = model.Normals[face[2]]
			}

			modelVertexIndex, found := resultIndexMap[face]
			if !found {
				modelVertexIndex = len(result.Positions)
				resultIndexMap[face] = modelVertexIndex

				result.Positions = append(result.Positions, currentPosition)
				result.Texcoords = append(result.Texcoords, currentTexcoord)
				result.Normals = append(result.Normals, currentNormal)
			}

			normalModelIndex, found := normalIndexMap[face[0]]
			if !found {
				normalModelIndex = len(normalModel.Positions)
				normalIndexMap[face[0]] = normalModelIndex

				normalModel.Positions = append(normalModel.Positions, currentPosition)
				normalModel.Texcoords = append(normalModel.Texcoords, currentTexcoord)
				normalModel.Normals = append(normalModel.Normals, currentNormal)
				normalModel.Tangents = append(normalModel.Tangents, ga.Vec4d{})
			}

			result.Indices = append(result.Indices, modelVertexIndex)
			normalModel.Indices = append(normalModel.Indices, normalModelIndex)
			indexMap[modelVertexIndex] = normalModelIndex
		}
	}

	normalModel.calcNormals()
	for i := range result.Positions {
		result.Normals = append(result.Normals, normalModel.Normals[indexMap[i]])
	}

	normalModel.calcTangents()
	for i := range result.Positions {
		result.Tangents = append(result.Tangents, normalModel.Tangents[indexMap[i]])
	}

	return result
}

func (c *IndexedModel) calcNormals() {
	// normal calculation happens on 2 edges of a triangle
	// the 2 edges of a triangle makes a 2d plane, and their
	// cross product creates a vector perpendicular to it,
	// use that vector as the normal
	// cross(triangle_edge_1, triangle_edge_2) = normal_vector
	for i := 2; i < len(c.Indices); i += 3 {
		i0 := c.Indices[i-2]
		i1 := c.Indices[i-1]
		i2 := c.Indices[i]

		if c.indexOutOfBounds(len(c.Positions), i0, i1, i2) {
			continue
		}
		if c.indexOutOfBounds(len(c.Normals), i0, i1, i2) {
			continue
		}

		v1 := vec4.Vec3(vec4.Sub(c.Positions[i1], c.Positions[i0]))
		v2 := vec4.Vec3(vec4.Sub(c.Positions[i2], c.Positions[i0]))
		v3 := vec3.Cross(vec3.Normalize(v1), vec3.Normalize(v2))
		normal := vec3.Vec4(v3)

		c.Normals[i0] = vec4.Add(c.Normals[i0], normal)
		c.Normals[i1] = vec4.Add(c.Normals[i1], normal)
		c.Normals[i2] = vec4.Add(c.Normals[i2], normal)
	}
	for i := range c.Normals {
		c.Normals[i] = vec4.Normalize(c.Normals[i])
	}
}

func (c *IndexedModel) calcTangents() {
	// tangent should lie on the plane of
	// the two triangle edges, standard construction for
	// it is write the triangle edges as a linear
	// combination of the tangent and bitangent
	// vectors, though we only calculate for tangent here

	// solve the 2 simultaneous equation for T and B
	// triangle_edge1 = texcoord_edge1.x*Tangent_Vector + texcoord_edge1.y*Bitangent_Vector
	// triangle_edge2 = texcoord_edge2.x*Tangent_Vector + texcoord_edge2.y*Bitangent_Vector
	for i := 2; i < len(c.Indices); i += 3 {
		i0 := c.Indices[i-2]
		i1 := c.Indices[i-1]
		i2 := c.Indices[i]

		if c.indexOutOfBounds(len(c.Positions), i0, i1, i2) {
			continue
		}
		if c.indexOutOfBounds(len(c.Texcoords), i0, i1, i2) {
			continue
		}
		if c.indexOutOfBounds(len(c.Tangents), i0, i1, i2) {
			continue
		}

		edge1 := vec4.Sub(c.Positions[i1], c.Positions[i0])
		edge2 := vec4.Sub(c.Positions[i2], c.Positions[i0])

		deltaU1 := c.Texcoords[i1].X - c.Texcoords[i0].X
		deltaV1 := c.Texcoords[i1].Y - c.Texcoords[i0].Y
		deltaU2 := c.Texcoords[i2].X - c.Texcoords[i0].X
		deltaV2 := c.Texcoords[i2].Y - c.Texcoords[i0].Y
		dividend := deltaU1*deltaV2 - deltaU2*deltaV1
		f := 0.0
		if dividend != 0 {
			f = 1 / dividend
		}

		tangent := ga.Vec4d{
			f * (deltaV2*edge1.X - deltaV1*edge2.X),
			f * (deltaV2*edge1.Y - deltaV1*edge2.Y),
			f * (deltaV2*edge1.Z - deltaV1*edge2.Z),
			0,
		}
		c.Tangents[i0] = vec4.Add(c.Tangents[i0], tangent)
		c.Tangents[i1] = vec4.Add(c.Tangents[i1], tangent)
		c.Tangents[i2] = vec4.Add(c.Tangents[i2], tangent)
	}
}

func (c *IndexedModel) indexOutOfBounds(length int, idx ...int) bool {
	for _, idx := range idx {
		if idx < 0 || idx >= length {
			return true
		}
	}
	return false
}

type Mesh struct {
	Vertices []Vertex
	Indices  []int
}

func LoadMesh(name string) (*Mesh, error) {
	model, err := obj.LoadFile(name)
	if err != nil {
		return nil, err
	}
	indexedModel := NewIndexedModel(model)

	var vertices []Vertex
	for i := range indexedModel.Positions {
		vertices = append(vertices, Vertex{
			Position: indexedModel.Positions[i],
			Texcoord: indexedModel.Texcoords[i],
			Normal:   indexedModel.Normals[i],
		})
	}
	indices := indexedModel.Indices

	return &Mesh{
		Vertices: vertices,
		Indices:  indices,
	}, nil
}

func (m *Mesh) Draw(context *RenderContext, viewProjection, transform *ga.Mat4d, texture *image.RGBA) {
	// 'transform' is the model matrix
	// combined with viewProjection gives us the standard modelViewProjection matrix
	// which transform from various spaces:
	// model -> world -> camera -> clip
	//       (model)  (view)    (projection)
	var mvp ga.Mat4d
	mat4.Mul(&mvp, viewProjection, transform)
	for i := 2; i < len(m.Indices); i += 3 {
		i0 := m.Indices[i-2]
		i1 := m.Indices[i-1]
		i2 := m.Indices[i]

		v0 := m.Vertices[i0].Transform(&mvp, transform)
		v1 := m.Vertices[i1].Transform(&mvp, transform)
		v2 := m.Vertices[i2].Transform(&mvp, transform)

		context.DrawTriangle(&v0, &v1, &v2, texture)
	}
}

func RotateVector(rotation ga.Quatd, vec ga.Vec4d) ga.Vec4d {
	qi := quat.Conj(rotation)
	qr := rotation
	qp := quat.Mul(quat.Mulv(qr, vec), qi)
	return ga.Vec4d{qp.X, qp.Y, qp.Z, 1}
}

func CardinalDirections(rotation ga.Quatd) (forward, backward, up, down, left, right ga.Vec4d) {
	forward = ga.Vec4d{0, 0, 1, 1}
	backward = ga.Vec4d{0, 0, -1, 1}
	up = ga.Vec4d{0, 1, 0, 1}
	down = ga.Vec4d{0, -1, 0, 1}
	left = ga.Vec4d{-1, 0, 0, 1}
	right = ga.Vec4d{1, 0, 0, 1}

	forward = RotateVector(rotation, forward)
	backward = RotateVector(rotation, backward)
	left = RotateVector(rotation, left)
	right = RotateVector(rotation, right)

	return
}

func ScreenSpaceTransform(halfWidth, halfHeight float64) ga.Mat4d {
	return ga.Mat4d{
		{halfWidth, 0, 0, halfWidth - 0.5},
		{0, -halfHeight, 0, halfHeight - 0.5},
		{0, 0, 1, 0},
		{0, 0, 0, 1},
	}
}

// left handed z=1, right handed z=-1
func Perspective(fovy, aspect, znear, zfar, handedness float64) ga.Mat4d {
	f := math.Tan(fovy / 2)
	zrange := znear - zfar
	return ga.Mat4d{
		{1 / (f * aspect), 0, 0, 0},
		{0, 1 / f, 0, 0},
		{0, 0, handedness * (-znear - zfar) / zrange, 2 * zfar * znear / zrange},
		{0, 0, handedness, 0},
	}
}
