// draw some snowflakes turtle graphics style
package main

import (
	"image/color"
	"log"
	"math"
	"os"
	"runtime"

	"github.com/qeedquan/go-media/math/f64"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
)

var (
	window   *sdl.Window
	renderer *sdl.Renderer
	pen      *Pen
	mode     int
)

func main() {
	runtime.LockOSThread()
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

	window.SetTitle("Snowflake")
}

func reset() {
	w, h, _ := renderer.OutputSize()
	pen = newPen()
	pen.Place(w/2, h/2)
	mode = 0
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
				mode = 0
			case sdl.K_2:
				mode = 1
			}
		}
	}
}

func blit() {
	renderer.SetDrawColor(color.RGBA{32, 93, 112, 255})
	renderer.Clear()
	pen.Reset()
	switch mode {
	case 0:
		blitSnowflake1()
	case 1:
		blitSnowflake2()
	}
	renderer.Present()
}

func blitSnowflake1() {
	// draw a bunch of rotated parallelograms
	for i := 0; i < 10; i++ {
		// draw a parallelogram
		for j := 0; j < 2; j++ {
			pen.Forward(100)
			pen.Right(60)
			pen.Forward(100)
			pen.Right(120)
		}
		pen.Right(36)
	}
}

func blitSnowflake2() {
	pen.Release()
	pen.Forward(90)
	pen.Left(45)
	pen.Hold()
	for i := 0; i < 8; i++ {
		blitBranch()
		pen.Left(45)
	}
}

func blitBranch() {
	for i := 0; i < 3; i++ {
		// draw a bunch of rotated lines
		for j := 0; j < 3; j++ {
			pen.Forward(30)
			pen.Backward(30)
			pen.Right(45)
		}
		pen.Left(90)
		pen.Backward(30)
		pen.Left(45)
	}
	pen.Right(90)
	pen.Forward(90)
}

type Pen struct {
	lw     int
	px, py int
	x, y   int
	a      int
	col    color.RGBA
	drw    bool
}

func newPen() *Pen {
	return &Pen{
		lw:  4,
		col: color.RGBA{255, 255, 255, 255},
		drw: true,
	}
}

func (p *Pen) Hold() {
	p.drw = true
}

func (p *Pen) Release() {
	p.drw = false
}

func (p *Pen) Place(x, y int) {
	p.px, p.py = x, y
	p.Reset()
}

func (p *Pen) Left(d int) {
	p.a -= d
}

func (p *Pen) Right(d int) {
	p.a += d
}

func (p *Pen) Forward(n int) {
	t := f64.Deg2Rad(float64(p.a))
	r := float64(n)

	ox := r * math.Cos(t)
	oy := r * math.Sin(t)

	nx := f64.Iround(float64(p.x) + ox)
	ny := f64.Iround(float64(p.y) + oy)
	p.line(p.x, p.y, nx, ny)

	p.x, p.y = nx, ny
}

func (p *Pen) Backward(n int) {
	p.Forward(-n)
}

func (p *Pen) line(x1, y1, x2, y2 int) {
	if p.drw {
		sdlgfx.ThickLine(renderer, x1, y1, x2, y2, p.lw, p.col)
	}
}

func (p *Pen) Reset() {
	p.x, p.y = p.px, p.py
	p.a = 0
}

func (p *Pen) Color(c color.RGBA) {
	p.col = c
}
