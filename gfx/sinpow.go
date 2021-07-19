package main

import (
	"fmt"
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

var (
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

	w, h := 800, 600
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, sdl.WINDOW_RESIZABLE)
	ck(err)

	resize(w, h)

	window.SetTitle("Sinpow")
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
	const S = 0.25
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
			case sdl.K_LEFT:
				exp -= S
			case sdl.K_RIGHT:
				exp += S
			}
			fmt.Println("exp:", exp)
		case sdl.WindowEvent:
			switch ev.Event {
			case sdl.WINDOWEVENT_RESIZED:
				resize(int(ev.Data[0]), int(ev.Data[1]))
			}
		}
	}
}

func blit() {
	renderer.SetDrawColor(sdlcolor.Black)
	renderer.Clear()
	draw.Draw(canvas, canvas.Bounds(), image.NewUniform(color.Black), image.ZP, draw.Src)
	blitCircle()
	texture.Update(nil, canvas.Pix, canvas.Stride)
	renderer.Copy(texture, nil, nil)
	renderer.Present()
}

var (
	exp = 1.0
)

func blitCircle() {
	w, h, err := renderer.OutputSize()
	ck(err)

	aspect := float64(w) / float64(h)

	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			p := f64.Vec2{
				float64(x-w/2) / aspect,
				float64(y - h/2),
			}

			s := f64.Clamp(math.Pow(math.Sin(p.Len()), exp)*255, 0, 255) + 1

			c := color.RGBA{
				uint8(s),
				uint8(s),
				uint8(s),
				255,
			}

			canvas.SetRGBA(x, y, c)
		}
	}
}
