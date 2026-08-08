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
	width       float64
	height      float64
	mouseX      float64
	mouseY      float64
	mouseButton int
	window      *sdl.Window
	renderer    *sdl.Renderer
	fps         sdlgfx.FPSManager
	spring      *Spring
	bob         *Bob
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

	window.SetTitle("Spring")

	width, height = float64(w), float64(h)
}

func reset() {
	spring = newSpring(width/2, 10, 100)
	bob = newBob(width/2, 100)
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
			bob.Clicked(float64(ev.X), float64(ev.Y))
		case sdl.MouseButtonUpEvent:
			bob.StopDragging()
		}
	}
	w, h, _ := renderer.OutputSize()
	width, height = float64(w), float64(h)

	mx, my, mb := sdl.GetMouseState()
	mouseX, mouseY, mouseButton = float64(mx), float64(my), int(mb)
}

func update() {
	gravity := ga.Vec2d{0, 2}
	bob.ApplyForce(gravity)
	spring.Connect(bob)
	spring.ConstrainLength(bob, 30, 200)

	bob.Update()
	bob.Drag(mouseX, mouseY)
}

func display() {
	renderer.SetDrawColor(color.RGBA{150, 150, 150, 255})
	renderer.Clear()
	spring.DisplayLine(bob)
	bob.Display()
	spring.Display()
	renderer.Present()
}

type Bob struct {
	Pos        ga.Vec2d
	Vel        ga.Vec2d
	Acc        ga.Vec2d
	Mass       float64
	Damping    float64
	DragOffset ga.Vec2d
	Dragging   bool
}

func newBob(x, y float64) *Bob {
	return &Bob{
		Pos:     ga.Vec2d{x, y},
		Damping: 0.98,
		Mass:    24,
	}
}

// Euler integration
func (b *Bob) Update() {
	b.Vel = vec2.Add(b.Vel, b.Acc)
	b.Vel = vec2.Scale(b.Vel, b.Damping)
	b.Pos = vec2.Add(b.Pos, b.Vel)
	b.Acc = ga.Vec2d{}
}

// F = MA
func (b *Bob) ApplyForce(force ga.Vec2d) {
	f := vec2.Scale(force, 1/b.Mass)
	b.Acc = vec2.Add(b.Acc, f)
}

func (b *Bob) Display() {
	col := color.RGBA{175, 175, 175, 255}
	if b.Dragging {
		col = color.RGBA{50, 50, 50, 255}
	}
	sdlgfx.FilledCircle(renderer, int(b.Pos.X), int(b.Pos.Y), int(b.Mass), col)
	sdlgfx.AACircle(renderer, int(b.Pos.X), int(b.Pos.Y), int(b.Mass), color.RGBA{A: 255})

}

func (b *Bob) Clicked(mx, my float64) {
	m := ga.Vec2d{mx, my}
	d := vec2.Distance(b.Pos, m)
	if d < b.Mass {
		b.Dragging = true
		b.DragOffset = vec2.Sub(b.Pos, m)
	}
}

func (b *Bob) Drag(mx, my float64) {
	if b.Dragging {
		m := ga.Vec2d{mx, my}
		b.Pos = vec2.Add(m, b.DragOffset)
	}
}

func (b *Bob) StopDragging() {
	b.Dragging = false
}

type Spring struct {
	Anchor ga.Vec2d
	Len    float64
	K      float64
}

func newSpring(x, y, l float64) *Spring {
	return &Spring{
		Anchor: ga.Vec2d{x, y},
		Len:    l,
		K:      0.2,
	}
}

func (s *Spring) Connect(b *Bob) {
	F := vec2.Sub(b.Pos, s.Anchor)
	d := vec2.Len(F)
	stretch := d - s.Len

	// hooke's law
	// F = k * stretch
	F = vec2.Normalize(F)
	F = vec2.Scale(F, -1*s.K*stretch)
	b.ApplyForce(F)
}

func (s *Spring) ConstrainLength(b *Bob, minlen, maxlen float64) {
	dir := vec2.Sub(b.Pos, s.Anchor)
	d := vec2.Len(dir)
	if d < minlen {
		dir = vec2.Normalize(dir)
		dir = vec2.Scale(dir, minlen)
		b.Pos = vec2.Add(s.Anchor, dir)
		b.Vel = ga.Vec2d{}
	} else if d > maxlen {
		dir = vec2.Normalize(dir)
		dir = vec2.Scale(dir, maxlen)
		b.Pos = vec2.Add(s.Anchor, dir)
		b.Vel = ga.Vec2d{}
	}
}

func (s *Spring) Display() {
	renderer.SetDrawColor(color.RGBA{100, 100, 100, 255})
	renderer.DrawRect(&sdl.Rect{int32(s.Anchor.X - 10), int32(s.Anchor.Y - 10), 20, 20})
}

func (s *Spring) DisplayLine(b *Bob) {
	sdlgfx.ThickLine(renderer, int(b.Pos.X), int(b.Pos.Y), int(s.Anchor.X), int(s.Anchor.Y), 5, color.RGBA{100, 100, 100, 255})
}
