// based on http://lodev.org/cgtutor/juliamandelbrot.html
package main

import (
	"image"
	"image/color"
	"image/draw"
	"log"
	"math"
	"math/rand"
	"os"
	"runtime"
	"time"

	"github.com/qeedquan/go-media/image/chroma"
	"github.com/qeedquan/go-media/math/f64"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
	"github.com/qeedquan/go-media/sdl/sdlimage/sdlcolor"
)

var (
	window   *sdl.Window
	renderer *sdl.Renderer
	texture  *sdl.Texture
	canvas   *image.RGBA
	fps      sdlgfx.FPSManager

	iterations = 128
	zoom       = 1.0
	move       = f64.Vec2{0, 0}
	coord      = complex(0.7, 0.27015)
	curTime    float64
	oldTime    float64
	frameTime  float64
)

func main() {
	runtime.LockOSThread()
	rand.Seed(time.Now().UnixNano())
	log.SetFlags(0)
	log.SetPrefix("")
	initSDL()

	for {
		event()
		blit()
		timer()
		fps.Delay()
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

	w, h := 512, 512
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, sdl.WINDOW_RESIZABLE)
	ck(err)

	resize(w, h)

	window.SetTitle("Julia Sets")

	fps.Init()
	fps.SetRate(60)
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
			case sdl.K_KP_PLUS, sdl.K_z:
				zoom *= math.Pow(1.5, frameTime)
			case sdl.K_KP_MINUS, sdl.K_x:
				zoom /= math.Pow(1.5, frameTime)
			case sdl.K_LEFT:
				move.X -= 0.3 * frameTime / zoom
			case sdl.K_RIGHT:
				move.X += 0.3 * frameTime / zoom
			case sdl.K_UP:
				move.Y -= 0.3 * frameTime / zoom
			case sdl.K_DOWN:
				move.Y += 0.3 * frameTime / zoom
			case sdl.K_KP_2, sdl.K_2:
				coord = complex(
					real(coord),
					imag(coord)+0.2*frameTime/zoom,
				)
			case sdl.K_KP_8, sdl.K_8:
				coord = complex(
					real(coord),
					imag(coord)-0.2*frameTime/zoom,
				)
			case sdl.K_KP_6, sdl.K_6:
				coord = complex(
					real(coord)+0.2*frameTime/zoom,
					imag(coord),
				)
			case sdl.K_KP_4, sdl.K_4:
				coord = complex(
					real(coord)-0.2*frameTime/zoom,
					imag(coord),
				)
			}
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
	blitJulia()
	texture.Update(nil, canvas.Pix, canvas.Stride)
	renderer.Copy(texture, nil, nil)
	renderer.Present()
}

func blitJulia() {
	w, h, err := renderer.OutputSize()
	ck(err)

	re := real(coord)
	im := imag(coord)
	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			newre := 1.5*float64(x-w/2)/(0.5*zoom*float64(w)) + move.X
			newim := float64(y-h/2)/(0.5*zoom*float64(h)) + move.Y

			i := 0
			for ; i < iterations; i++ {
				oldre := newre
				oldim := newim
				newre = oldre*oldre - oldim*oldim + re
				newim = 2*oldre*oldim + im
				if newre*newre+newim*newim > 4 {
					break
				}
			}

			color := chroma.HSV{float64(i%256*20) / 360.0, 1, 0}
			if i < iterations {
				color.V = 1
			}
			canvas.Set(x, y, color)
		}
	}
}

func timer() {
	oldTime = curTime
	curTime = float64(sdl.GetTicks()) / 1000.0
	frameTime = (curTime - oldTime)
}
