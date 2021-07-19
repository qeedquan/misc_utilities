package main

import (
	"fmt"
	"log"
	"os"
	"runtime"

	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
)

var (
	window   *sdl.Window
	renderer *sdl.Renderer
	radius   = 1
	degree1  = 0
	degree2  = 10
)

func main() {
	runtime.LockOSThread()
	initSDL()
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

	w, h := 1440, 900
	wflag := sdl.WINDOW_RESIZABLE
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, wflag)
	window.SetTitle("Rounded Rectangle")
	sdlgfx.SetFont(sdlgfx.Font10x20, 10, 20)
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
				radius--
			case sdl.K_RIGHT:
				radius++
			case sdl.K_1:
				degree1--
			case sdl.K_2:
				degree1++
			case sdl.K_3:
				degree2--
			case sdl.K_4:
				degree2++
			}
		}
	}
}

func blit() {
	renderer.SetDrawColor(sdl.Color{4, 35, 39, 255})
	renderer.Clear()
	sdlgfx.RoundedRectangle(renderer, 512, 512, 640, 640, radius, sdl.Color{255, 255, 255, 255})
	sdlgfx.RoundedBox(renderer, 400, 400, 500, 500, radius, sdl.Color{255, 255, 255, 255})
	sdlgfx.Arc(renderer, 600, 700, radius, degree1, degree2, sdl.Color{255, 255, 255, 255})
	text := fmt.Sprintf("Radius: %v Degree 1: %d Degree 2: %d", radius, degree1, degree2)
	sdlgfx.String(renderer, 0, 0, sdl.Color{255, 255, 255, 255}, text)
	renderer.Present()
}
