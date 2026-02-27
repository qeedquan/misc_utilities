// https://www.johndcook.com/blog/2018/08/30/drawing-spirograph-curves-in-python/
package main

import (
	"fmt"
	"image/color"
	"log"
	"math"
	"math/cmplx"
	"os"
	"runtime"

	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
)

var (
	window   *sdl.Window
	renderer *sdl.Renderer

	r1     = 1.0 + 0i
	r2     = 52.0/96 + 0i
	r3     = 42.0/96 + 0i
	ncycle = 13.0
)

func main() {
	runtime.LockOSThread()
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
	err := sdl.Init(sdl.INIT_VIDEO | sdl.INIT_TIMER)
	ck(err)

	sdl.SetHint(sdl.HINT_RENDER_SCALE_QUALITY, "best")
	w, h := 1280, 800
	wflag := sdl.WINDOW_RESIZABLE
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, wflag)
	ck(err)

	window.SetTitle("Spirograph")
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
				ncycle -= 0.5
			case sdl.K_RIGHT:
				ncycle += 0.5
			case sdl.K_1:
				r1 += 0.05
			case sdl.K_2:
				r1 -= 0.05
			case sdl.K_q:
				r2 += 0.05
			case sdl.K_w:
				r2 -= 0.05
			case sdl.K_a:
				r3 += 0.05
			case sdl.K_s:
				r3 -= 0.05
			case sdl.K_SPACE:
				fmt.Printf("r1 %v\n", r1)
				fmt.Printf("r2 %v\n", r2)
				fmt.Printf("r3 %v\n", r3)
				fmt.Printf("ncycle %v\n", ncycle)
			}
		}
	}
}

func blit() {
	renderer.SetDrawColor(color.RGBA{200, 200, 200, 255})
	renderer.Clear()
	blitCurve(r1, r2, r3, ncycle)
	renderer.Present()
}

func blitCurve(r1, r2, r3 complex128, ncycle float64) {
	width, height, _ := renderer.OutputSize()
	n := ncycle * 2 * math.Pi
	s := 50000.0
	r := 200.0

	var lv complex128
	for i := 0.0; i < n; i += n / s {
		t := complex(i, 0)
		v := spiro(t, r1, r2, r3)
		lv = v
		if i > 0 {
			x1 := int(real(v)*r) + width/2
			y1 := int(imag(v)*r) + height/2

			x2 := int(real(lv)*r) + width/2
			y2 := int(imag(lv)*r) + height/2

			sdlgfx.ThickLine(renderer, x1, y1, x2, y2, 4, color.RGBA{0, 0, 100, 255})
		}
	}

	for i := 0.0; i < n; i += n / s {
		t := complex(i, 0)
		v := circle(t, r1)
		x := int(real(v)*r) + width/2
		y := int(imag(v)*r) + height/2
		renderer.SetDrawColor(color.RGBA{64, 25, 100, 255})
		renderer.DrawPoint(x, y)
	}
}

func circle(t, r complex128) complex128 {
	return r * cmplx.Exp(1i*t)
}

func spiro(t, r1, r2, r3 complex128) complex128 {
	return r3*cmplx.Exp(1i*t*(r1+r2)/r2) + (r1+r2)*cmplx.Exp(1i*t)
}
