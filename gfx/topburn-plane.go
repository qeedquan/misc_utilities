// plane algorithm based on https://casual-effects.com/gamejam/topburn/index.html
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
	"time"

	"github.com/qeedquan/go-media/math/f64"
	"github.com/qeedquan/go-media/sdl"
)

var (
	window   *sdl.Window
	renderer *sdl.Renderer
	texture  *sdl.Texture

	center = f64.Vec2{64, 64}
	zpace  = 4.0
	curvy  = 4.0
	paused bool
	ticker = time.NewTicker(16 * time.Millisecond)
	frame  = uint(0)
	canvas = image.NewRGBA(image.Rect(0, 0, 128, 128))
	stripe = Stripe{
		Dim: 1,
		Pal: []color.RGBA{
			{131, 118, 156, 255},
			{255, 204, 170, 255},
		},
	}
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
	err := sdl.Init(sdl.INIT_VIDEO)
	ck(err)

	w, h := 640, 640
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, sdl.WINDOW_RESIZABLE)
	ck(err)

	renderer.SetLogicalSize(w, h)
	window.SetTitle("Plane")

	cr := canvas.Bounds()
	texture, err = renderer.CreateTexture(sdl.PIXELFORMAT_ABGR8888, sdl.TEXTUREACCESS_STREAMING, cr.Dx(), cr.Dy())
	ck(err)
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
			case sdl.K_SPACE:
				paused = !paused
			case sdl.K_r:
				frame = 0
			case sdl.K_UP:
				zpace += 0.1
				fmt.Printf("zpace %.2f\n", zpace)
			case sdl.K_DOWN:
				zpace -= 0.1
				fmt.Printf("zpace %.2f\n", zpace)
			case sdl.K_LEFT:
				curvy -= 0.1
				fmt.Printf("curvy %.2f\n", curvy)
			case sdl.K_RIGHT:
				curvy += 0.1
				fmt.Printf("curvy %.2f\n", curvy)
			case sdl.K_1:
				center.X -= 1
				fmt.Printf("center %v\n", center)
			case sdl.K_2:
				center.X += 1
				fmt.Printf("center %v\n", center)
			case sdl.K_3:
				center.Y -= 1
				fmt.Printf("center %v\n", center)
			case sdl.K_4:
				center.Y += 1
				fmt.Printf("center %v\n", center)
			}
		}
	}
}

func blitPlane(t uint) {
	// the plane takes up half the screen
	// k is the number of scanlines to draw
	r := canvas.Bounds()
	k := float64(r.Dy() / 2)

	l := float64(t) / 16
	for y := 3; y < int(k); y++ {
		// the idea is that we stretch out the width of the texture as the scanline moves down
		// combined with a stretched blitter, we can simulate the effect of things becoming smaller
		// and larger as we move up and down the scanline respectively
		z := float64(y) / zpace
		sx := math.Pow(z, 1/curvy)*16 - l
		dx := float64(-y * 32)
		dw := k * 4 * z

		dx += center.X
		dy := y + int(center.Y)

		blitGrad(sx, 128, 1, dx, dw, dy, l)
	}
}

func blitGrad(sx, sw, sh, dx, dw float64, dy int, l float64) {
	for px := int(dx); px < int(dx+dw); px++ {
		a := f64.LinearRemap(float64(px), dx, dx+dw, 0, 1)
		tx := sx + a*(sw-sx)
		col := stripe.At(int(tx), 1)
		canvas.Set(px, dy, col)
	}
}

func blit() {
	renderer.Clear()
	draw.Draw(canvas, canvas.Bounds(), image.NewUniform(color.RGBA{27, 121, 159, 255}), image.ZP, draw.Over)
	blitPlane(frame)
	texture.Update(nil, canvas.Pix, canvas.Stride)
	renderer.Copy(texture, nil, nil)
	renderer.Present()
	if !paused {
		select {
		case <-ticker.C:
			frame++
		default:
		}
	}
}

type Stripe struct {
	Dim int
	Pal []color.RGBA
}

func (s Stripe) Bounds() image.Rectangle {
	return image.Rect(-math.MaxInt32, -math.MaxInt32, math.MaxInt32, math.MaxInt32)
}

func (s Stripe) At(x, y int) color.Color {
	if len(s.Pal) == 0 {
		return color.Black
	}
	i := (x / s.Dim) % len(s.Pal)
	if i < 0 {
		i += len(s.Pal)
	}
	return s.Pal[i]
}

func (s Stripe) ColorModel() color.Model {
	return color.RGBAModel
}
