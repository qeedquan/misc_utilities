// http://www.lysator.liu.se/~mikaelk/doc/perspectivetexture/
package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	"image/draw"
	"log"
	"math"
	"math/rand"
	"os"
	"path/filepath"
	"runtime"
	"time"

	"github.com/qeedquan/go-media/image/chroma"
	"github.com/qeedquan/go-media/image/imageutil"
	"github.com/qeedquan/go-media/math/ga"
	"github.com/qeedquan/go-media/math/ga/mat4"
	"github.com/qeedquan/go-media/math/ga/quat"
	"github.com/qeedquan/go-media/math/ga/vec3"
	"github.com/qeedquan/go-media/sdl"
)

var (
	window   *sdl.Window
	renderer *sdl.Renderer
	texture  *sdl.Texture
	canvas   *image.RGBA
	images   []*image.RGBA

	cameraPos   ga.Vec3d
	cameraFront ga.Vec3d
	cameraUp    ga.Vec3d
	modelview   ga.Mat4d
	projection  ga.Mat4d
	zbuffer     []float64
	rot         ga.Vec3d
	rotating    bool

	imageDir = flag.String("t", "textures", "image directory")
)

var cube = []Triangle{
	// front
	{
		Position: [3]ga.Vec3d{
			{-1, -1, +1},
			{+1, -1, +1},
			{-1, +1, +1},
		},
		Texcoord: [3]ga.Vec2d{
			{0, 0},
			{1, 0},
			{0, 1},
		},
	},
	{
		Position: [3]ga.Vec3d{
			{+1, -1, +1},
			{-1, +1, +1},
			{+1, +1, +1},
		},
		Texcoord: [3]ga.Vec2d{
			{1, 0},
			{0, 1},
			{1, 1},
		},
	},

	// back
	{
		Position: [3]ga.Vec3d{
			{-1, -1, -1},
			{+1, -1, -1},
			{-1, +1, -1},
		},
		Texcoord: [3]ga.Vec2d{
			{0, 0},
			{1, 0},
			{0, 1},
		},
	},
	{
		Position: [3]ga.Vec3d{
			{+1, -1, -1},
			{-1, +1, -1},
			{+1, +1, -1},
		},
		Texcoord: [3]ga.Vec2d{
			{1, 0},
			{0, 1},
			{1, 1},
		},
	},

	// top
	{
		Position: [3]ga.Vec3d{
			{-1, +1, -1},
			{+1, +1, -1},
			{-1, +1, +1},
		},
		Texcoord: [3]ga.Vec2d{
			{0, 0},
			{1, 0},
			{0, 1},
		},
	},
	{
		Position: [3]ga.Vec3d{
			{+1, +1, -1},
			{-1, +1, +1},
			{+1, +1, +1},
		},
		Texcoord: [3]ga.Vec2d{
			{1, 0},
			{0, 1},
			{1, 1},
		},
	},

	// bottom
	{
		Position: [3]ga.Vec3d{
			{-1, -1, -1},
			{+1, -1, -1},
			{-1, -1, +1},
		},
		Texcoord: [3]ga.Vec2d{
			{0, 0},
			{1, 0},
			{0, 1},
		},
	},
	{
		Position: [3]ga.Vec3d{
			{+1, -1, -1},
			{-1, -1, +1},
			{+1, -1, +1},
		},
		Texcoord: [3]ga.Vec2d{
			{1, 0},
			{0, 1},
			{1, 1},
		},
	},
	// left
	{
		Position: [3]ga.Vec3d{
			{-1, -1, -1},
			{-1, -1, +1},
			{-1, +1, -1},
		},
		Texcoord: [3]ga.Vec2d{
			{0, 0},
			{1, 0},
			{0, 1},
		},
	},
	{
		Position: [3]ga.Vec3d{
			{-1, -1, +1},
			{-1, +1, -1},
			{-1, +1, +1},
		},
		Texcoord: [3]ga.Vec2d{
			{1, 0},
			{0, 1},
			{1, 1},
		},
	},
	// right
	{
		Position: [3]ga.Vec3d{
			{+1, -1, -1},
			{+1, -1, +1},
			{+1, +1, -1},
		},
		Texcoord: [3]ga.Vec2d{
			{0, 0},
			{1, 0},
			{0, 1},
		},
	},
	{
		Position: [3]ga.Vec3d{
			{+1, -1, +1},
			{+1, +1, -1},
			{+1, +1, +1},
		},
		Texcoord: [3]ga.Vec2d{
			{1, 0},
			{0, 1},
			{1, 1},
		},
	},
}

