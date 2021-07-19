package main

import (
	"flag"
	"fmt"
	"log"
	"math"
	"os"
	"runtime"

	"github.com/qeedquan/go-media/math/f64"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlimage/sdlcolor"
)

type Line [2]f64.Vec3

var (
	window   *sdl.Window
	renderer *sdl.Renderer
	los      Line
	views    []Line
)

var conf struct {
	width  int
	height int
	radius int
	aspect float64
}

func main() {
	runtime.LockOSThread()
	log.SetFlags(0)
	parseFlags()
	initSDL()
	loop()
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func parseFlags() {
	flag.IntVar(&conf.width, "width", 1024, "window width")
	flag.IntVar(&conf.height, "height", 768, "window height")
	flag.Parse()
	conf.radius = 10
	conf.aspect = float64(conf.width) / float64(conf.height)
}

func initSDL() {
	err := sdl.Init(sdl.INIT_VIDEO)
	ck(err)

	window, renderer, err = sdl.CreateWindowAndRenderer(conf.width, conf.height, sdl.WINDOW_RESIZABLE)
	ck(err)

	window.SetTitle("Dot Test")
}

func loop() {
	reset()
	for {
		event()
		blit()
	}
}

func reset() {
	los = Line{
		{0, -1, 0},
		{0, 1, 0},
	}
	views = views[:0]
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
				reset()
			}
		case sdl.MouseButtonDownEvent:
			addView(int(ev.X), int(ev.Y))
		}
	}
}

func blit() {
	renderer.SetDrawColor(sdl.Color{88, 88, 88, 255})
	renderer.Clear()
	los.Draw()
	for _, l := range views {
		l.Draw()
	}
	renderer.Present()
}

func (l Line) Draw() {
	M := screenSpace()
	A := M.Transform3(l[0])
	B := M.Transform3(l[1])
	renderer.SetDrawColor(sdlcolor.White)
	renderer.DrawLine(int(A.X), int(A.Y), int(B.X), int(B.Y))

	R := reflectX()
	M.Mul(&M, &R)
	A = M.Transform3(l[0])
	B = M.Transform3(l[1])
	renderer.SetDrawColor(sdlcolor.White)
	renderer.DrawLine(int(A.X), int(A.Y), int(B.X), int(B.Y))
}

func objectSpace() f64.Mat4 {
	w, h, _ := renderer.OutputSize()
	return f64.Mat4{
		{2 / float64(w), 0, 0, -1},
		{0, 2 / float64(h), 0, -1},
		{0, 0, 1, 0},
		{0, 0, 0, 1},
	}
}

func screenSpace() f64.Mat4 {
	w, h, _ := renderer.OutputSize()
	return f64.Mat4{
		{float64(w) / 2, 0, 0, float64(w) / 2},
		{0, float64(h) / 2, 0, float64(h) / 2},
		{0, 0, 1, 0},
		{0, 0, 0, 1},
	}
}

func reflectX() f64.Mat4 {
	return f64.Mat4{
		{-1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1},
	}
}

func addView(x, y int) {
	M := objectSpace()
	P := f64.Vec3{float64(x), float64(y), 0}
	P = M.Transform3(P)
	views = append(views, Line{
		los[1],
		P,
	})

	A := P.Sub(los[1]).Normalize()
	B := los[1].Sub(los[0]).Normalize()
	C := math.Acos(A.Dot(B))
	fmt.Println(180 - f64.Rad2Deg(C))
}
