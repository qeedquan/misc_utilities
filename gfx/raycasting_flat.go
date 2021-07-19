// based on http://lodev.org/cgtutor/raycasting.html
package main

import (
	"image"
	"image/color"
	"image/draw"
	"log"
	"math"
	"os"
	"runtime"

	"github.com/qeedquan/go-media/math/f64"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlimage/sdlcolor"
)

const (
	MAP_WIDTH  = 24
	MAP_HEIGHT = 24
)

var world = [MAP_WIDTH][MAP_HEIGHT]int{
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 4, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 4, 0, 0, 0, 0, 5, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 4, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 4, 0, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
}

var (
	pos   = f64.Vec2{22, 12}
	dir   = f64.Vec2{-1, 0}
	plane = f64.Vec2{0, 0.66}
	speed = 1.0
	rot   = math.Pi / 8

	window   *sdl.Window
	renderer *sdl.Renderer
	texture  *sdl.Texture
	canvas   *image.RGBA
)

func main() {
	runtime.LockOSThread()
	log.SetFlags(0)
	log.SetPrefix("")
	initSDL()

	for {
		event()
		blit()
	}
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func initSDL() {
	err := sdl.Init(sdl.INIT_VIDEO)
	ck(err)

	w, h := 512, 384
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, sdl.WINDOW_RESIZABLE)
	ck(err)

	resize(w, h)

	window.SetTitle("Raycasting Untextured Walls")
}

func resize(w, h int) {
	var err error

	if texture != nil {
		texture.Destroy()
	}

	texture, err = renderer.CreateTexture(sdl.PIXELFORMAT_ABGR8888, sdl.TEXTUREACCESS_STREAMING, w, h)
	ck(err)

	canvas = image.NewRGBA(image.Rect(0, 0, w, h))
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
			case sdl.K_UP:
				moveForward(dir.X, dir.Y, speed)
			case sdl.K_DOWN:
				moveForward(-dir.X, -dir.Y, speed)
			case sdl.K_LEFT:
				moveSide(rot)
			case sdl.K_RIGHT:
				moveSide(-rot)
			}
		case sdl.WindowEvent:
			switch ev.Event {
			case sdl.WINDOWEVENT_RESIZED:
				resize(int(ev.Data[0]), int(ev.Data[1]))
			}
		}
	}
}

func moveForward(dx, dy, speed float64) {
	if world[int(pos.X+dx*speed)][int(pos.Y)] == 0 {
		pos.X += dx * speed
	}

	if world[int(pos.X)][int(pos.Y+dy*speed)] == 0 {
		pos.Y += dy * speed
	}
}

func moveSide(rot float64) {
	dir = dir.Rotate(rot)
	plane = plane.Rotate(rot)
}

func blit() {
	renderer.SetDrawColor(sdlcolor.Black)
	renderer.Clear()
	draw.Draw(canvas, canvas.Bounds(), image.NewUniform(color.RGBA{112, 112, 112, 255}), image.ZP, draw.Src)
	blitWorld()
	texture.Update(nil, canvas.Pix, canvas.Stride)
	renderer.Copy(texture, nil, nil)
	renderer.Present()
}

func blitWorld() {
	w, h, err := renderer.OutputSize()
	ck(err)

	for x := 0; x < w; x++ {
		camera := f64.Vec2{2*float64(x)/float64(w) - 1, 0}
		raypos := pos
		raydir := dir.AddScale(plane, camera.X)
		map_ := image.Pt(int(raypos.X), int(raypos.Y))
		delta := f64.Vec2{
			math.Sqrt(1 + (raydir.Y*raydir.Y)/(raydir.X*raydir.X)),
			math.Sqrt(1 + (raydir.X*raydir.X)/(raydir.Y*raydir.Y)),
		}

		var step, sidedist f64.Vec2
		if raydir.X < 0 {
			step.X = -1
			sidedist.X = (raypos.X - float64(map_.X)) * delta.X
		} else {
			step.X = 1
			sidedist.X = (float64(map_.X) + 1 - raypos.X) * delta.X
		}

		if raydir.Y < 0 {
			step.Y = -1
			sidedist.Y = (raypos.Y - float64(map_.Y)) * delta.Y
		} else {
			step.Y = 1
			sidedist.Y = (float64(map_.Y) + 1 - raypos.Y) * delta.Y
		}

		side := 0
		for {
			if sidedist.X < sidedist.Y {
				sidedist.X += delta.X
				map_.X = int(float64(map_.X) + step.X)
				side = 0
			} else {
				sidedist.Y += delta.Y
				map_.Y = int(float64(map_.Y) + step.Y)
				side = 1
			}

			if world[map_.X][map_.Y] > 0 {
				break
			}
		}

		var perpdist float64
		if side == 0 {
			perpdist = (float64(map_.X) - raypos.X + (1-step.X)/2) / raydir.X
		} else {
			perpdist = (float64(map_.Y) - raypos.Y + (1-step.Y)/2) / raydir.Y
		}

		height := int(float64(h) / perpdist)
		start := -height/2 + h/2
		if start < 0 {
			start = 0
		}
		end := height/2 + h/2
		if end >= h {
			end = h - 1
		}

		var color sdl.Color
		switch world[map_.X][map_.Y] {
		case 1:
			color = sdlcolor.Red
		case 2:
			color = sdlcolor.Green
		case 3:
			color = sdlcolor.Blue
		case 4:
			color = sdlcolor.White
		default:
			color = sdlcolor.Yellow
		}

		if side == 1 {
			color = sdl.Color{color.R / 2, color.G / 2, color.B / 2, 255}
		}

		vline(x, 0, start, sdl.Color{55, 55, 55, 255})
		vline(x, start, end, color)
	}
}

func vline(x int, y0, y1 int, color sdl.Color) {
	if y0 > y1 {
		y0, y1 = y1, y0
	}

	for y := y0; y < y1; y++ {
		canvas.Set(x, y, color)
	}
}
