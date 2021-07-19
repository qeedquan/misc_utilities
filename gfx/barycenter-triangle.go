// https://github.com/ssloy/tinyrenderer/wiki/Lesson-2:-Triangle-rasterization-and-back-face-culling
// barycentric coordinates are used for interpolation between triangle coordinates
package main

import (
	"flag"
	"log"
	"math"
	"os"
	"runtime"

	"github.com/qeedquan/go-media/math/f64"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
	"github.com/qeedquan/go-media/sdl/sdlimage/sdlcolor"
)

type Triangle [3]f64.Vec2

var (
	window   *sdl.Window
	renderer *sdl.Renderer
	triangle Triangle
	grab     int
)

var conf struct {
	width  int
	height int
	radius int
	aspect float64
}

func main() {
	runtime.LockOSThread()
	log.SetFlags(0)
	parseFlags()
	initSDL()
	loop()
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func parseFlags() {
	flag.IntVar(&conf.width, "width", 1024, "window width")
	flag.IntVar(&conf.height, "height", 768, "window height")
	flag.Parse()
	conf.radius = 10
	conf.aspect = float64(conf.width) / float64(conf.height)
}

func initSDL() {
	err := sdl.Init(sdl.INIT_VIDEO)
	ck(err)

	window, renderer, err = sdl.CreateWindowAndRenderer(conf.width, conf.height, sdl.WINDOW_RESIZABLE)
	ck(err)

	window.SetTitle("Barycentric Triangle")
	renderer.SetLogicalSize(conf.width, conf.height)
}

func loop() {
	reset()
	for {
		event()
		blit()
	}
}

func reset() {
	grab = -1
	triangle = Triangle{
		{0.7, 0.5},
		{0.7 - 0.3, 0.5 + 0.3},
		{0.7 + 0.3, 0.5 + 0.3},
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
			case sdl.K_r:
				reset()
			}
		case sdl.MouseMotionEvent:
			if grab >= 0 {
				triangle[grab] = f64.Vec2{
					float64(ev.X) / float64(conf.width) * conf.aspect,
					float64(ev.Y) / float64(conf.height),
				}
			}
		case sdl.MouseButtonDownEvent:
			if ev.Button == 1 {
				grip(&triangle, int(ev.X), int(ev.Y))
			}
		case sdl.MouseButtonUpEvent:
			if ev.Button == 1 {
				grab = -1
			}
		}
	}
}

func grip(t *Triangle, mx, my int) {
	gx := float64(mx) / float64(conf.width)
	gy := float64(my) / float64(conf.height)
	rd := float64(conf.radius) / (float64(conf.width) / conf.aspect)
	for i := range t {
		rx := t[i].X / conf.aspect
		ry := t[i].Y
		if (gx-rx)*(gx-rx)+(gy-ry)*(gy-ry) < rd*rd {
			grab = i
			return
		}
	}
}

func blit() {
	renderer.SetDrawColor(sdl.Color{88, 88, 88, 255})
	renderer.Clear()
	triangle.Draw()
	renderer.Present()
}

func (t *Triangle) Draw() {
	x1 := math.MaxFloat64
	y1 := math.MaxFloat64
	x2 := -math.MaxFloat64
	y2 := -math.MaxFloat64
	for i := range t {
		if x1 > t[i].X {
			x1 = t[i].X
		}
		if y1 > t[i].Y {
			y1 = t[i].Y
		}
		if x2 < t[i].X {
			x2 = t[i].X
		}
		if y2 < t[i].Y {
			y2 = t[i].Y
		}
	}
	x1 *= float64(conf.width) / conf.aspect
	y1 *= float64(conf.height)
	x2 *= float64(conf.width) / conf.aspect
	y2 *= float64(conf.height)

	for y := y1; y <= y2; y++ {
		for x := x1; x <= x2; x++ {
			px := x / (float64(conf.width) / conf.aspect)
			py := y / float64(conf.height)
			b := barycenter(t[0], t[1], t[2], f64.Vec2{px, py})
			if b.X >= 0 && b.Y >= 0 && b.Z >= 0 {
				renderer.SetDrawColor(sdl.Color{
					uint8(255 * b.X),
					uint8(255 * b.Y),
					uint8(255 * b.Z),
					255,
				})
				renderer.DrawPoint(int(x), int(y))
			}
		}
	}

	c := []sdl.Color{
		sdlcolor.Red,
		sdlcolor.Blue,
		sdlcolor.Green,
	}
	for i := 0; i < len(t); i++ {
		x := t[i].X * float64(conf.width) / conf.aspect
		y := t[i].Y * float64(conf.height)
		sdlgfx.FilledCircle(renderer, int(x), int(y), conf.radius, c[i])
	}
}

// barycentric coordinate expresses point P as
// P = A + uAB + vAC
// we solve for the vector Z that is the barycentric values [u v 1-u-v] which adds to 1
// do this by doing some algebra
// P = A + uAB + vAC subtract A from both side get
// 0 = PA + uAB + vAC
// split into x and y coordinate get
// uABx + vACx + PAx = 0
// uABy + vACy + PAy = 0
// if we write it in vector form, we want a vector orthogonal to those
// two vector, so do cross product to get that
func barycenter(TA, TB, TC f64.Vec2, P f64.Vec2) f64.Vec3 {
	X := f64.Vec3{
		TA.X - P.X,
		TB.X - TA.X,
		TC.X - TA.X,
	}
	Y := f64.Vec3{
		TA.Y - P.Y,
		TB.Y - TA.Y,
		TC.Y - TA.Y,
	}
	Z := X.Cross(Y)

	Z.Y /= Z.X
	Z.Z /= Z.X
	Z.X /= Z.X

	Z.X = 1 - Z.Y - Z.Z
	return Z
}
