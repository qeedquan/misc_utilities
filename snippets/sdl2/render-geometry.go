package main

import (
	"os"
	"runtime"

	"github.com/qeedquan/go-media/sdl"
)

func main() {
	runtime.LockOSThread()
	window, _ := sdl.CreateWindow("Triangle Example", sdl.WINDOWPOS_CENTERED, sdl.WINDOWPOS_CENTERED, 800, 600, 0)
	renderer, _ := sdl.CreateRenderer(window, -1, sdl.RENDERER_ACCELERATED)

	var vert [3]sdl.Vertex
	// center
	vert[0].Position.X = 400
	vert[0].Position.Y = 150
	vert[0].Color.R = 255
	vert[0].Color.G = 0
	vert[0].Color.B = 0
	vert[0].Color.A = 255

	// left
	vert[1].Position.X = 200
	vert[1].Position.Y = 450
	vert[1].Color.R = 0
	vert[1].Color.G = 0
	vert[1].Color.B = 255
	vert[1].Color.A = 255

	// right
	vert[2].Position.X = 600
	vert[2].Position.Y = 450
	vert[2].Color.R = 0
	vert[2].Color.G = 255
	vert[2].Color.B = 0
	vert[2].Color.A = 255

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

		renderer.SetDrawColor(sdl.Color{0, 0, 0, 255})
		renderer.Clear()
		renderer.Geometry(nil, vert[:], nil)
		renderer.Present()
	}
}