func main() {
	runtime.LockOSThread()
	rand.Seed(time.Now().UnixNano())
	parseFlags()
	initSDL()
	reset()
	for {
		event()
		update()
		blit()
	}
}

func parseFlags() {
	flag.Usage = usage
	flag.Parse()
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options]")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func reset() {
	cameraPos = ga.Vec3d{0, 0, -3}
	cameraFront = ga.Vec3d{0, 0, -1}
	cameraUp = ga.Vec3d{0, 1, 0}
	modelview = mat4.LookAt(cameraPos, vec3.Add(cameraPos, cameraFront), cameraUp)
	projection = mat4.Perspective(ga.Rad2Deg(40.0), aspectRatio(), 0.2, 1000)
}

func aspectRatio() float64 {
	w, h, _ := renderer.OutputSize()
	return float64(w) / float64(h)
}

func loadImages(dir string) ([]*image.RGBA, error) {
	fis, err := os.ReadDir(dir)
	if err != nil {
		return nil, err
	}

	var imgs []*image.RGBA
	for _, fi := range fis {
		name := filepath.Join(dir, fi.Name())
		img, err := imageutil.LoadRGBAFile(name)
		if err == nil {
			imgs = append(imgs, img)
			fmt.Println("loaded", name)
		}
	}
	return imgs, nil
}

func initSDL() {
	err := sdl.Init(sdl.INIT_VIDEO | sdl.INIT_TIMER)
	ck(err)

	sdl.SetHint(sdl.HINT_RENDER_SCALE_QUALITY, "best")
	w, h := 1280, 800
	wflag := sdl.WINDOW_RESIZABLE
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, wflag)
	ck(err)

	window.SetTitle("Perspective Texture Mapping")

	texture, err = renderer.CreateTexture(sdl.PIXELFORMAT_ABGR8888, sdl.TEXTUREACCESS_STREAMING, w, h)
	ck(err)
	canvas = image.NewRGBA(image.Rect(0, 0, w, h))

	images, err = loadImages(*imageDir)
	fmt.Printf("loaded %d images\n", len(images))

	zbuffer = make([]float64, w*h)
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
				rotating = !rotating
			}
		case sdl.WindowEvent:
			switch ev.Event {
			case sdl.WINDOWEVENT_RESIZED:
				w, h := int(ev.Data[0]), int(ev.Data[1])
				zbuffer = make([]float64, w*h)
				canvas = image.NewRGBA(image.Rect(0, 0, w, h))
			}
		case sdl.MouseWheelEvent:
			cameraPos.Z += ga.Clamp(float64(ev.Y), -0.1, 0.1)
		}
	}
}

func update() {
	if rotating {
		rot.X += 0.05
		rot.Y += 0.05
	}
}

func blit() {
	for i := range zbuffer {
		zbuffer[i] = -math.MaxFloat32
	}
	renderer.Clear()
	draw.Draw(canvas, canvas.Bounds(), image.NewUniform(color.RGBA{100, 100, 120, 255}), image.ZP, draw.Over)
	blitCubes()
	texture.Update(nil, canvas.Pix, canvas.Stride)
	renderer.Copy(texture, nil, nil)
	renderer.Present()
}

