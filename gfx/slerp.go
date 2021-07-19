// http://number-none.com/product/Understanding%20Slerp,%20Then%20Not%20Using%20It/
package main

import (
	"fmt"
	"image"
	"image/color"
	"image/draw"
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
	vect          [2]f64.Vec3
	mode          int
	interpolating bool
)

func main() {
	runtime.LockOSThread()
	initSDL()
	reset()
	for {
		event()
		update()
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

	window.SetTitle("Slerp")

	resizeWindow(w, h)
}

func reset() {
	camera.position = f64.Vec3{0, 0, -1}
	camera.orientation = f64.Quat{}.FromAxisAngle(f64.Vec3{0, 1, 0}, math.Pi/4)

	setupProjection()
}

func resizeWindow(w, h int) {
	var err error
	texture, err = renderer.CreateTexture(sdl.PIXELFORMAT_ABGR8888, sdl.TEXTUREACCESS_STREAMING, w, h)
	ck(err)

	canvas = image.NewRGBA(image.Rect(0, 0, w, h))
	dcg = drc.New(canvas)

	setupProjection()
}

func setupProjection() {
	w, h, _ := renderer.OutputSize()
	hw, hh := float64(w)/2, float64(h)/2

	arcball.Window = f64.Rect(0, 0, float64(w), float64(h))

	var translate f64.Mat4
	orientation := camera.orientation.Mat4()
	translate.Translate3(camera.position)
	modelview.Identity()
	modelview.Mul(&translate, &orientation)

	projection.Perspective(f64.Deg2Rad(90), float64(w)/float64(h), 0.2, 1000)
	viewport = f64.Mat4{
		{hw, 0, 0, hw},
		{0, hh, 0, hh},
		{0, 0, 1, 0},
		{0, 0, 0, 1},
	}
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
				vect[1] = f64.Vec3{1, 0, 0}
				interpolating = true
			case sdl.K_2:
				vect[1] = f64.Vec3{0, 1, 0}
				interpolating = true
			case sdl.K_3:
				vect[1] = f64.Vec3{0, 0, 1}
				interpolating = true
			case sdl.K_SPACE:
				mode = 1 - mode
				fmt.Println("mode:", mode)
			}
		case sdl.MouseButtonDownEvent:
			switch ev.Button {
			case sdl.BUTTON_LEFT:
				arcball.From = arcball.Vector(int(ev.X), int(ev.Y))
			case sdl.BUTTON_RIGHT:
				M := dcg.Matrix()
				M.Inverse()
				p := f64.Vec3{float64(ev.X), float64(ev.Y), 0}
				vect[0] = M.Transform3(p).Normalize()
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

func update() {
	if interpolating {
		q1 := f64.Quat{vect[0].X, vect[0].Y, vect[0].Z, 0}.Normalize()
		q2 := f64.Quat{vect[1].X, vect[1].Y, vect[1].Z, 0}.Normalize()
		if mode == 0 {
			q1 = q1.Slerp(0.1, q2)
		} else {
			q1 = q1.Lerp(0.1, q2)
		}
		fmt.Println("interpolating", q1, q2)
		vect[0] = f64.Vec3{q1.X, q1.Y, q1.Z}
		if vect[0].Equals(vect[1], 1e-2) {
			interpolating = false
		}
	}
}

func blit() {
	M := f64.Mat4{}
	M.Identity()
	M.Mul(&projection, &modelview)
	M.Mul(&viewport, &M)
	dcg.SetMatrix(M)

	renderer.Clear()
	draw.Draw(canvas, canvas.Bounds(), image.NewUniform(color.RGBA{30, 30, 80, 255}), image.ZP, draw.Over)
	blitLines()
	texture.Update(nil, canvas.Pix, canvas.Stride)
	renderer.Copy(texture, nil, nil)
	renderer.Present()
}

func blitLines() {
	o := f64.Vec3{0, 0, 0}
	x := f64.Vec3{1, 0, 0}
	y := f64.Vec3{0, 1, 0}
	z := f64.Vec3{0, 0, 1}

	dcg.Save()
	defer dcg.Restore()

	M := f64.Mat4{}
	M.Scale(0.25, 0.25, 0.25)
	dcg.TransformMatrix(&M)

	dcg.SetLineWidth(5)
	dcg.SetStroke(sdlcolor.Red)
	dcg.Line3p(o, x)
	dcg.SetStroke(sdlcolor.Green)
	dcg.Line3p(o, y)
	dcg.SetStroke(sdlcolor.Blue)
	dcg.Line3p(o, z)

	dcg.SetStroke(color.RGBA{255, 255, 255, 255})
	dcg.Line3p(o, vect[0])
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
