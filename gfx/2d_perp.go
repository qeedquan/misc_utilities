// demonstrates how to calculate a perpendicular line to 2d
// given 2 points, make a line out of that based on the unit
// circle by the basis <cos(t), sin(t)>, then use the perp
// operator to get a directional basis out of it then we just
// scale it to get the point, we center it
package main

import (
	"math"
	"os"
	"runtime"

	"github.com/qeedquan/go-media/math/ga"
	"github.com/qeedquan/go-media/math/ga/vec2"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
	"github.com/qeedquan/go-media/sdl/sdlimage/sdlcolor"
)

var (
	window   *sdl.Window
	renderer *sdl.Renderer
	fps      sdlgfx.FPSManager
)

func main() {
	runtime.LockOSThread()
	sdl.Init(sdl.INIT_EVERYTHING)
	window, renderer, _ = sdl.CreateWindowAndRenderer(800, 600, 0)
	window.SetTitle("Perpendicular Line")

	renderer.SetDrawColor(sdlcolor.Black)
	renderer.Clear()

	fps.Init()
	fps.SetRate(60)

	R := math.Pi / 2
	L := 100.0
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
					return
				}
			}
		}

		x, y, button := sdl.GetMouseState()
		if button&1 != 0 {
			R += 0.1
		} else if button&4 != 0 {
			R -= 0.1
		}

		c := math.Cos(R) * L
		s := math.Sin(R) * L

		C := ga.Vec2d{float64(x), float64(y)}
		P := ga.Vec2d{C.X - c, C.Y - s}
		Q := ga.Vec2d{C.X + c, C.Y + s}

		renderer.SetDrawColor(sdlcolor.Black)
		renderer.Clear()

		U := vec2.Perp(vec2.Sub(Q, P))
		U = vec2.Normalize(U)
		U.X *= L
		U.Y *= L
		U = vec2.Add(C, U)

		line(P, Q)
		line(C, U)

		renderer.Present()
		fps.Delay()
	}
}

func line(P, Q ga.Vec2d) {
	renderer.SetDrawColor(sdlcolor.White)
	renderer.DrawLine(int(P.X), int(P.Y), int(Q.X), int(Q.Y))
}
