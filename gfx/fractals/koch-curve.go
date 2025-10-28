// https://en.wikipedia.org/wiki/Koch_snowflake
package main

import (
	"image/color"
	"log"
	"math"
	"math/rand"
	"os"
	"runtime"
	"time"

	"github.com/qeedquan/go-media/math/ga"
	"github.com/qeedquan/go-media/math/ga/vec2"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
)

var (
	window   *sdl.Window
	renderer *sdl.Renderer
	mode     = 0
	iters    = 5
	koch     []line
)

func main() {
	runtime.LockOSThread()
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
	w, h := 600, 692
	wflag := sdl.WINDOW_RESIZABLE
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, wflag)
	ck(err)

	window.SetTitle("Koch Curve")
}

func reset() {
	w, h, _ := renderer.OutputSize()

	var l []line
	switch mode {
	case 0:
		l = []line{
			{start: ga.Vec2d{0, float64(h / 2)}, end: ga.Vec2d{float64(w), float64(h / 2)}},
		}
	case 1:
		a := ga.Vec2d{0, 173}
		b := ga.Vec2d{float64(w), 173}
		c := ga.Vec2d{float64(w / 2), 173 + float64(w)*math.Cos(ga.Deg2Rad(30.0))}
		l = []line{
			{a, b},
			{b, c},
			{c, a},
		}
	}
	koch = generate(l, iters)
}

func generate(l []line, n int) []line {
	for i := 0; i < n; i++ {
		var lp []line
		for _, l := range l {
			a := l.start

			b := vec2.Sub(l.end, l.start)
			b = vec2.Scale(b, 1.0/3)
			b = vec2.Add(b, l.start)

			c := l.start
			t := vec2.Sub(l.end, l.start)
			t = vec2.Scale(t, 1.0/3)
			c = vec2.Add(c, t)
			t = vec2.Rotate(t, -ga.Deg2Rad(60.0))
			c = vec2.Add(c, t)

			d := vec2.Sub(l.end, l.start)
			d = vec2.Scale(d, 2.0/3)
			d = vec2.Add(d, l.start)

			e := l.end

			lp = append(lp, line{a, b})
			lp = append(lp, line{b, c})
			lp = append(lp, line{c, d})
			lp = append(lp, line{d, e})
		}
		l = lp
	}
	return l
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
			case sdl.K_LEFT:
				if iters > 0 {
					iters--
					reset()
				}
			case sdl.K_RIGHT:
				iters++
				reset()
			case sdl.K_UP:
				if mode > 0 {
					mode--
					reset()
				}
			case sdl.K_DOWN:
				if mode < 1 {
					mode++
					reset()
				}
			}
		}
	}
}

func blit() {
	renderer.SetDrawColor(color.RGBA{150, 150, 150, 255})
	renderer.Clear()
	for _, l := range koch {
		sdlgfx.ThickLine(renderer, int(l.start.X), int(l.start.Y), int(l.end.X), int(l.end.Y), 5, color.RGBA{0, 0, 0, 255})
	}
	renderer.Present()
}

type line struct {
	start, end ga.Vec2d
}
