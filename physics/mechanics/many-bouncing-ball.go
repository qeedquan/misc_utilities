package main

import (
	"fmt"
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
	movers   []*Mover
	ticker   *time.Ticker
	speedup  float64
	wind     ga.Vec2d
	gravity  ga.Vec2d
	fps      sdlgfx.FPSManager
)

func main() {
	runtime.LockOSThread()
	rand.Seed(time.Now().UnixNano())
	initSDL()
	reset()
	for {
		event()
		update()
		blit()
		fps.Delay()
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

	window.SetTitle("Bouncing Balls")
}

func reset() {
	w, h, _ := renderer.OutputSize()
	for i := 0; i < 200; i++ {
		movers = append(movers, newMover(0.1+rand.Float64()*4, rand.Float64()*float64(w), rand.Float64()*float64(h)))
	}

	ticker = time.NewTicker(16 * time.Millisecond)
	speedup = 1

	wind = ga.Vec2d{0.1, 0}
	gravity = ga.Vec2d{0, 0.1}

	fps.SetRate(60)
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
				speedup -= 1
			case sdl.K_RIGHT:
				speedup += 1
			case sdl.K_q:
				wind.X -= 0.1
			case sdl.K_w:
				wind.X += 0.1
			case sdl.K_a:
				gravity.Y -= 0.1
			case sdl.K_s:
				gravity.Y += 0.1
			}

			fmt.Printf("speedup: %v\n", speedup)
			fmt.Printf("wind: %v\n", wind)
			fmt.Printf("gravity: %v\n", gravity)
			fmt.Println()
		}
	}
}

func update() {
	select {
	case <-ticker.C:
	default:
		return
	}

	for i := range movers {
		movers[i].ApplyForce(wind)
		movers[i].ApplyForce(vec2.Scale(gravity, 1/movers[i].Mass))
		movers[i].Update(speedup)
		movers[i].CheckEdges()
	}
}

func blit() {
	renderer.SetDrawColor(color.RGBA{150, 150, 150, 255})
	renderer.Clear()
	for i := range movers {
		movers[i].Blit()
	}
	renderer.Present()
}

type Mover struct {
	Pos    ga.Vec2d
	Vel    ga.Vec2d
	Acc    ga.Vec2d
	Mass   float64
	Radius float64
}

func newMover(m, x, y float64) *Mover {
	return &Mover{
		Pos:    ga.Vec2d{x, y},
		Mass:   m,
		Radius: 16 * m,
	}
}

func (m *Mover) ApplyForce(force ga.Vec2d) {
	f := vec2.Scale(force, 1/m.Mass)
	m.Acc = vec2.Add(m.Acc, f)
}

func (m *Mover) Update(t float64) {
	m.Vel = vec2.Fmas(m.Vel, m.Acc, t)
	m.Pos = vec2.Fmas(m.Pos, m.Vel, t)
	m.Pos = vec2.Fmas(m.Pos, m.Acc, t*t*0.5)
	m.Acc = ga.Vec2d{}
}

func (m *Mover) Blit() {
	sdlgfx.FilledCircle(renderer, int(m.Pos.X), int(m.Pos.Y), int(m.Radius), color.RGBA{127, 127, 127, 127})
	sdlgfx.AACircle(renderer, int(m.Pos.X), int(m.Pos.Y), int(m.Radius), color.RGBA{0, 0, 0, 127})
}

func (m *Mover) CheckEdges() {
	iw, ih, _ := renderer.OutputSize()
	w, h := float64(iw), float64(ih)
	if m.Pos.X+m.Radius > w {
		m.Pos.X = w - m.Radius
		m.Vel.X *= -1
	} else if m.Pos.X < 0 {
		m.Pos.X = 0
		m.Vel.X *= -1
	}

	if m.Pos.Y+m.Radius > h {
		m.Pos.Y = h - m.Radius
		m.Vel.Y *= -1
	}
}
