package main

import (
	"log"
	"math/rand"
	"runtime"

	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlimage/sdlcolor"
)

const (
	W = 1024
	H = 600
)

type Vec2 [2]float32

func (v Vec2) Add(p Vec2) Vec2 {
	return Vec2{v[0] + p[0], v[1] + p[1]}
}

func (v Vec2) Scale(s float32) Vec2 {
	return Vec2{v[0] * s, v[1] * s}
}

func main() {
	runtime.LockOSThread()

	err := sdl.Init(sdl.INIT_EVERYTHING)
	if err != nil {
		log.Fatalln(err)
	}

	defer sdl.Quit()

	window, renderer, err := sdl.CreateWindowAndRenderer(W, H, sdl.WINDOW_RESIZABLE)
	if err != nil {
		log.Fatalln(err)
	}
	texture, err := renderer.CreateTexture(sdl.PIXELFORMAT_ARGB8888, sdl.TEXTUREACCESS_STREAMING, W, H)
	if err != nil {
		log.Fatalln(err)
	}
	pixels := make([]uint32, W*H)
	window.SetTitle("Gasket")
	renderer.SetLogicalSize(W, H)

	v := []Vec2{
		{0.5, 0},
		{0, 1},
		{1, 1},
	}
	p := Vec2{0.5, 0.5}

loop:
	for {
		for {
			ev := sdl.PollEvent()
			if ev == nil {
				break
			}
			switch ev := ev.(type) {
			case sdl.QuitEvent:
				break loop
			case sdl.KeyDownEvent:
				switch ev.Sym {
				case sdl.K_ESCAPE:
					break loop
				}
			}
		}
		r, g, b := rand.Intn(256), rand.Intn(256), rand.Intn(256)

		n := rand.Intn(3)
		q := v[n].Add(p).Scale(.5)

		x := int(q[0] * W)
		y := int(q[1] * H)
		if y*W+x < len(pixels) {
			pixels[y*W+x] = 255<<24 | uint32(r)<<16 | uint32(g)<<8 | uint32(b)
		}
		p = q

		renderer.SetDrawColor(sdlcolor.Black)
		renderer.Clear()
		texture.Update(nil, pixels, W*4)
		renderer.Copy(texture, nil, nil)
		renderer.Present()
	}
}