func screenSpace() ga.Mat4d {
	w, h, _ := renderer.OutputSize()
	hw := float64(w) / 2
	hh := float64(h) / 2
	return ga.Mat4d{
		{hw, 0, 0, hw},
		{0, hh, 0, hh},
		{0, 0, 1, 0},
		{0, 0, 0, 1},
	}
}

func blitCubes() {
	Q := quat.AxisAngle(ga.Vec3d{0, 1, 0}, rot.Y)
	Ry := quat.Matrix(Q)

	Q = quat.AxisAngle(ga.Vec3d{1, 0, 0}, rot.X)
	Rx := quat.Matrix(Q)

	R := matMul(&Rx, &Ry)

	modelview = mat4.LookAt(cameraPos, vec3.Add(cameraPos, cameraFront), cameraUp)
	S := mat4.Scale(0.5, 0.5, 0.5)
	for n := 0; n < 10; n++ {
		tx := float64(n) * 1.05
		ty := float64(n) * 2
		tz := 0.0
		T := ga.Mat4d{
			{1, 0, 0, tx},
			{0, 1, 0, ty},
			{0, 0, 1, tz},
			{0, 0, 0, 1},
		}
		for _, c := range cube {
			if len(images) > 0 {
				c.Texture = images[n%len(images)]
			}
			N := screenSpace()
			M := matMul(&N, &projection, &modelview, &T, &S, &R)

			for i := range c.Position {
				c.Position[i] = transform(&M, c.Position[i])
			}

			c.Draw()
		}
	}
}

func transform(m *ga.Mat4d, v ga.Vec3d) ga.Vec3d {
	s := m[3][0]*v.X + m[3][1]*v.Y + m[3][2]*v.Z + m[3][3]
	switch s {
	case 0:
		return ga.Vec3d{}
	}

	invs := 1 / s
	p := mat4.Apply(m, vec3.Vec4(v))
	return ga.Vec3d{p.X * invs, p.Y * invs, p.Z * invs}
}

func randImage() *image.RGBA {
	if len(images) == 0 {
		return nil
	}
	return images[rand.Intn(len(images))]
}

func matMul(m ...*ga.Mat4d) ga.Mat4d {
	p := mat4.Diagonal(1.0)
	for i := range m {
		mat4.Mul(&p, &p, m[i])
	}
	return p
}

type Triangle struct {
	Position [3]ga.Vec3d
	Texcoord [3]ga.Vec2d
	Texture  *image.RGBA
}

