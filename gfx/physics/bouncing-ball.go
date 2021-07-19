package main

import (
	"fmt"
	"image/color"
	"log"
	"math/rand"
	"os"
	"runtime"
	"time"

	"github.com/qeedquan/go-media/math/f64"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
)

var (
	window   *sdl.Window
	renderer *sdl.Renderer
	mover    *Mover
	ticker   *time.Ticker
	speedup  float64
	wind     f64.Vec2
	gravity  f64.Vec2
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
	mover = newMover()
	ticker = time.NewTicker(16 * time.Millisecond)
	speedup = 1

	wind = f64.Vec2{0.05, 0}
	gravity = f64.Vec2{0, 0.1}
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

	mover.ApplyForce(wind)
	mover.ApplyForce(gravity)
	mover.Update(speedup)
	mover.CheckEdges()
}

func blit() {
	renderer.SetDrawColor(color.RGBA{150, 150, 150, 255})
	renderer.Clear()
	mover.Blit()
	renderer.Present()
}

type Mover struct {
	Pos    f64.Vec2
	Vel    f64.Vec2
	Acc    f64.Vec2
	Mass   float64
	Radius float64
}

func newMover() *Mover {
	return &Mover{
		Pos:    f64.Vec2{30, 30},
		Mass:   1,
		Radius: 48,
	}
}

func (m *Mover) ApplyForce(force f64.Vec2) {
	f := force.Scale(1 / m.Mass)
	m.Acc = m.Acc.Add(f)
}

func (m *Mover) Update(t float64) {
	m.Vel = m.Vel.Add(m.Acc.Scale(t))
	m.Pos = m.Pos.AddScale(m.Vel, t)
	m.Pos = m.Pos.AddScale(m.Acc, t*t*0.5)
	m.Acc = f64.Vec2{}
}

func (m *Mover) Blit() {
	sdlgfx.FilledCircle(renderer, int(m.Pos.X), int(m.Pos.Y), int(m.Radius), color.RGBA{127, 127, 127, 255})
	sdlgfx.AACircle(renderer, int(m.Pos.X), int(m.Pos.Y), int(m.Radius), color.RGBA{0, 0, 0, 255})
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
