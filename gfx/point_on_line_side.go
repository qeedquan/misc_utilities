// given a point determine if it is left or right of a line
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
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
)

var (
	window   *sdl.Window
	renderer *sdl.Renderer
	line     Line
	pause    bool
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

func initSDL() {
	err := sdl.Init(sdl.INIT_VIDEO)
	ck(err)

	window, renderer, err = sdl.CreateWindowAndRenderer(1024, 768, sdl.WINDOW_RESIZABLE)
	ck(err)

	window.SetTitle("Point on which side of the line")
}

func reset() {
	fps.SetRate(60)
	w, h, _ := renderer.OutputSize()
	line = Line{
		Origin:    ga.Vec2d{float64(w / 2), float64(h / 2)},
		Magnitude: 50,
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
			case sdl.K_h:
				line.Handedness = !line.Handedness
			case sdl.K_SPACE:
				pause = !pause
			case sdl.K_UP:
				line.Origin.Y -= 1
			case sdl.K_DOWN:
				line.Origin.Y += 1
			case sdl.K_LEFT:
				line.Origin.X -= 1.0
			case sdl.K_RIGHT:
				line.Origin.X += 1.0
			case sdl.K_q:
				line.Magnitude += 1.0
			case sdl.K_w:
				line.Magnitude -= 1.0
			}
		}
	}
}

func update() {
	line.Update()
	if pause {
		return
	}
	if line.Heading += 0.3; line.Heading >= 2*math.Pi {
		line.Heading -= 2 * math.Pi
	}
}

func blit() {
	renderer.SetDrawColor(color.RGBA{120, 120, 120, 255})
	renderer.Clear()
	blitSide()
	line.Blit()
	renderer.Present()
}

func blitSide() {
	w, h, _ := renderer.OutputSize()
	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			col := []color.RGBA{
				{151, 191, 213, 255},
				{240, 135, 213, 255},
				{145, 135, 132, 255},
			}
			sign := line.Side(float64(x), float64(y))
			renderer.SetDrawColor(col[1-sign])
			renderer.DrawPoint(x, y)
		}
	}
}

type Line struct {
	Heading    float64
	Magnitude  float64
	Origin     ga.Vec2d
	Handedness bool
	P1, P2     ga.Vec2d
}

func (l *Line) Update() {
	l.P1, l.P2 = l.XY()
}

func (l *Line) XY() (p1, p2 ga.Vec2d) {
	p1 = ga.Vec2d{-1, 0}
	p2 = ga.Vec2d{1, 0}

	R := mat2.Rotation(l.Heading)
	S := mat2.Scale(l.Magnitude, l.Magnitude)
	M := ga.Mat2d{}
	mat2.Mul(&M, &R, &S)

	p1 = mat2.Apply(&M, p1)
	p2 = mat2.Apply(&M, p2)

	p1 = vec2.Add(p1, l.Origin)
	p2 = vec2.Add(p2, l.Origin)

	return
}

func (l *Line) Blit() {
	p1, p2 := l.P1, l.P2
	renderer.SetDrawColor(color.RGBA{A: 255})
	renderer.DrawLine(int(p1.X), int(p1.Y), int(p2.X), int(p2.Y))
}

// https://stackoverflow.com/questions/1560492/how-to-tell-whether-a-point-is-to-the-right-or-left-side-of-a-line
// use determinant to figure out the side we're on
func (l *Line) Side(x, y float64) int {
	p1, p2 := l.P1, l.P2
	M := ga.Vec2d{x, y}
	var A, B ga.Vec2d
	if l.Handedness {
		A, B = p1, p2
	} else {
		A, B = p2, p1
	}
	sign := (B.X-A.X)*(M.Y-A.Y) - (B.Y-A.Y)*(M.X-A.X)
	if sign < 0 {
		return -1
	}
	if sign > 0 {
		return 1
	}
	return 0
}