// draw a perspectively correct mapped texture on a triangle in screen space
func (c *Triangle) Draw() {
	// shift xy coordinate system +0.5 to make it be center for subpixeling
	x1 := math.Floor(c.Position[0].X) + 0.5
	y1 := math.Floor(c.Position[0].Y) + 0.5
	x2 := math.Floor(c.Position[1].X) + 0.5
	y2 := math.Floor(c.Position[1].Y) + 0.5
	x3 := math.Floor(c.Position[2].X) + 0.5
	y3 := math.Floor(c.Position[2].Y) + 0.5

	// calculate alternative 1/Z, U/Z and V/Z values which will be interpolated
	// z = 1 / (1/z_screen)
	// u = (u/z) * z_screen
	// v = (v/z) * z_screen

	iz1 := 1 / c.Position[0].Z
	iz2 := 1 / c.Position[1].Z
	iz3 := 1 / c.Position[2].Z
	uiz1 := c.Texcoord[0].X * iz1
	viz1 := c.Texcoord[0].Y * iz1
	uiz2 := c.Texcoord[1].X * iz2
	viz2 := c.Texcoord[1].Y * iz2
	uiz3 := c.Texcoord[2].X * iz3
	viz3 := c.Texcoord[2].Y * iz3

	// sort the vertices in ascending Y order
	if y1 > y2 {
		x1, x2 = x2, x1
		y1, y2 = y2, y1
		iz1, iz2 = iz2, iz1
		uiz1, uiz2 = uiz2, uiz1
		viz1, viz2 = viz2, viz1
	}
	if y1 > y3 {
		x1, x3 = x3, x1
		y1, y3 = y3, y1
		iz1, iz3 = iz3, iz1
		uiz1, uiz3 = uiz3, uiz1
		viz1, viz3 = viz3, viz1
	}
	if y2 > y3 {
		x2, x3 = x3, x2
		y2, y3 = y3, y2
		iz2, iz3 = iz3, iz2
		uiz2, uiz3 = uiz3, uiz2
		viz2, viz3 = viz3, viz2
	}

	y1i := int(y1)
	y2i := int(y2)
	y3i := int(y3)

	// skip poly if it's too thin to cover any pixels at all
	if (y1i == y2i && y1i == y3i) || (int(x1) == int(x2) && int(x1) == int(x3)) {
		return
	}

	// calculate horizontal and vertical increments for UV axes (these
	// calcs are certainly not optimal, although they're stable
	// (handles any dy being 0)
	denom := ((x3-x1)*(y2-y1) - (x2-x1)*(y3-y1))

	// skip if it's an infinitely thin line
	if denom == 0 {
		return
	}

	// Reciprocal for speeding up
	denom = 1 / denom

	dizdx := ((iz3-iz1)*(y2-y1) - (iz2-iz1)*(y3-y1)) * denom
	duizdx := ((uiz3-uiz1)*(y2-y1) - (uiz2-uiz1)*(y3-y1)) * denom
	dvizdx := ((viz3-viz1)*(y2-y1) - (viz2-viz1)*(y3-y1)) * denom
	dizdy := ((iz2-iz1)*(x3-x1) - (iz3-iz1)*(x2-x1)) * denom
	duizdy := ((uiz2-uiz1)*(x3-x1) - (uiz3-uiz1)*(x2-x1)) * denom
	dvizdy := ((viz2-viz1)*(x3-x1) - (viz3-viz1)*(x2-x1)) * denom

	// calculate X-slopes along the edges
	var dxdy1, dxdy2, dxdy3 float64
	if y2 > y1 {
		dxdy1 = (x2 - x1) / (y2 - y1)
	}
	if y3 > y1 {
		dxdy2 = (x3 - x1) / (y3 - y1)
	}
	if y3 > y2 {
		dxdy3 = (x3 - x2) / (y3 - y2)
	}

	// determine which side of the poly the longer edge is on
	side := dxdy2 > dxdy1
	if y1 == y2 {
		side = x1 > x2
	}
	if y2 == y3 {
		side = x3 > x2
	}

	var (
		dy, dxdya, dizdya, duizdya, dvizdya float64
		xa, xb, dxdyb                       float64
		iza, uiza, viza                     float64
	)

	if !side {
		// Longer edge is on the left side
		// Calculate slopes along left edge

		dxdya = dxdy2
		dizdya = dxdy2*dizdx + dizdy
		duizdya = dxdy2*duizdx + duizdy
		dvizdya = dxdy2*dvizdx + dvizdy

		// Perform subpixel pre-stepping along left edge

		dy = 1 - (y1 - float64(y1i))
		xa = x1 + dy*dxdya
		iza = iz1 + dy*dizdya
		uiza = uiz1 + dy*duizdya
		viza = viz1 + dy*dvizdya

		if y1i < y2i {
			// Draw upper segment if possibly visible
			// Set right edge X-slope and perform subpixel pre-stepping
			xb = x1 + dy*dxdy1
			dxdyb = dxdy1
			c.drawScanline(&xa, &xb, &iza, &uiza, &viza, dizdx, duizdx, dvizdx, dxdya, dxdyb, dizdya, duizdya, dvizdya, y1i, y2i)
		}
		if y2i < y3i {
			// Draw lower segment if possibly visible
			// Set right edge X-slope and perform subpixel pre-stepping
			xb = x2 + (1-(y2-float64(y2i)))*dxdy3
			dxdyb = dxdy3
			c.drawScanline(&xa, &xb, &iza, &uiza, &viza, dizdx, duizdx, dvizdx, dxdya, dxdyb, dizdya, duizdya, dvizdya, y2i, y3i)
		}
	} else {
		// Longer edge is on the right side
		// Set right edge X-slope and perform subpixel pre-stepping

		dxdyb = dxdy2
		dy = 1 - (y1 - float64(y1i))
		xb = x1 + dy*dxdyb

		if y1i < y2i {
			// Draw upper segment if possibly visible
			// Set slopes along left edge and perform subpixel pre-stepping
			dxdya = dxdy1
			dizdya = dxdy1*dizdx + dizdy
			duizdya = dxdy1*duizdx + duizdy
			dvizdya = dxdy1*dvizdx + dvizdy
			xa = x1 + dy*dxdya
			iza = iz1 + dy*dizdya
			uiza = uiz1 + dy*duizdya
			viza = viz1 + dy*dvizdya
			c.drawScanline(&xa, &xb, &iza, &uiza, &viza, dizdx, duizdx, dvizdx, dxdya, dxdyb, dizdya, duizdya, dvizdya, y1i, y2i)
		}
		if y2i < y3i {
			// Draw lower segment if possibly visible
			// Set slopes along left edge and perform subpixel pre-stepping
			dxdya = dxdy3
			dizdya = dxdy3*dizdx + dizdy
			duizdya = dxdy3*duizdx + duizdy
			dvizdya = dxdy3*dvizdx + dvizdy
			dy = 1 - (y2 - float64(y2i))
			xa = x2 + dy*dxdya
			iza = iz2 + dy*dizdya
			uiza = uiz2 + dy*duizdya
			viza = viz2 + dy*dvizdya
			c.drawScanline(&xa, &xb, &iza, &uiza, &viza, dizdx, duizdx, dvizdx, dxdya, dxdyb, dizdya, duizdya, dvizdya, y2i, y3i)
		}
	}
}

