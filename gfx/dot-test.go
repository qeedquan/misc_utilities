package main

import (
	"flag"
	"fmt"
	"log"
	"math"
	"os"
	"runtime"

	"github.com/qeedquan/go-media/math/ga"
	"github.com/qeedquan/go-media/math/ga/mat4"
	"github.com/qeedquan/go-media/math/ga/vec3"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlimage/sdlcolor"
)

type Line [2]ga.Vec3d

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
	A := mat4.Apply3(&M, l[0])
	B := mat4.Apply3(&M, l[1])
	renderer.SetDrawColor(sdlcolor.White)
	renderer.DrawLine(int(A.X), int(A.Y), int(B.X), int(B.Y))

	R := reflectX()
	mat4.Mul(&M, &M, &R)
	A = mat4.Apply3(&M, l[0])
	B = mat4.Apply3(&M, l[1])
	renderer.SetDrawColor(sdlcolor.White)
	renderer.DrawLine(int(A.X), int(A.Y), int(B.X), int(B.Y))
}

func objectSpace() ga.Mat4d {
	w, h, _ := renderer.OutputSize()
	return ga.Mat4d{
		{2 / float64(w), 0, 0, -1},
		{0, 2 / float64(h), 0, -1},
		{0, 0, 1, 0},
		{0, 0, 0, 1},
	}
}

func screenSpace() ga.Mat4d {
	w, h, _ := renderer.OutputSize()
	return ga.Mat4d{
		{float64(w) / 2, 0, 0, float64(w) / 2},
		{0, float64(h) / 2, 0, float64(h) / 2},
		{0, 0, 1, 0},
		{0, 0, 0, 1},
	}
}

func reflectX() ga.Mat4d {
	return ga.Mat4d{
		{-1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1},
	}
}

func addView(x, y int) {
	M := objectSpace()
	P := ga.Vec3d{float64(x), float64(y), 0}
	P = mat4.Apply3(&M, P)
	views = append(views, Line{
		los[1],
		P,
	})

	A := vec3.Normalize(vec3.Sub(P, los[1]))
	B := vec3.Normalize(vec3.Sub(los[1], los[0]))
	C := math.Acos(vec3.Dot(A, B))
	fmt.Println(180 - ga.Rad2Deg(C))
}
