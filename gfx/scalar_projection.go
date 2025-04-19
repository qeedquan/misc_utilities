// scalar projection of one vector onto another
// https://en.wikipedia.org/wiki/Scalar_projection

package main

import (
	"image/color"
	"log"
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
	fps      sdlgfx.FPSManager
	vectors  [2]ga.Vec2d
)

func main() {
	runtime.LockOSThread()
	rand.Seed(time.Now().UnixNano())
	initSDL()
	reset()
	for {
		event()
		blit()
		fps.Delay()
	}
}

func initSDL() {
	err := sdl.Init(sdl.INIT_VIDEO)
	ck(err)

	window, renderer, err = sdl.CreateWindowAndRenderer(1024, 768, sdl.WINDOW_RESIZABLE)
	ck(err)

	window.SetTitle("Scalar Projection")
}

func reset() {
	vectors = [2]ga.Vec2d{
		{20, 300},
		{500, 250},
	}
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
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
			}
		}
	}
}

func blit() {
	renderer.SetDrawColor(color.RGBA{100, 100, 100, 255})
	renderer.Clear()
	blitProjection()
	renderer.Present()
}

func blitProjection() {
	mx, my, _ := sdl.GetMouseState()
	a := vectors[0]
	b := vectors[1]
	sdlgfx.ThickLine(renderer, int(a.X), int(a.Y), int(b.X), int(b.Y), 4, color.RGBA{A: 255})
	sdlgfx.ThickLine(renderer, int(a.X), int(a.Y), int(mx), int(my), 4, color.RGBA{A: 255})

	sdlgfx.FilledCircle(renderer, int(a.X), int(a.Y), 8, color.RGBA{A: 255})
	sdlgfx.FilledCircle(renderer, int(b.X), int(b.Y), 8, color.RGBA{A: 255})

	m := ga.Vec2d{float64(mx), float64(my)}
	am := vec2.Sub(m, a)
	ab := vec2.Sub(b, a)
	norm := vec2.Proj(am, ab)
	norm = vec2.Add(a, norm)
	sdlgfx.ThickLine(renderer, int(mx), int(my), int(norm.X), int(norm.Y), 4, color.RGBA{A: 255})
	sdlgfx.FilledCircle(renderer, int(norm.X), int(norm.Y), 8, color.RGBA{R: 255, A: 255})
}
