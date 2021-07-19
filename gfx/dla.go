// http://paulbourke.net/fractals/dla/
package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	"image/draw"
	"log"
	"math"
	"math/rand"
	"os"
	"runtime"
	"time"

	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlimage/sdlcolor"
)

var (
	conf struct {
		width  int
		height int
	}

	window   *sdl.Window
	renderer *sdl.Renderer
	texture  *sdl.Texture
	canvas   *image.RGBA
)

func main() {
	runtime.LockOSThread()
	log.SetPrefix("dla: ")
	log.SetFlags(0)
	rand.Seed(time.Now().UnixNano())
	parseFlags()
	initSDL()
	reset()
	for {
		event()
		update()
		blit()
	}
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func parseFlags() {
	conf.width = 640
	conf.height = 480

	flag.Usage = usage
	flag.IntVar(&conf.width, "w", conf.width, "window width")
	flag.IntVar(&conf.height, "h", conf.height, "window height")
	flag.Parse()
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options]")
	flag.PrintDefaults()
	os.Exit(2)
}

func initSDL() {
	err := sdl.Init(sdl.INIT_VIDEO)
	ck(err)

	sdl.SetHint(sdl.HINT_RENDER_SCALE_QUALITY, "best")

	w, h := conf.width, conf.height
	wflag := sdl.WINDOW_RESIZABLE
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, wflag)
	ck(err)

	window.SetTitle("Diffusion Limited Aggregation")
	renderer.SetLogicalSize(w, h)

	texture, err = renderer.CreateTexture(sdl.PIXELFORMAT_ABGR8888, sdl.TEXTUREACCESS_STREAMING, w, h)
	ck(err)

	canvas = image.NewRGBA(image.Rect(0, 0, w, h))
}

func reset() {
	draw.Draw(canvas, canvas.Bounds(), image.NewUniform(color.White), image.ZP, draw.Src)
	canvas.Set(conf.width/2, conf.height/2, color.Black)
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

func update() {
	w := conf.width
	h := conf.height
	fx := float64(w)
	fy := float64(h)

	theta := rand.Float64() * 2 * math.Pi
	ix := int(fx/2 + 0.5*(fx-2)*math.Cos(theta))
	iy := int(fy/2 + 0.5*(fy-2)*math.Sin(theta))
loop:
	for {
		nx := ix + rand.Intn(3) - 1
		if nx > 0 && nx < w-1 {
			ix = nx
		}
		ny := iy + rand.Intn(3) - 1
		if ny > 0 && ny < h-1 {
			iy = ny
		}

		for i := -1; i <= 1; i++ {
			for j := -1; j <= 1; j++ {
				c := canvas.At(ix+i, iy+j)
				r, _, _, _ := c.RGBA()
				if r < 128 {
					break loop
				}
			}
		}

		event()
	}
	canvas.Set(ix, iy, color.Black)
}

func blit() {
	renderer.SetDrawColor(sdlcolor.Black)
	renderer.Clear()
	texture.Update(nil, canvas.Pix, canvas.Stride)
	renderer.Copy(texture, nil, nil)
	renderer.Present()
}
