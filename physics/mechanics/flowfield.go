package main

import (
	"image/color"
	"log"
	"math"
	"math/rand"
	"os"
	"runtime"
	"time"

	"github.com/qeedquan/go-media/math/ga"
	"github.com/qeedquan/go-media/math/ga/mat2"
	"github.com/qeedquan/go-media/math/ga/vec2"
	"github.com/qeedquan/go-media/math/rng/valuenoise"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
)

var (
	width       int
	height      int
	mouseX      int
	mouseY      int
	mouseButton int
	window      *sdl.Window
	renderer    *sdl.Renderer
	flowfield   *FlowField
	vehicles    []*Vehicle
	noiser      *valuenoise.Rand
	fps         sdlgfx.FPSManager
	distort     = 0.5
	debug       = true
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

	window.SetTitle("Flow Field")
	fps.SetRate(60)

	width, height = w, h
}

func reset() {
	vehicles = make([]*Vehicle, 512)
	for i := range vehicles {
		pos := ga.Vec2d{rand.Float64() * float64(width), rand.Float64() * float64(height)}
		vehicles[i] = newVehicle(pos, 2+3*rand.Float64(), 0.1+rand.Float64()*0.4)
	}
	flowfield = newFlowField(20)
}

func updateEvent() {
	w, h, _ := renderer.OutputSize()
	width, height = w, h

	mx, my, mb := sdl.GetMouseState()
	mouseX, mouseY, mouseButton = int(mx), int(my), int(mb)
}

func event() {
	for {
		updateEvent()
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
				distort -= 0.1
				flowfield.Init()
			case sdl.K_RIGHT:
				distort += 0.1
				flowfield.Init()
			case sdl.K_SPACE:
				flowfield.Init()
			case sdl.K_BACKSPACE:
				debug = !debug
			}
		case sdl.WindowEvent:
			switch ev.Event {
			case sdl.WINDOWEVENT_RESIZED:
				reset()
			}
		}
	}
}

func update() {
	for i := range vehicles {
		vehicles[i].Follow(flowfield)
		vehicles[i].Update()
	}
}

func display() {
	renderer.SetDrawColor(color.RGBA{150, 150, 150, 255})
	renderer.Clear()
	if debug {
		flowfield.Display()
	}
	for i := range vehicles {
		vehicles[i].Display()
	}
	renderer.Present()
}

type FlowField struct {
	Field      [][]ga.Vec2d
	Rows, Cols int
	Resolution int
}

func newFlowField(r int) *FlowField {
	cols := width / r
	rows := height / r
	field := make([][]ga.Vec2d, cols)
	for i := range field {
		field[i] = make([]ga.Vec2d, rows)
	}
	f := &FlowField{
		Field:      field,
		Cols:       cols,
		Rows:       rows,
		Resolution: r,
	}
	f.Init()
	return f
}

func (f *FlowField) Init() {
	noiser = valuenoise.New(512, valuenoise.Smoother)
	xoff := 0.0
	for i := 0; i < f.Cols; i++ {
		yoff := 0.0
		for j := 0; j < f.Rows; j++ {
			t := noiser.Gen2D(xoff, yoff) * distort
			t = ga.LinearRemap(t, 0, 1, 0, 2*math.Pi)
			f.Field[i][j] = ga.Vec2d{math.Cos(t), math.Sin(t)}
			yoff += 0.1
		}
		xoff += 0.1
	}
}

func (f *FlowField) Display() {
	for i := 0; i < f.Cols; i++ {
		for j := 0; j < f.Rows; j++ {
			f.drawVector(f.Field[i][j], float64(i*f.Resolution), float64(j*f.Resolution), float64(f.Resolution-2))
		}
	}
}

func (f *FlowField) drawVector(v ga.Vec2d, x, y, s float64) {
	p := vec2.Scale(v, s)
	p = vec2.Add(p, ga.Vec2d{x, y})
	col := color.RGBA{50, 50, 50, 100}
	sdlgfx.ThickLine(renderer, int(x), int(y), int(p.X), int(p.Y), 3, col)
}

func (f *FlowField) Lookup(p ga.Vec2d) ga.Vec2d {
	col := int(p.X / float64(f.Resolution))
	row := int(p.Y / float64(f.Resolution))
	if col >= f.Cols {
		col = f.Cols - 1
	}
	if row >= f.Rows {
		row = f.Rows - 1
	}
	return f.Field[col][row]
}

type Vehicle struct {
	Pos      ga.Vec2d
	Vel      ga.Vec2d
	Acc      ga.Vec2d
	R        float64
	MaxForce float64
	MaxSpeed float64
}

func newVehicle(l ga.Vec2d, ms, mf float64) *Vehicle {
	return &Vehicle{
		Pos:      l,
		R:        3,
		MaxSpeed: ms,
		MaxForce: mf,
	}
}

func (v *Vehicle) Display() {
	theta := math.Atan2(v.Vel.Y, v.Vel.X) + ga.Deg2Rad(90.0)
	M := mat2.Rotation(theta)

	p1 := ga.Vec2d{0, -v.R * 4}
	p2 := ga.Vec2d{-v.R, v.R * 4}
	p3 := ga.Vec2d{v.R, v.R * 4}

	p1 = mat2.Apply(&M, p1)
	p2 = mat2.Apply(&M, p2)
	p3 = mat2.Apply(&M, p3)

	p1 = vec2.Add(p1, v.Pos)
	p2 = vec2.Add(p2, v.Pos)
	p3 = vec2.Add(p3, v.Pos)

	col := color.RGBA{175, 175, 175, 255}
	sdlgfx.FilledTrigon(renderer, int(p1.X), int(p1.Y), int(p2.X), int(p2.Y), int(p3.X), int(p3.Y), col)
}

// Reynold's flow following algorithm
// http://www.red3d.com/cwr/steer/FlowFollow.html
func (v *Vehicle) Follow(flow *FlowField) {
	desired := flow.Lookup(v.Pos)
	desired = vec2.Scale(desired, v.MaxSpeed)

	steer := vec2.Sub(desired, v.Vel)
	if vec2.Len(steer) > v.MaxForce {
		steer = vec2.Normalize(steer)
		steer = vec2.Scale(steer, v.MaxForce)
	}
	v.ApplyForce(steer)
}

func (v *Vehicle) ApplyForce(force ga.Vec2d) {
	v.Acc = vec2.Add(v.Acc, force)
}

func (v *Vehicle) Update() {
	v.Vel = vec2.Add(v.Vel, v.Acc)
	if vec2.Len(v.Vel) > v.MaxSpeed {
		v.Vel = vec2.Normalize(v.Vel)
		v.Vel = vec2.Scale(v.Vel, v.MaxSpeed)
	}
	v.Pos = vec2.Add(v.Pos, v.Vel)
	v.Acc = ga.Vec2d{}
	v.borders()
}

func (v *Vehicle) borders() {
	if v.Pos.X < -v.R {
		v.Pos.X = float64(width) + v.R
	}
	if v.Pos.Y < -v.R {
		v.Pos.Y = float64(height) + v.R
	}
	if v.Pos.X > float64(width)+v.R {
		v.Pos.X = -v.R
	}
	if v.Pos.Y > float64(height)+v.R {
		v.Pos.Y = -v.R
	}
}
