// http://fabiensanglard.net/fizzlefade/index.php
// use of LFSR to generate fill pixels on screen
package main

import (
	"os"
	"runtime"

	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlimage/sdlcolor"
)

const (
	WIDTH  = 320
	HEIGHT = 200
)

var (
	window   *sdl.Window
	renderer *sdl.Renderer
)

func main() {
	runtime.LockOSThread()
	sdl.Init(sdl.INIT_EVERYTHING)
	window, renderer, _ = sdl.CreateWindowAndRenderer(WIDTH, HEIGHT, 0)
	window.SetTitle("Fizzle Fade")

	renderer.SetDrawColor(sdlcolor.Black)
	renderer.Clear()

	fizzlefade()
}

func fizzlefade() {
	rnd := 1
	for {
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
				}
			}
		}

		y := (rnd & 0x000ff)
		x := (rnd & 0x1ff00) >> 8
		lsb := rnd & 1
		rnd >>= 1
		if lsb != 0 {
			rnd ^= 0x00012000
		}
		if x < WIDTH && y < HEIGHT {
			renderer.SetDrawColor(sdlcolor.Red)
			renderer.DrawPoint(x, y)
			renderer.Present()
		}
	}
}
