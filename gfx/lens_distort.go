// http://paulbourke.net/miscellaneous/lens/
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
	"github.com/qeedquan/go-media/sdl/sdlgfx"
	"github.com/qeedquan/go-media/sdl/sdlimage/sdlcolor"
)

var (
	window   *sdl.Window
	renderer *sdl.Renderer
	texture  *sdl.Texture
	canvas   *image.RGBA
	mode     int
	alpha    f64.Vec2
)

func main() {
	runtime.LockOSThread()
	initSDL()
	reset()
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

func reset() {
	alpha = f64.Vec2{0.5, 0.5}
}

func initSDL() {
	err := sdl.Init(sdl.INIT_VIDEO | sdl.INIT_TIMER)
	ck(err)

	sdl.SetHint(sdl.HINT_RENDER_SCALE_QUALITY, "best")
	w, h := 1280, 800
	wflag := sdl.WINDOW_RESIZABLE
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, wflag)
	ck(err)

	window.SetTitle("Lens Distortion")

	texture, err = renderer.CreateTexture(sdl.PIXELFORMAT_ABGR8888, sdl.TEXTUREACCESS_STREAMING, w, h)
	ck(err)

	canvas = image.NewRGBA(image.Rect(0, 0, w, h))

	sdlgfx.SetFont(sdlgfx.Font10x20, 10, 20)
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
			case sdl.K_1:
				mode = 0
			case sdl.K_2:
				mode = 1
			case sdl.K_3:
				mode = 2
			case sdl.K_4:
				mode = 3
			case sdl.K_5:
				mode = 4
			case sdl.K_6:
				mode = 5
			case sdl.K_LEFT:
				alpha.X += 0.1
			case sdl.K_RIGHT:
				alpha.X += 0.1
			case sdl.K_UP:
				alpha.Y -= 0.1
			case sdl.K_DOWN:
				alpha.Y += 0.1
			case sdl.K_q:
				alpha.X -= 0.1
				alpha.Y -= 0.1
			case sdl.K_w:
				alpha.X += 0.1
				alpha.Y += 0.1
			case sdl.K_BACKSPACE:
				reset()
			}
		}
	}
}

func blit() {
	renderer.SetDrawColor(sdlcolor.Black)
	renderer.Clear()
	draw.Draw(canvas, canvas.Bounds(), image.NewUniform(color.RGBA{150, 150, 150, 255}), image.ZP, draw.Over)
	blitGrid()
	texture.Update(nil, canvas.Pix, canvas.Stride)
	renderer.Copy(texture, nil, nil)
	blitStatus()
	renderer.Present()
}

func blitStatus() {
	col := color.RGBA{255, 255, 255, 255}
	str := fmt.Sprintf("mode: %d", mode)
	sdlgfx.String(renderer, 0, 0, col, str)
	str = fmt.Sprintf("alpha: %v", alpha)
	sdlgfx.String(renderer, 0, 32, col, str)
}

func blitGrid() {
	w, h, _ := renderer.OutputSize()
	s := 20

	for y := 0; y < h; y += s {
		for x := 0; x < w; x += s {
			square(x, y, s, color.RGBA{0, 0, 0, 255})
		}
	}
}

func pixel(x, y int, col color.RGBA) {
	w, h, _ := renderer.OutputSize()

	p := mapin(x, y, w, h)
	var dp f64.Vec2

	switch mode {
	case 0: // identity
		dp = p
	case 1: // curvature of lens
		l := p.Len()
		dp = f64.Vec2{
			p.X * (1 - alpha.X*l),
			p.Y * (1 - alpha.Y*l),
		}
	case 2:
		dp = f64.Vec2{
			math.Asin(p.X) * alpha.X,
			math.Asin(p.Y) * alpha.Y,
		}
	case 3:
		dp = f64.Vec2{
			math.Sin(p.X),
			math.Sin(p.Y),
		}
	case 4:
		r := p.Len()
		a := math.Atan2(p.Y, p.X)
		dp = f64.Vec2{
			r * math.Cos(a*alpha.X),
			r * math.Sin(a*alpha.Y),
		}
	case 5:
		dp = f64.Vec2{
			1 / (1 + math.Exp(alpha.X*-p.X)),
			1 / (1 + math.Exp(alpha.Y*-p.Y)),
		}
	}

	x, y = mapout(dp, w, h)
	canvas.Set(x, y, col)
}

func square(x, y, s int, col color.RGBA) {
	for i := 0; i < s; i++ {
		pixel(x+i, y, col)
		pixel(x+i, y+s, col)
		pixel(x, y+i, col)
		pixel(x+s, y+i, col)
	}
}

func mapin(x, y, w, h int) f64.Vec2 {
	return f64.Vec2{
		(2*float64(x) - float64(w)) / float64(w),
		(2*float64(y) - float64(h)) / float64(h),
	}
}

func mapout(p f64.Vec2, w, h int) (x, y int) {
	np := f64.Vec2{
		(p.X + 1) * float64(w) / 2,
		(p.Y + 1) * float64(h) / 2,
	}
	return int(np.X + 0.5), int(np.Y + 0.5)
}
