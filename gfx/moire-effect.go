/*

https://seancode.com/demofx/

*/

package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	"log"
	"math"
	"math/rand"
	"os"
	"runtime"
	"time"

	"github.com/qeedquan/go-media/sdl"
)

var (
	window    *sdl.Window
	renderer  *sdl.Renderer
	texture   *sdl.Texture
	canvas    *image.RGBA
	running   bool
	paused    bool
	start     time.Time
	animation float64
	moire     Moire
)

var opt struct {
	width    int
	height   int
	interval time.Duration
	step     float64
	iters    int
}

func main() {
	runtime.LockOSThread()
	rand.Seed(time.Now().UnixNano())
	parseflags()
	initsdl()
	reset()
	for running {
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

func parseflags() {
	flag.IntVar(&opt.width, "width", 1024, "window width")
	flag.IntVar(&opt.height, "height", 768, "window height")
	flag.Float64Var(&moire.Radius, "radius", 16, "ring width")
	flag.Float64Var(&opt.step, "step", 0.002, "animation step")
	flag.DurationVar(&opt.interval, "interval", 16*time.Millisecond, "animation interval")
	flag.Usage = usage
	flag.Parse()
}

func initsdl() {
	err := sdl.Init(sdl.INIT_VIDEO | sdl.INIT_TIMER)
	ck(err)

	wflags := sdl.WINDOW_RESIZABLE
	window, renderer, err = sdl.CreateWindowAndRenderer(opt.width, opt.height, wflags)
	ck(err)

	window.SetTitle("Moire Effect")

	resize(opt.width, opt.height)
}

func reset() {
	start = time.Now()
	running = true
	paused = false
	animation = rand.Float64() * 2 * math.Pi
}

func event() {
	for {
		ev := sdl.PollEvent()
		if ev == nil {
			break
		}
		switch ev := ev.(type) {
		case sdl.QuitEvent:
			running = false
		case sdl.WindowEvent:
			switch ev.Event {
			case sdl.WINDOWEVENT_RESIZED:
				resize(int(ev.Data[0]), int(ev.Data[1]))
			}
		case sdl.KeyDownEvent:
			switch ev.Sym {
			case sdl.K_ESCAPE:
				running = false
			case sdl.K_SPACE:
				paused = !paused
			case sdl.K_RETURN:
				reset()
			case sdl.K_LEFT:
				moire.Radius -= 1
			case sdl.K_RIGHT:
				moire.Radius += 1
			}
		}
	}
}

func resize(w, h int) {
	if texture != nil {
		texture.Destroy()
	}

	var err error
	texture, err = renderer.CreateTexture(sdl.PIXELFORMAT_ABGR8888, sdl.TEXTUREACCESS_STREAMING, w, h)
	ck(err)

	opt.width, opt.height = w, h
	canvas = image.NewRGBA(image.Rect(0, 0, opt.width, opt.height))
}

func update() {
	if time.Since(start) >= opt.interval {
		start = time.Now()
		if !paused {
			animation += opt.step

			w := float64(opt.width / 2)
			h := float64(opt.height / 2)

			x1, y1 := lissajous(w, h, 3, 4, w, h, math.Pi/4, animation)
			x2, y2 := lissajous(w, h, 5, 4, w, h, math.Pi/4, 1.05*animation)

			moire.C1 = image.Pt(int(x1+0.5), int(y1+0.5))
			moire.C2 = image.Pt(int(x2+0.5), int(y2+0.5))
		}
	}
}

func lissajous(x0, y0, a, b, A, B, d, t float64) (x, y float64) {
	x = A*math.Sin(a*t+d) + x0
	y = B*math.Sin(b*t) + y0
	return
}

func blit() {
	renderer.SetDrawColor(color.RGBA{0, 0, 0, 255})
	renderer.Clear()
	moire.Blit(canvas)
	texture.Update(nil, canvas.Pix, canvas.Stride)
	renderer.Copy(texture, nil, nil)
	renderer.Present()
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options]")
	flag.PrintDefaults()
	os.Exit(2)
}

type Moire struct {
	C1, C2 image.Point
	Radius float64
}

func (c *Moire) Blit(m *image.RGBA) {
	r := m.Bounds()
	w := r.Dx()
	h := r.Dy()
	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			p := image.Pt(x, y)
			u := idist(p, c.C1)
			v := idist(p, c.C2)
			b := int(float64(u^v) / c.Radius)

			col := color.RGBA{50, 50, 50, 255}
			if b&0x1 != 0 {
				col = color.RGBA{200, 200, 200, 255}
			}

			m.SetRGBA(x, y, col)
		}
	}
}

func idist(p, q image.Point) int {
	dx := float64(p.X - q.X)
	dy := float64(p.Y - q.Y)
	return int(math.Hypot(dx, dy))
}
