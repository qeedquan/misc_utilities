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
	"math/rand"
	"os"
	"runtime"
	"time"

	"github.com/qeedquan/go-media/image/imageutil"
	"github.com/qeedquan/go-media/sdl"
)

var (
	window   *sdl.Window
	renderer *sdl.Renderer
	texture  *sdl.Texture
	canvas   *image.RGBA
	rain     *Rain
	running  bool
	paused   bool
	start    time.Time
)

var opt struct {
	width    int
	height   int
	interval time.Duration
	perturb  time.Duration
	texture  *image.RGBA
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
	flag.DurationVar(&opt.interval, "interval", 16*time.Millisecond, "animation interval")
	flag.DurationVar(&opt.perturb, "perturb", 500*time.Millisecond, "random perturbation interval")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() >= 1 {
		var err error
		opt.texture, err = imageutil.LoadRGBAFile(flag.Arg(0))
		ck(err)
	} else {
		opt.texture = genchecker(opt.width, opt.height, 32)
	}
}

func initsdl() {
	err := sdl.Init(sdl.INIT_VIDEO | sdl.INIT_TIMER)
	ck(err)

	wflags := sdl.WINDOW_RESIZABLE
	window, renderer, err = sdl.CreateWindowAndRenderer(opt.width, opt.height, wflags)
	ck(err)

	window.SetTitle("Rain Effect")

	rain = NewRain(opt.texture, opt.width, opt.height)
	resize(opt.width, opt.height)
}

func reset() {
	start = time.Now()
	running = true
	paused = false
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
		case sdl.MouseButtonDownEvent:
			switch ev.Button {
			case sdl.BUTTON_LEFT:
				rain.Perturb(int(ev.X), int(ev.Y))
			}
		case sdl.KeyDownEvent:
			switch ev.Sym {
			case sdl.K_ESCAPE:
				running = false
			case sdl.K_SPACE:
				paused = !paused
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
	rain.Resize(opt.width, opt.height)
}

func update() {
	if paused {
		return
	}

	dt := time.Since(start)
	t := opt.interval
	if t < opt.perturb {
		t = opt.perturb
	}

	if dt >= opt.interval {
		rain.Update()
	}
	if opt.perturb != 0 && dt >= opt.perturb {
		rain.Perturb(rand.Intn(rain.Width), rand.Intn(rain.Height))
	}
	if dt >= t {
		start = time.Now()
	}
}

func blit() {
	renderer.SetDrawColor(color.RGBA{0, 0, 0, 255})
	renderer.Clear()
	rain.Blit(canvas)
	texture.Update(nil, canvas.Pix, canvas.Stride)
	renderer.Copy(texture, nil, nil)
	renderer.Present()
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options]")
	flag.PrintDefaults()
	os.Exit(2)
}

type Rain struct {
	Texture *image.RGBA
	Dst     [][]int16
	Src     [][]int16
	Width   int
	Height  int
}

func NewRain(m *image.RGBA, w, h int) *Rain {
	r := &Rain{
		Texture: m,
	}
	r.Resize(w, h)
	return r
}

func (r *Rain) Resize(w, h int) {
	r.Width = w
	r.Height = h
	r.Dst = make([][]int16, h)
	r.Src = make([][]int16, h)
	for i := 0; i < h; i++ {
		r.Dst[i] = make([]int16, w)
		r.Src[i] = make([]int16, w)
	}
}

func (r *Rain) Blit(m *image.RGBA) {
	b := r.Texture.Bounds()
	tw := b.Dx()
	th := b.Dy()
	for y := 1; y < r.Height-1; y++ {
		for x := 1; x < r.Width-1; x++ {
			dx := int(r.Src[y][x-1] - r.Src[y][x+1])
			dy := int(r.Src[y-1][x] - r.Src[y+1][x])

			col := r.Texture.RGBAAt((x+dx)%tw, (y+dy)%th)
			cr := int(col.R) - dx
			cg := int(col.G) - dx
			cb := int(col.B) - dx
			cr = clamp(cr, 0, 255)
			cg = clamp(cg, 0, 255)
			cb = clamp(cb, 0, 255)

			m.SetRGBA(x, y, color.RGBA{uint8(cr), uint8(cg), uint8(cb), 255})
		}
	}
}

func (r *Rain) Update() {
	for y := 1; y < r.Height-1; y++ {
		for x := 1; x < r.Width-1; x++ {
			r.Dst[y][x] = ((r.Src[y-1][x] + r.Src[y+1][x] + r.Src[y][x+1] + r.Src[y][x-1]) >> 1) - r.Dst[y][x]
			r.Dst[y][x] -= (r.Dst[y][x] >> 5)
		}
	}
	r.Dst, r.Src = r.Src, r.Dst
}

func (r *Rain) Perturb(x, y int) {
	r.Src[y][x] = int16(rand.Intn(0x3000))
}

func genchecker(w, h, s int) *image.RGBA {
	m := image.NewRGBA(image.Rect(0, 0, w, h))
	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			xs := x / s
			ys := y / s
			col := color.RGBA{200, 200, 205, 255}
			if (xs^ys)&0x1 != 0 {
				col = color.RGBA{40, 40, 40, 255}
			}
			m.SetRGBA(x, y, col)
		}
	}
	return m
}

func clamp(x, a, b int) int {
	if x < a {
		x = a
	}
	if x > b {
		x = b
	}
	return x
}
