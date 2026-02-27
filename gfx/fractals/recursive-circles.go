package main

import (
	"image/color"
	"log"
	"math/rand"
	"os"
	"runtime"
	"time"

	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
)

var (
	window   *sdl.Window
	renderer *sdl.Renderer
	radius   float64
	mode     int
	animate  bool
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
	w, h := 1280, 800
	wflag := sdl.WINDOW_RESIZABLE
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, wflag)
	ck(err)

	window.SetTitle("Recursive Circles")
}

func reset() {
	radius = 1024
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
				animate = !animate
			case sdl.K_LEFT:
				if mode > 0 {
					mode--
				}
			case sdl.K_RIGHT:
				if mode < 2 {
					mode++
				}
			}
		}
	}
}

func blit() {
	renderer.SetDrawColor(color.RGBA{150, 150, 150, 255})
	renderer.Clear()
	w, h, _ := renderer.OutputSize()
	switch mode {
	case 0:
		blitCircle1(w/2, h/2, radius)
	case 1:
		blitCircle2(w/2, h/2, radius)
	case 2:
		blitCircle4(w/2, h/2, radius/2)
	}
	renderer.Present()
}

func blitCircle1(x, y int, r float64) {
	p := r
	if animate {
		p *= 5 * rand.Float64()
	}

	sdlgfx.AACircle(renderer, x, y, int(p), color.RGBA{0, 0, 0, 255})
	if r > 2 {
		r *= 0.75
		blitCircle1(x, y, r)
	}
}

func blitCircle2(x, y int, r float64) {
	p := r
	if animate {
		p *= 5 * rand.Float64()
	}

	sdlgfx.AACircle(renderer, x, y, int(p), color.RGBA{0, 0, 0, 255})
	if r > 2 {
		blitCircle2(x+int(r/2), y, r/2)
		blitCircle2(x-int(r/2), y, r/2)
	}
}

func blitCircle4(x, y int, r float64) {
	p := r
	if animate {
		p *= 5 * rand.Float64()
	}

	sdlgfx.AACircle(renderer, x, y, int(p), color.RGBA{0, 0, 0, 255})
	if r > 8 {
		blitCircle4(x+int(r/2), y, r/2)
		blitCircle4(x-int(r/2), y, r/2)
		blitCircle4(x, y+int(r/2), r/2)
		blitCircle4(x, y-int(r/2), r/2)
	}
}
