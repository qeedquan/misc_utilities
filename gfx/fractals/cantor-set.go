// https://en.wikipedia.org/wiki/Cantor_set
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
	length   float64
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
	w, _, _ := renderer.OutputSize()
	blitCantor1(0, 0, float64(w)*4, mode)
	renderer.Present()
}

func blitCantor1(x, y, l float64, mode int) {
	if l >= 1 {
		switch mode {
		case 0:
			sdlgfx.ThickLine(renderer, int(x), int(y), int(x+l), int(y), 5, color.RGBA{0, 0, 0, 255})
		case 1:
			sdlgfx.AACircle(renderer, int(x), int(y), int(l), color.RGBA{0, 0, 0, 255})
		}
		y += 20
		blitCantor1(x, y, l/3, mode)
		blitCantor1(x+l*2/3, y, l/3, mode)
	}
}