func (c *Triangle) drawScanline(xa, xb, iza, uiza, viza *float64, dizdx, duizdx, dvizdx, dxdya, dxdyb, dizdya, duizdya, dvizdya float64, y1, y2 int) {
	for ; y1 < y2; y1++ {
		x1, x2 := int(*xa), int(*xb)

		// perform subtexel pre-stepping on 1/Z, U/Z and V/Z
		dx := 1 - (*xa - float64(x1))
		iz := *iza + dx*dizdx
		uiz := *uiza + dx*duizdx
		viz := *viza + dx*dvizdx
		for x1 < x2 {
			x1++

			// calculate U and V from 1/Z, U/Z and V/Z
			z := 1 / iz
			u := uiz * z
			v := viz * z

			// copy pixel from texture to screen
			idx := y1*canvas.Bounds().Dx() + x1
			if 0 <= idx && idx < len(zbuffer) && z >= zbuffer[idx] {
				zbuffer[idx] = z
				if c.Texture != nil {
					r := c.Texture.Bounds()
					tx := int(u*float64(r.Dx()-1) + 0.5)
					ty := int(v*float64(r.Dy()-1) + 0.5)
					col := c.Texture.At(tx, ty)
					canvas.Set(x1, y1, col)
				} else {
					col := chroma.Vec4ToRGBA(ga.Vec4d{u, v, 1 - u - v, 1})
					canvas.Set(x1, y1, col)
				}
			}

			// step 1/Z, U/Z and V/Z horizontally
			iz += dizdx
			uiz += duizdx
			viz += dvizdx
		}

		// step along both edges
		*xa += dxdya
		*xb += dxdyb
		*iza += dizdya
		*uiza += duizdya
		*viza += dvizdya
	}
}
