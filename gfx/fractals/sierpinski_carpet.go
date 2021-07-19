// based on http://lodev.org/cgtutor/sierpinski.html
package main

import (
	"fmt"
	"image"
	"image/color"
	"image/draw"
	"log"
	"math/rand"
	"os"
	"runtime"
	"time"

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

	iterations = 8
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

	w, h := 243*4, 243*4
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, sdl.WINDOW_RESIZABLE)
	ck(err)

	resize(w, h)

	window.SetTitle("Sierpinski Carpet")

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
			case sdl.K_LEFT:
				iterations--
			case sdl.K_RIGHT:
				iterations++
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
	w, h, err := renderer.OutputSize()
	ck(err)

	renderer.SetDrawColor(sdlcolor.White)
	renderer.Clear()
	draw.Draw(canvas, canvas.Bounds(), image.NewUniform(color.White), image.ZP, draw.Src)
	blitCarpet(1, iterations, 0, 0, float64(w-1), float64(h-1))
	texture.Update(nil, canvas.Pix, canvas.Stride)
	renderer.Copy(texture, nil, nil)
	renderer.Present()

	window.SetTitle(fmt.Sprintf("Sierpinski Carpet - %d", iterations))
}

func blitCarpet(n, m int, x1, y1, x2, y2 float64) {
	blitRect(int((2*x1+x2)/3.0), int((2*y1+y2)/3.0), int((x1+2*x2)/3.0)-1, int((y1+2*y2)/3.0)-1,
		f64.Vec4{rand.Float64(), rand.Float64(), rand.Float64(), 1})

	if n < m {
		blitCarpet(n+1, m, x1, y1, (2*x1+x2)/3.0, (2*y1+y2)/3.0)
		blitCarpet(n+1, m, (2*x1+x2)/3.0, y1, (x1+2*x2)/3.0, (2*y1+y2)/3.0)
		blitCarpet(n+1, m, (x1+2*x2)/3.0, y1, x2, (2*y1+y2)/3.0)
		blitCarpet(n+1, m, x1, (2*y1+y2)/3.0, (2*x1+x2)/3.0, (y1+2*y2)/3.0)
		blitCarpet(n+1, m, (x1+2*x2)/3.0, (2*y1+y2)/3.0, x2, (y1+2*y2)/3.0)
		blitCarpet(n+1, m, x1, (y1+2*y2)/3.0, (2*x1+x2)/3.0, y2)
		blitCarpet(n+1, m, (2*x1+x2)/3.0, (y1+2*y2)/3.0, (x1+2*x2)/3.0, y2)
		blitCarpet(n+1, m, (x1+2*x2)/3.0, (y1+2*y2)/3.0, x2, y2)
	}
}

func blitRect(x1, y1, x2, y2 int, c color.Color) {
	for x := x1; x <= x2; x++ {
		for y := y1; y <= y2; y++ {
			canvas.Set(x, y, c)
		}
	}
}
