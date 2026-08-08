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
	"github.com/qeedquan/go-media/math/rng"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
)

var (
	window    *sdl.Window
	renderer  *sdl.Renderer
	ps        *ParticleStream
	repeller  *Repeller
	fps       sdlgfx.FPSManager
	repelling = true
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

	window.SetTitle("Particle Repeller")
}

func reset() {
	w, h, _ := renderer.OutputSize()
	ps = newParticleStream(ga.Vec2d{float64(w) / 2, 50})
	repeller = newRepeller(float64(w)/2-20, float64(h)/2-50)
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
			case sdl.K_r:
				repelling = !repelling
			}
			fmt.Printf("repelling: %v\n", repelling)
		case sdl.MouseButtonDownEvent:
			mx, my, _ := sdl.GetMouseState()
			repeller.Clicked(float64(mx), float64(my))
		case sdl.MouseButtonUpEvent:
			repeller.StopDragging()
		}
	}
}

func update() {
	ps.AddParticle()
	gravity := ga.Vec2d{0, 0.1}
	ps.ApplyForce(gravity)
	if repelling {
		ps.ApplyRepeller(repeller)
	}
	ps.Update()

	mx, my, _ := sdl.GetMouseState()
	repeller.Drag()
	repeller.Hover(float64(mx), float64(my))
}

func display() {
	renderer.SetDrawColor(color.RGBA{150, 150, 150, 255})
	renderer.Clear()
	repeller.Display()
	ps.Display()
	renderer.Present()
}

type Particle struct {
	Pos  ga.Vec2d
	Vel  ga.Vec2d
	Acc  ga.Vec2d
	Life float64
	Mass float64
}

func newParticle(l ga.Vec2d) Particle {
	return Particle{
		Vel: ga.Vec2d{
			rng.Float64n(-1, 1),
			rng.Float64n(-2, 0),
		},
		Pos:  l,
		Life: 255,
		Mass: 1,
	}
}

func (p *Particle) ApplyForce(f ga.Vec2d) {
	f = vec2.Scale(f, 1/p.Mass)
	p.Acc = vec2.Add(p.Acc, f)
}

func (p *Particle) IsDead() bool {
	return p.Life < 0
}

func (p *Particle) Update() {
	p.Vel = vec2.Add(p.Vel, p.Acc)
	p.Pos = vec2.Add(p.Pos, p.Vel)
	p.Acc = ga.Vec2d{}
	p.Life -= 2
}

func (p *Particle) Display() {
	col := color.RGBA{127, 127, 127, uint8(p.Life)}
	sdlgfx.FilledCircle(renderer, int(p.Pos.X), int(p.Pos.Y), 12, col)
	sdlgfx.AACircle(renderer, int(p.Pos.X), int(p.Pos.Y), 12, color.RGBA{A: 255})

}

type ParticleStream struct {
	Particles []Particle
	Origin    ga.Vec2d
}

func newParticleStream(p ga.Vec2d) *ParticleStream {
	return &ParticleStream{
		Origin: p,
	}
}

func (s *ParticleStream) AddParticle() {
	s.Particles = append(s.Particles, newParticle(s.Origin))
}

func (s *ParticleStream) ApplyForce(f ga.Vec2d) {
	for i := range s.Particles {
		s.Particles[i].ApplyForce(f)
	}
}

func (s *ParticleStream) ApplyRepeller(r *Repeller) {
	for i := range s.Particles {
		f := r.Repel(&s.Particles[i])
		s.Particles[i].ApplyForce(f)
	}
}

func (s *ParticleStream) Update() {
	for i := len(s.Particles) - 1; i >= 0; {
		p := &s.Particles[i]
		p.Update()
		if p.IsDead() {
			n := len(s.Particles) - 1
			s.Particles[i], s.Particles = s.Particles[n], s.Particles[:n]
		} else {
			i--
		}
	}
}

func (s *ParticleStream) Display() {
	for _, p := range s.Particles {
		p.Display()
	}
}

type Repeller struct {
	Pos        ga.Vec2d
	R          float64
	G          float64
	Dragging   bool
	DragOffset ga.Vec2d
	Rollover   bool
}

func newRepeller(x, y float64) *Repeller {
	return &Repeller{
		Pos: ga.Vec2d{x, y},
		R:   1,
		G:   100,
	}
}

func (r *Repeller) Display() {
	col := color.RGBA{175, 175, 175, 255}
	if r.Dragging {
		col = color.RGBA{50, 50, 50, 200}
	} else if r.Rollover {
		col = color.RGBA{100, 100, 100, 200}
	}
	sdlgfx.FilledCircle(renderer, int(r.Pos.X), int(r.Pos.Y), 48, col)
}

func (r *Repeller) Repel(p *Particle) ga.Vec2d {
	dir := vec2.Sub(r.Pos, p.Pos)
	d := vec2.Len(dir)
	dir = vec2.Normalize(dir)
	d = ga.Clamp(d, 5, 100)

	force := -1 * r.G / (d * d)
	return vec2.Scale(dir, force)
}

func (r *Repeller) Clicked(mx, my float64) {
	m := ga.Vec2d{mx, my}
	d := vec2.Len(vec2.Sub(r.Pos, m))
	if d < 48 {
		r.Dragging = true
		r.DragOffset = vec2.Sub(r.Pos, m)
	}
}

func (r *Repeller) Hover(mx, my float64) {
	m := ga.Vec2d{mx, my}
	d := vec2.Len(vec2.Sub(r.Pos, m))
	r.Rollover = d < 48
}

func (r *Repeller) Drag() {
	mx, my, _ := sdl.GetMouseState()
	if r.Dragging {
		r.Pos.X = float64(mx) + r.DragOffset.X
		r.Pos.Y = float64(my) + r.DragOffset.Y
	}
}

func (r *Repeller) StopDragging() {
	r.Dragging = false
}
