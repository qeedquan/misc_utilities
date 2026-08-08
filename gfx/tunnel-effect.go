/*

https://lodev.org/cgtutor/tunnel.html

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

	"github.com/qeedquan/go-media/image/imageutil"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlimage/sdlcolor"
)

var (
	window    *sdl.Window
	renderer  *sdl.Renderer
	texture   *sdl.Texture
	canvas    *image.RGBA
	tunnel    *Tunnel
	running   bool
	paused    bool
	start     time.Time
	animation float64
	center    image.Point
)

var opt struct {
	width  int
	height int
	radius float64

	interval time.Duration
	step     float64

	texture *image.RGBA
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
	flag.Float64Var(&opt.radius, "radius", 32, "tunnel radius")
	flag.Float64Var(&opt.step, "step", 0.003, "animation step")
	flag.DurationVar(&opt.interval, "interval", 16*time.Millisecond, "animation interval")
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

	window.SetTitle("Tunnel Effect")

	tunnel = NewTunnel(opt.texture)
	center = image.Pt(opt.width/2, opt.height/2)
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
				center = image.Pt(int(ev.X), int(ev.Y))
				tunnel.Gen(opt.radius, opt.width, opt.height, center.X, center.Y)
			}
		case sdl.KeyDownEvent:
			switch ev.Sym {
			case sdl.K_ESCAPE:
				running = false
			case sdl.K_SPACE:
				paused = !paused
			case sdl.K_LEFT:
				opt.radius -= 5
				tunnel.Gen(opt.radius, opt.width, opt.height, center.X, center.Y)
			case sdl.K_RIGHT:
				opt.radius += 5
				tunnel.Gen(opt.radius, opt.width, opt.height, center.X, center.Y)
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
	if center.X >= w {
		center.X = w / 2
	}
	if center.Y >= h {
		center.Y = h / 2
	}

	canvas = image.NewRGBA(image.Rect(0, 0, opt.width, opt.height))
	tunnel.Gen(opt.radius, opt.width, opt.height, center.X, center.Y)
}

func update() {
	if time.Since(start) >= opt.interval {
		start = time.Now()
		if !paused {
			animation += opt.step
		}
	}
}

func blit() {
	renderer.SetDrawColor(sdlcolor.Black)
	renderer.Clear()
	tunnel.Blit(canvas)
	texture.Update(nil, canvas.Pix, canvas.Stride)
	renderer.Copy(texture, nil, nil)
	renderer.Present()
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options]")
	flag.PrintDefaults()
	os.Exit(2)
}

type Tunnel struct {
	Texture  *image.RGBA
	Distance [][]int
	Angle    [][]int
}

func NewTunnel(m *image.RGBA) *Tunnel {
	return &Tunnel{
		Texture: m,
	}
}

func (t *Tunnel) Gen(r float64, w, h, cx, cy int) {
	b := t.Texture.Bounds()
	tw := b.Dx()
	th := b.Dy()

	t.Distance = make([][]int, h)
	t.Angle = make([][]int, h)
	for i := 0; i < h; i++ {
		t.Distance[i] = make([]int, w)
		t.Angle[i] = make([]int, w)
	}
	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			px := float64(x) - float64(cx)
			py := float64(y) - float64(cy)
			d := int(r*float64(th)/math.Hypot(px, py)) % th
			a := int(0.5 * float64(tw) * math.Atan2(py, px) / math.Pi)

			t.Distance[y][x] = d
			t.Angle[y][x] = a
		}
	}
}

func (t *Tunnel) Blit(m *image.RGBA) {
	mb := m.Bounds()
	tb := t.Texture.Bounds()

	w := mb.Dx()
	h := mb.Dy()
	tw := tb.Dx()
	th := tb.Dy()

	sx := int(float64(tw) * 1.5 * animation)
	sy := int(float64(th) * 0.25 * animation)
	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			tx := uint(t.Distance[y][x]+sx) % uint(tw)
			ty := uint(t.Angle[y][x]+sy) % uint(th)

			c := t.Texture.RGBAAt(int(tx), int(ty))
			m.SetRGBA(x, y, c)
		}
	}
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
