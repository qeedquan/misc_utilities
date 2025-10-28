// cosine palettes, based on
// http://www.iquilezles.org/www/articles/palettes/palettes.htm
package main

import (
	"image"
	"log"
	"math"
	"os"
	"runtime"

	"github.com/qeedquan/go-media/image/chroma"
	"github.com/qeedquan/go-media/math/ga"
	"github.com/qeedquan/go-media/math/ga/vec3"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlimage/sdlcolor"
)

const (
	W = 800
	H = 600
)

var (
	window   *sdl.Window
	renderer *sdl.Renderer
	texture  *sdl.Texture
	pixels   *image.RGBA
)

func main() {
	runtime.LockOSThread()
	log.SetFlags(0)
	err := sdl.Init(sdl.INIT_VIDEO)
	ck(err)

	window, renderer, err = sdl.CreateWindowAndRenderer(W, H, sdl.WINDOW_RESIZABLE)
	ck(err)

	window.SetTitle("Cosine Palette")
	renderer.SetLogicalSize(W, H)

	texture, err = renderer.CreateTexture(sdl.PIXELFORMAT_ABGR8888, sdl.TEXTUREACCESS_STREAMING, W, H)
	ck(err)

	pixels = image.NewRGBA(image.Rect(0, 0, W, H))

	for {
		event()
		draw()
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
			}
		}
	}
}

func draw() {
	renderer.SetDrawColor(sdlcolor.Black)
	renderer.Clear()

	for y := 0; y < H; y++ {
		for x := 0; x < W; x++ {
			Y := float64(y) / float64(H)
			X := float64(x) / float64(W)

			A := ga.Vec3d{0.5, 0.5, 0.5}
			B := ga.Vec3d{0.5, 0.5, 0.5}
			C := ga.Vec3d{1.0, 1.0, 1.0}
			D := ga.Vec3d{0.0, 0.33, 0.67}

			if Y > (1.0 / 7.0) {
				A = ga.Vec3d{0.5, 0.5, 0.5}
				B = ga.Vec3d{0.5, 0.5, 0.5}
				C = ga.Vec3d{1.0, 1.0, 1.0}
				D = ga.Vec3d{0.0, 0.10, 0.20}
			}
			if Y > (2.0 / 7.0) {
				A = ga.Vec3d{0.5, 0.5, 0.5}
				B = ga.Vec3d{0.5, 0.5, 0.5}
				C = ga.Vec3d{1.0, 1.0, 1.0}
				D = ga.Vec3d{0.3, 0.20, 0.20}
			}
			if Y > (3.0 / 7.0) {
				A = ga.Vec3d{0.5, 0.5, 0.5}
				B = ga.Vec3d{0.5, 0.5, 0.5}
				C = ga.Vec3d{1.0, 1.0, 0.5}
				D = ga.Vec3d{0.8, 0.90, 0.30}
			}
			if Y > (4.0 / 7.0) {
				A = ga.Vec3d{0.5, 0.5, 0.5}
				B = ga.Vec3d{0.5, 0.5, 0.5}
				C = ga.Vec3d{1.0, 0.7, 0.4}
				D = ga.Vec3d{0.0, 0.15, 0.20}
			}
			if Y > (5.0 / 7.0) {
				A = ga.Vec3d{0.5, 0.5, 0.5}
				B = ga.Vec3d{0.5, 0.5, 0.5}
				C = ga.Vec3d{2.0, 1.0, 0.0}
				D = ga.Vec3d{0.5, 0.20, 0.25}
			}
			if Y > (6.0 / 7.0) {
				A = ga.Vec3d{0.8, 0.5, 0.4}
				B = ga.Vec3d{0.2, 0.4, 0.2}
				C = ga.Vec3d{2.0, 1.0, 1.0}
				D = ga.Vec3d{0.0, 0.25, 0.25}
			}

			P := pal(X, A, B, C, D)
			_, F := math.Modf(Y * 7)
			S := 0.5 + 0.5*math.Sqrt(4*F*(1-F))
			P = vec3.Scale(P, S)

			pixels.SetRGBA(x, y, chroma.Vec3ToRGBA(P))
		}
	}

	texture.Update(nil, pixels.Pix, W*4)
	renderer.Copy(texture, nil, nil)
	renderer.Present()
}

func pal(t float64, a, b, c, d ga.Vec3d) ga.Vec3d {
	P := 2 * math.Pi
	S := vec3.Cos(vec3.Scale(vec3.Add(vec3.Scale(c, t), d), P))
	R := vec3.Hadamard(b, S)
	T := vec3.Add(a, R)
	return T
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}
