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

	window.SetTitle("Attractor")

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
	gravity := f64.Vec2{0, 2}
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
	Pos        f64.Vec2
	Vel        f64.Vec2
	Acc        f64.Vec2
	Mass       float64
	Damping    float64
	DragOffset f64.Vec2
	Dragging   bool
}

func newBob(x, y float64) *Bob {
	return &Bob{
		Pos:     f64.Vec2{x, y},
		Damping: 0.98,
		Mass:    24,
	}
}

// Euler integration
func (b *Bob) Update() {
	b.Vel = b.Vel.Add(b.Acc)
	b.Vel = b.Vel.Scale(b.Damping)
	b.Pos = b.Pos.Add(b.Vel)
	b.Acc = f64.Vec2{}
}

// F = MA
func (b *Bob) ApplyForce(force f64.Vec2) {
	f := force.Scale(1 / b.Mass)
	b.Acc = b.Acc.Add(f)
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
	m := f64.Vec2{mx, my}
	d := b.Pos.Distance(m)
	if d < b.Mass {
		b.Dragging = true
		b.DragOffset = b.Pos.Sub(m)
	}
}

func (b *Bob) Drag(mx, my float64) {
	if b.Dragging {
		m := f64.Vec2{mx, my}
		b.Pos = m.Add(b.DragOffset)
	}
}

func (b *Bob) StopDragging() {
	b.Dragging = false
}

type Spring struct {
	Anchor f64.Vec2
	Len    float64
	K      float64
}

func newSpring(x, y, l float64) *Spring {
	return &Spring{
		Anchor: f64.Vec2{x, y},
		Len:    l,
		K:      0.2,
	}
}

func (s *Spring) Connect(b *Bob) {
	F := b.Pos.Sub(s.Anchor)
	d := F.Len()
	stretch := d - s.Len

	// hooke's law
	// F = k * stretch
	F = F.Normalize()
	F = F.Scale(-1 * s.K * stretch)
	b.ApplyForce(F)
}

func (s *Spring) ConstrainLength(b *Bob, minlen, maxlen float64) {
	dir := b.Pos.Sub(s.Anchor)
	d := dir.Len()
	if d < minlen {
		dir = dir.Normalize()
		dir = dir.Scale(minlen)
		b.Pos = s.Anchor.Add(dir)
		b.Vel = f64.Vec2{}
	} else if d > maxlen {
		dir = dir.Normalize()
		dir = dir.Scale(maxlen)
		b.Pos = s.Anchor.Add(dir)
		b.Vel = f64.Vec2{}
	}
}

func (s *Spring) Display() {
	renderer.SetDrawColor(color.RGBA{100, 100, 100, 255})
	renderer.DrawRect(&sdl.Rect{int32(s.Anchor.X - 10), int32(s.Anchor.Y - 10), 20, 20})
}

func (s *Spring) DisplayLine(b *Bob) {
	sdlgfx.ThickLine(renderer, int(b.Pos.X), int(b.Pos.Y), int(s.Anchor.X), int(s.Anchor.Y), 5, color.RGBA{100, 100, 100, 255})
}
