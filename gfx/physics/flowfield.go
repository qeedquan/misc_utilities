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
		pos := f64.Vec2{rand.Float64() * float64(width), rand.Float64() * float64(height)}
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
	Field      [][]f64.Vec2
	Rows, Cols int
	Resolution int
}

func newFlowField(r int) *FlowField {
	cols := width / r
	rows := height / r
	field := make([][]f64.Vec2, cols)
	for i := range field {
		field[i] = make([]f64.Vec2, rows)
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
			t = f64.LinearRemap(t, 0, 1, 0, 2*math.Pi)
			f.Field[i][j] = f64.Vec2{math.Cos(t), math.Sin(t)}
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

func (f *FlowField) drawVector(v f64.Vec2, x, y, s float64) {
	p := v.Scale(s)
	p = p.Add(f64.Vec2{x, y})
	col := color.RGBA{50, 50, 50, 100}
	sdlgfx.ThickLine(renderer, int(x), int(y), int(p.X), int(p.Y), 3, col)
}

func (f *FlowField) Lookup(p f64.Vec2) f64.Vec2 {
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
	Pos      f64.Vec2
	Vel      f64.Vec2
	Acc      f64.Vec2
	R        float64
	MaxForce float64
	MaxSpeed float64
}

func newVehicle(l f64.Vec2, ms, mf float64) *Vehicle {
	return &Vehicle{
		Pos:      l,
		R:        3,
		MaxSpeed: ms,
		MaxForce: mf,
	}
}

func (v *Vehicle) Display() {
	theta := math.Atan2(v.Vel.Y, v.Vel.X) + f64.Deg2Rad(90)
	M := f64.Mat2{}
	M.Rotate(theta)

	p1 := f64.Vec2{0, -v.R * 4}
	p2 := f64.Vec2{-v.R, v.R * 4}
	p3 := f64.Vec2{v.R, v.R * 4}

	p1 = M.Transform(p1)
	p2 = M.Transform(p2)
	p3 = M.Transform(p3)

	p1 = p1.Add(v.Pos)
	p2 = p2.Add(v.Pos)
	p3 = p3.Add(v.Pos)

	col := color.RGBA{175, 175, 175, 255}
	sdlgfx.FilledTrigon(renderer, int(p1.X), int(p1.Y), int(p2.X), int(p2.Y), int(p3.X), int(p3.Y), col)
}

// Reynold's flow following algorithm
// http://www.red3d.com/cwr/steer/FlowFollow.html
func (v *Vehicle) Follow(flow *FlowField) {
	desired := flow.Lookup(v.Pos)
	desired = desired.Scale(v.MaxSpeed)

	steer := desired.Sub(v.Vel)
	if steer.Len() > v.MaxForce {
		steer = steer.Normalize()
		steer = steer.Scale(v.MaxForce)
	}
	v.ApplyForce(steer)
}

func (v *Vehicle) ApplyForce(force f64.Vec2) {
	v.Acc = v.Acc.Add(force)
}

func (v *Vehicle) Update() {
	v.Vel = v.Vel.Add(v.Acc)
	if v.Vel.Len() > v.MaxSpeed {
		v.Vel = v.Vel.Normalize()
		v.Vel = v.Vel.Scale(v.MaxSpeed)
	}
	v.Pos = v.Pos.Add(v.Vel)
	v.Acc = f64.Vec2{}
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
