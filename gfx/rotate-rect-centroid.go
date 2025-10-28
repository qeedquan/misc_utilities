// the rectangle has to be 4 independent points
// for rotation to work as expected
// if we defined it to be drawn using 2 points
// ie, (x,y) (x+w, y+h) the two point will constrain
// the other two points and give wrong rotation for
// the dependent points

// observe the centroid is different for different formula
// one is just a center of a rect, which is wrong for non-rect
// one takes an average of the points
// one is a convex polygon
package main

import (
	"flag"
	"fmt"
	"log"
	"math"
	"os"
	"runtime"

	"github.com/qeedquan/go-media/math/ga"
	"github.com/qeedquan/go-media/math/ga/vec2"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlimage/sdlcolor"
)

type Rect [4]ga.Vec2d

var (
	window   *sdl.Window
	renderer *sdl.Renderer

	spin   Rect
	theta  = -0.2
	around int
)

var flags struct {
	width  int
	height int
}

func main() {
	runtime.LockOSThread()

	log.SetPrefix("")
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

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options]")
	flag.PrintDefaults()
	os.Exit(2)
}

func parseFlags() {
	flag.IntVar(&flags.width, "width", 1024, "window width")
	flag.IntVar(&flags.height, "height", 768, "window height")
	flag.Usage = usage
	flag.Parse()
}

func initSDL() {
	err := sdl.Init(sdl.INIT_VIDEO)
	ck(err)

	window, renderer, err = sdl.CreateWindowAndRenderer(flags.width, flags.height, sdl.WINDOW_RESIZABLE)
	ck(err)

	renderer.SetLogicalSize(flags.width, flags.height)
	window.SetTitle("Rotate")

	w := float64(flags.width)
	h := float64(flags.height)
	s := 500.0
	spin = Rect{
		ga.Vec2d{(w - s) / 2, (h - s) / 2},
		ga.Vec2d{(w + s) / 2, (h - s) / 2},
		ga.Vec2d{(w + s) / 2, (h + s) / 2},
		ga.Vec2d{(w - s) / 2, (h + s) / 2},
	}
}

func event() {
	for {
		ev := sdl.PollEvent()
		if ev == nil {
			break
		}
		m := ga.Vec2d{5, 5}
		switch ev := ev.(type) {
		case sdl.QuitEvent:
			os.Exit(0)
		case sdl.KeyDownEvent:
			switch ev.Sym {
			case sdl.K_LEFT:
				spin = spin.Rot1(theta)
			case sdl.K_RIGHT:
				spin = spin.Rot2(theta)
			case sdl.K_1:
				spin[0] = vec2.Sub(spin[0], m)
			case sdl.K_2:
				spin[0] = vec2.Add(spin[0], m)
			case sdl.K_q:
				spin[1] = vec2.Sub(spin[1], m)
			case sdl.K_w:
				spin[1] = vec2.Add(spin[1], m)
			case sdl.K_a:
				spin[2] = vec2.Sub(spin[2], m)
			case sdl.K_s:
				spin[2] = vec2.Add(spin[2], m)
			case sdl.K_z:
				spin[3] = vec2.Sub(spin[3], m)
			case sdl.K_x:
				spin[3] = vec2.Add(spin[3], m)
			case sdl.K_p:
				around = (around + 1) % 3
				fmt.Println(around)
			case sdl.K_o:
				around = (around - 1) % 3
				if around < 0 {
					around += 3
				}
				fmt.Println(around)
			}
		}
	}
}

func draw() {
	renderer.SetDrawColor(sdlcolor.Black)
	renderer.Clear()
	spin.Draw()
	drawpt(spin.Center(), sdlcolor.Red)
	drawpt(centroidPoints(spin[:]), sdlcolor.Blue)
	drawpt(centroidConvexPoly(spin[:]), sdlcolor.Green)
	renderer.Present()
}

func drawpt(p ga.Vec2d, c sdl.Color) {
	const S = 10
	renderer.SetDrawColor(c)
	renderer.FillRect(&sdl.Rect{
		int32(p.X - S/2),
		int32(p.Y - S/2),
		S,
		S,
	})
}

func (r Rect) Draw() {
	renderer.SetDrawColor(sdlcolor.White)
	for i := 0; i < 4; i++ {
		a := r[i]
		b := r[(i+1)%4]
		renderer.DrawLine(int(a.X), int(a.Y), int(b.X), int(b.Y))
	}
}

func (r Rect) Rot1(t float64) Rect {
	s, c := math.Sincos(t)
	p := centroid(r)
	for i := range r {
		x := r[i].X - p.X
		y := r[i].Y - p.Y
		r[i] = ga.Vec2d{
			x*c - y*s,
			x*s + y*c,
		}
		r[i] = vec2.Add(r[i], p)
	}
	return r
}

func (r Rect) Rot2(t float64) Rect {
	s, c := math.Sincos(t)
	p := centroid(r)
	for i := range r {
		x := r[i].X - p.X
		y := r[i].Y - p.Y
		r[i] = ga.Vec2d{
			x*c + y*s,
			-x*s + y*c,
		}
		r[i] = vec2.Add(r[i], p)
	}
	return r
}

func (r Rect) Center() ga.Vec2d {
	a := ga.Vec2d{math.MaxFloat64, math.MaxFloat64}
	b := ga.Vec2d{-math.MaxFloat64, -math.MaxFloat64}
	for i := 0; i < 4; i++ {
		if a.X > r[i].X {
			a.X = r[i].X
		}
		if a.Y > r[i].Y {
			a.Y = r[i].Y
		}
		if b.X < r[i].X {
			b.X = r[i].X
		}
		if b.Y < r[i].Y {
			b.Y = r[i].Y
		}
	}
	return ga.Vec2d{
		(a.X + b.X) / 2,
		(a.Y + b.Y) / 2,
	}
}

func centroidPoints(p []ga.Vec2d) ga.Vec2d {
	var c ga.Vec2d
	for i := range p {
		c = vec2.Add(c, p[i])
	}
	return vec2.Scale(c, 1/float64(len(p)))
}

func centroidConvexPoly(p []ga.Vec2d) ga.Vec2d {
	var A float64
	for i := range p {
		a := p[i]
		b := p[(i+1)%len(p)]
		A += (a.X*b.Y - b.X*a.Y)
	}
	A *= 0.5

	var c ga.Vec2d
	for i := range p {
		a := p[i]
		b := p[(i+1)%len(p)]
		c.X += (a.X + b.X) * (a.X*b.Y - b.X*a.Y)
		c.Y += (a.Y + b.Y) * (a.X*b.Y - b.X*a.Y)
	}
	return vec2.Scale(c, 1/(6*A))
}

func centroid(r Rect) ga.Vec2d {
	var p ga.Vec2d
	switch around {
	case 0:
		p = r.Center()
	case 1:
		p = centroidPoints(r[:])
	case 2:
		p = centroidConvexPoly(r[:])
	default:
		panic("unreachable")
	}
	return p
}

func loop() {
	for {
		event()
		draw()
	}
}
