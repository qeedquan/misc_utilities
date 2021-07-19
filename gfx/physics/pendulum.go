package main

import (
	"image/color"
	"log"
	"math"
	"math/rand"
	"os"
	"runtime"
	"time"

	"github.com/qeedquan/go-media/math/f64"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
)

var (
	width       float64
	height      float64
	mouseX      float64
	mouseY      float64
	mouseButton int
	window      *sdl.Window
	renderer    *sdl.Renderer
	pendulum    *Pendulum
	fps         sdlgfx.FPSManager
)

func main() {
	runtime.LockOSThread()
	rand.Seed(time.Now().UnixNano())
	initSDL()
	reset()
	for {
		event()
		update()
		display()
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

	window.SetTitle("Pendulum")

	width, height = float64(w), float64(h)
}

func reset() {
	pendulum = newPendulum(f64.Vec2{width / 2, 0}, 175)
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
			}
		case sdl.MouseButtonDownEvent:
			pendulum.Clicked(float64(ev.X), float64(ev.Y))
		case sdl.MouseButtonUpEvent:
			pendulum.StopDragging()
		}
	}
	w, h, _ := renderer.OutputSize()
	width, height = float64(w), float64(h)

	mx, my, mb := sdl.GetMouseState()
	mouseX, mouseY, mouseButton = float64(mx), float64(my), int(mb)
}

func update() {
	pendulum.Update()
	pendulum.Drag()
}

func display() {
	renderer.SetDrawColor(color.RGBA{150, 150, 150, 255})
	renderer.Clear()
	pendulum.Display()
	renderer.Present()
}

type Pendulum struct {
	Position     f64.Vec2
	Origin       f64.Vec2
	Radius       float64
	Angle        float64
	Velocity     float64
	Acceleration float64
	BallRadius   float64
	Damping      float64
	Dragging     bool
}

func newPendulum(origin f64.Vec2, radius float64) *Pendulum {
	p := &Pendulum{
		Origin:     origin,
		Radius:     radius,
		Angle:      math.Pi / 4,
		Damping:    0.995,
		BallRadius: 48,
	}
	return p
}

func (p *Pendulum) Update() {
	if !p.Dragging {
		gravity := 0.4
		// http://physicslab.com/pendulum1.html
		p.Acceleration = (-1 * gravity / p.Radius) * math.Sin(p.Angle)
		p.Velocity += p.Acceleration
		p.Velocity *= p.Damping
		p.Angle += p.Velocity
	}
}

func (p *Pendulum) Clicked(mx, my float64) {
	m := f64.Vec2{mx, my}
	d := p.Position.Distance(m)
	if d < p.BallRadius {
		p.Dragging = true
	}
}

func (p *Pendulum) Drag() {
	if p.Dragging {
		m := f64.Vec2{mouseX, mouseY}
		d := p.Origin.Sub(m)
		// angle relative to vertical axis
		p.Angle = math.Atan2(-1*d.Y, d.X) - f64.Deg2Rad(90)
	}
}

func (p *Pendulum) StopDragging() {
	if p.Dragging {
		p.Velocity = 0
		p.Dragging = false
	}
}

func (p *Pendulum) Display() {
	p.Position = f64.Vec2{
		p.Radius * math.Sin(p.Angle),
		p.Radius * math.Cos(p.Angle),
	}.Add(p.Origin)

	sdlgfx.AALine(renderer, int(p.Origin.X), int(p.Origin.Y), int(p.Position.X), int(p.Position.Y), color.RGBA{A: 255})
	col := color.RGBA{175, 175, 175, 255}
	if p.Dragging {
		col = color.RGBA{A: 255}
	}

	sdlgfx.FilledCircle(renderer, int(p.Position.X), int(p.Position.Y), int(p.BallRadius), col)
	sdlgfx.AACircle(renderer, int(p.Position.X), int(p.Position.Y), int(p.BallRadius), color.RGBA{A: 255})
}
