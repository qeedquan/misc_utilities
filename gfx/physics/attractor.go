package main

import (
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
	width       float64
	height      float64
	mouseX      float64
	mouseY      float64
	mouseButton int
	window      *sdl.Window
	renderer    *sdl.Renderer
	mover       *Mover
	attractor   *Attractor
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

	window.SetTitle("Attractor")

	width, height = float64(w), float64(h)
}

func reset() {
	mover = newMover()
	attractor = newAttractor()
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
			attractor.Clicked(float64(ev.X), float64(ev.Y))
		case sdl.MouseButtonUpEvent:
			attractor.StopDragging()
		}
	}
	w, h, _ := renderer.OutputSize()
	width, height = float64(w), float64(h)

	mx, my, mb := sdl.GetMouseState()
	mouseX, mouseY, mouseButton = float64(mx), float64(my), int(mb)
}

func update() {
	force := attractor.Attract(mover)
	mover.ApplyForce(force)
	mover.Update()
	mover.CheckEdges()

	attractor.Drag()
	attractor.Hover(mouseX, mouseY)
}

func display() {
	renderer.SetDrawColor(color.RGBA{150, 150, 150, 255})
	renderer.Clear()
	mover.Display()
	attractor.Display()
	renderer.Present()
}

type Mover struct {
	Mass float64
	Pos  f64.Vec2
	Vel  f64.Vec2
	Acc  f64.Vec2
}

func newMover() *Mover {
	return &Mover{
		Pos:  f64.Vec2{500, 150},
		Vel:  f64.Vec2{1, 0},
		Mass: 1,
	}
}

func (m *Mover) ApplyForce(force f64.Vec2) {
	f := force.Scale(1 / m.Mass)
	m.Acc = m.Acc.Add(f)
}

func (m *Mover) Update() {
	m.Vel = m.Vel.Add(m.Acc)
	m.Pos = m.Pos.Add(m.Vel)
	m.Acc = f64.Vec2{}
}

func (m *Mover) CheckEdges() {
	if m.Pos.X > width {
		m.Pos.X = width
		m.Vel.X *= -1
	} else if m.Pos.X < 0 {
		m.Pos.X = 0
		m.Vel.X *= -1
	}

	if m.Pos.Y > height {
		m.Pos.Y = height
		m.Vel.Y *= -1
	} else if m.Pos.Y < 0 {
		m.Pos.Y = 0
		m.Vel.Y *= -1
	}
}

func (m *Mover) Display() {
	sdlgfx.FilledCircle(renderer, int(m.Pos.X), int(m.Pos.Y), 16, color.RGBA{127, 127, 127, 127})
	sdlgfx.AACircle(renderer, int(m.Pos.X), int(m.Pos.Y), 16, color.RGBA{0, 0, 0, 127})
}

type Attractor struct {
	Mass       float64
	G          float64
	Pos        f64.Vec2
	Dragging   bool
	Rollover   bool
	DragOffset f64.Vec2
}

func newAttractor() *Attractor {
	return &Attractor{
		Pos:  f64.Vec2{width / 2, height / 2},
		Mass: 20,
		G:    1,
	}
}

func (a *Attractor) Attract(m *Mover) f64.Vec2 {
	f := a.Pos.Sub(m.Pos)

	// limiting the distance to eliminate extreme results for very close
	// or very far objects
	d := f.Len()
	d = f64.Clamp(d, 5, 25)

	// just want the force direction
	f = f.Normalize()

	// calculate the strength of 2 object interaction using
	// newton universal gravitational force
	s := (a.G * m.Mass * a.Mass) / (d * d)

	// force is magnitude * direction
	f = f.Scale(s)

	return f
}

func (a *Attractor) Clicked(mx, my float64) {
	m := f64.Vec2{mx, my}
	d := a.Pos.Distance(m)
	if d < a.Mass {
		a.Dragging = true
		a.DragOffset = a.Pos.Sub(m)
	}
}

func (a *Attractor) Hover(mx, my float64) {
	m := f64.Vec2{mx, my}
	d := a.Pos.Distance(m)
	a.Rollover = d < a.Mass
}

func (a *Attractor) Drag() {
	if a.Dragging {
		a.Pos.X = mouseX + a.DragOffset.X
		a.Pos.Y = mouseY + a.DragOffset.Y
	}
}

func (a *Attractor) StopDragging() {
	a.Dragging = false
}

func (a *Attractor) Display() {
	col := color.RGBA{175, 175, 175, 200}
	if a.Dragging {
		col = color.RGBA{50, 50, 50, 200}
	} else if a.Rollover {
		col = color.RGBA{100, 100, 100, 200}
	}
	sdlgfx.FilledCircle(renderer, int(a.Pos.X), int(a.Pos.Y), int(a.Mass*2), col)
	sdlgfx.AACircle(renderer, int(a.Pos.X), int(a.Pos.Y), int(a.Mass*2), color.RGBA{A: 255})
}
