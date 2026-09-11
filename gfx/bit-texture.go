// http://lodev.org/cgtutor/xortexture.html
package main

import (
	"image"
	"image/color"
	"image/draw"
	"log"
	"os"
	"runtime"

	"github.com/qeedquan/go-media/image/chroma"
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

	window.SetTitle("Bit Texture")
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
				if op--; op < 0 {
					op = 0
				}
			case sdl.K_RIGHT:
				if op++; op > 14 {
					op = 14
				}
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
	blitBit()
	texture.Update(nil, canvas.Pix, canvas.Stride)
	renderer.Copy(texture, nil, nil)
	renderer.Present()
}

var (
	op int
)

func blitBit() {
	w, h, err := renderer.OutputSize()
	ck(err)

	const N = 32
	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			switch op {
			case 0:
				blitChroma(x, y, x^y)
			case 1:
				blitChroma(x, y, x|y)
			case 2:
				blitChroma(x, y, x&y)
			case 3:
				blitChroma(x, y, x&^y)
			case 4:
				blitChroma(x, y, ^x&^y)
			case 5:
				blitChroma(x, y, x)
			case 6:
				blitChroma(x, y, y)
			case 7:
				blitChroma(x, y, x+y)
			case 8:
				blitChroma(x, y, x-y)
			case 9:
				blitChroma(x, y, x*y)
			case 10:
				blitChroma(x, y, int(float64(x)/float64(y)*float64(w)*float64(h)))
			case 11:
				blitChroma(x, y, (x^y)&0x1f)
			case 12:
				blitMono(x, y, (x/N)&0x1)
			case 13:
				blitMono(x, y, (y/N)&0x1)
			case 14:
				blitMono(x, y, (x/N)&0x1^(y/N)&0x1)
			}
		}
	}
}

func blitChroma(x, y, v int) {
	canvas.Set(x, y, chroma.HSV{float64(uint8(v)) / 360, 1, 1})
}

func blitMono(x, y, v int) {
	c := color.RGBA{0, 0, 0, 255}
	if v != 0 {
		c = color.RGBA{255, 255, 255, 255}
	}
	canvas.Set(x, y, c)
}
