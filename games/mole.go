// ported from plan9front mole
package main

import (
	"flag"
	"fmt"
	"image"
	xdraw "image/draw"
	"log"
	"math"
	"math/rand"
	"os"
	"runtime"
	"time"

	"github.com/qeedquan/go-media/sdl"
)

type Particle struct {
	x, y         float64
	vx, vy       float64
	ax, ay       float64
	prevx, prevy float64
	col          sdl.Color
}

type Display struct {
	*sdl.Window
	*sdl.Renderer
}

var (
	screen   *Display
	viewport sdl.Rect
	canvas   *image.RGBA
	texture  *sdl.Texture

	N       = 20
	refresh = 0

	size = 1
	dt   = 0.01
	xmin = -40.0
	xmax = 40.0
	ymin = -40.0
	ymax = 40.0
	v0   = 0.1

	A, B      []Particle
	prev, cur []Particle
)

func main() {
	runtime.LockOSThread()

	var fullscreen bool
	flag.Usage = usage
	flag.Float64Var(&v0, "v", v0, "maximum start velocity")
	flag.Float64Var(&xmin, "x", xmin, "left boundary")
	flag.Float64Var(&xmax, "X", xmax, "right boundary")
	flag.Float64Var(&ymin, "y", ymin, "top boundary")
	flag.Float64Var(&ymax, "Y", ymax, "bottom boundary")
	flag.Float64Var(&dt, "t", dt, "time step")
	flag.IntVar(&N, "N", N, "number of particles")
	flag.IntVar(&size, "s", size, "size of particle (number of pixels)")
	flag.BoolVar(&fullscreen, "fs", false, "fullscreen")
	flag.Parse()

	A = make([]Particle, N)
	B = make([]Particle, N)
	prev = A
	cur = B

	rand.Seed(time.Now().UnixNano())

	err := sdl.Init(sdl.INIT_VIDEO)
	ck(err)

	wflag := sdl.WINDOW_RESIZABLE
	if fullscreen {
		wflag |= sdl.WINDOW_FULLSCREEN_DESKTOP
	}
	width, height := 800, 600
	screen = NewDisplay(width, height, wflag)

	makeTexture(width, height)

	screen.SetDrawColor(White)
	screen.Clear()
	screen.Present()
	screen.SetTitle("Molecular Dynamics")
	sdl.ShowCursor(0)

	reset()
	for {
		sw, sh := screen.Size()
		viewport.W = int32(sw)
		viewport.H = int32(sh)

		for i := range cur {
			cur[i] = Particle{}
		}
		for n := 0; n < N; n++ {
			p := &prev[n]
			for m := 0; m < n; m++ {
				q := &prev[m]
				dx1 := math.Abs(p.x - q.x)
				dx2 := xmax - xmin - dx1
				dx := min(dx1, dx2)
				dy1 := math.Abs(p.y - q.y)
				dy2 := ymax - ymin - dy1
				dy := min(dy1, dy2)
				R := dx*dx + dy*dy
				if R >= 9 || R == 0 {
					continue
				}
				R = 1 / math.Sqrt(R)
				R2 := R * R
				R4 := R2 * R2
				R6 := R4 * R2
				R12 := R6 * R6
				F := 24 * (2*R12 - R6)
				if p.x < q.x {
					dx = -dx
				}
				if p.y < q.y {
					dy = -dy
				}
				if dx1 > dx2 {
					dx = -dx
				}
				if dy1 > dy2 {
					dy = -dy
				}
				dx *= F
				dy *= F
				cur[n].ax += dx
				cur[n].ay += dy
				cur[m].ax -= dx
				cur[m].ay -= dy
			}
		}

		for j := 0; j < N; j++ {
			p := &prev[j]
			q := &cur[j]
			q.x = 2*p.x - p.prevx + q.ax*dt*dt
			q.y = 2*p.y - p.prevy + q.ay*dt*dt
			q.vx = (q.x - p.prevx) / (2 * dt)
			q.vy = (q.y - p.prevy) / (2 * dt)
			q.prevx = p.x
			q.prevy = p.y
			if q.x > xmax {
				q.x -= xmax - xmin
				q.prevx -= xmax - xmin
			}
			if q.x < xmin {
				q.x += xmax - xmin
				q.prevx += xmax - xmin
			}
			if q.y > ymax {
				q.y -= ymax - ymin
				q.prevy -= ymax - ymin
			}
			if q.y < ymin {
				q.y += ymax - ymin
				q.prevy += ymax - ymin
			}
			q.col = p.col

			x := float64(viewport.W)*(q.x-xmin)/(xmax-xmin) + float64(viewport.X)
			y := float64(viewport.H)*(q.y-ymin)/(ymax-ymin) + float64(viewport.Y)
			draw(sdl.Rect{int32(x), int32(y), int32(size), int32(size)}, p.col)
		}

		prev, cur = cur, prev

		texture.Update(nil, canvas.Pix, canvas.Stride)
		screen.Copy(texture, nil, nil)
		screen.Present()

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
				case sdl.K_q, sdl.K_ESCAPE:
					os.Exit(0)
				case sdl.K_r:
					reset()
				case sdl.K_f:
					draw(viewport, White)
				}
			case sdl.WindowEvent:
				switch ev.Event {
				case sdl.WINDOWEVENT_EXPOSED:
					draw(viewport, White)

				case sdl.WINDOWEVENT_RESIZED:
					sw, sh := screen.Size()
					viewport.W, viewport.H = int32(sw), int32(sh)
					makeTexture(sw, sh)
					draw(viewport, White)
				}
			}
		}
	}
}

func NewDisplay(w, h int, flags sdl.WindowFlags) *Display {
	window, renderer, err := sdl.CreateWindowAndRenderer(w, h, flags)
	ck(err)
	return &Display{window, renderer}
}

func reset() {
	grid := int(math.Sqrt(float64(N)) + 0.5)
	draw(viewport, White)
	for j := 0; j < N; j++ {
		p := &prev[j]
		p.x = 2*float64(j%grid) + rand.Float64()/2
		p.y = 2*float64(j/grid) + rand.Float64()/2
		p.vx = v0 * rand.Float64()
		p.vy = v0 * rand.Float64()
		p.prevx = p.x - p.vx*dt
		p.prevy = p.y - p.vy*dt
		p.col = colors[rand.Intn(len(colors))]
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: mole options")
	flag.PrintDefaults()
	os.Exit(1)
}

func draw(r sdl.Rect, c sdl.Color) {
	xdraw.Draw(canvas, r.Int(), image.NewUniform(c), image.ZP, xdraw.Over)
}

func ck(err error) {
	if err != nil {
		log.Fatal("mole:", err)
	}
}

func min(a, b float64) float64 {
	if a < b {
		return a
	}
	return b
}

func makeTexture(width, height int) {
	if texture != nil {
		texture.Destroy()
	}

	var err error
	texture, err = screen.CreateTexture(sdl.PIXELFORMAT_ARGB8888, sdl.TEXTUREACCESS_STREAMING, width, height)
	ck(err)

	canvas = image.NewRGBA(image.Rect(0, 0, width, height))
}

var (
	Black         = sdl.Color{0, 0, 0, 0xFF}
	White         = sdl.Color{0xFF, 0xFF, 0xFF, 0xFF}
	Gray          = sdl.Color{0xA0, 0xA0, 0xA0, 0xFF}
	Red           = sdl.Color{0xFF, 0, 0, 0xFF}
	Green         = sdl.Color{0, 0xFF, 0, 0xFF}
	Blue          = sdl.Color{0, 0, 0xFF, 0xFF}
	Cyan          = sdl.Color{0, 0xFF, 0xFF, 0xFF}
	Magenta       = sdl.Color{0xFF, 0, 0xFF, 0xFF}
	Yellow        = sdl.Color{0xFF, 0xFF, 0, 0xFF}
	PaleYellow    = sdl.Color{0xFF, 0xFF, 0xAA, 0xFF}
	DarkYellow    = sdl.Color{0xEE, 0xEE, 0x9E, 0xFF}
	PaleGreen     = sdl.Color{0xAA, 0xFF, 0xAA, 0xFF}
	DarkGreen     = sdl.Color{0x44, 0x88, 0x44, 0xFF}
	MedGreen      = sdl.Color{0x88, 0xCC, 0x88, 0xFF}
	DarkBlue      = sdl.Color{0x00, 0x00, 0x55, 0xFF}
	PaleBlueGreen = sdl.Color{0, 0, 0x55, 0xFF}
	PaleBlue      = sdl.Color{0, 0, 0xBB, 0xFF}
	BlueGreen     = sdl.Color{0, 0x88, 0x88, 0xFF}
	GreyGreen     = sdl.Color{0x55, 0xAA, 0xAA, 0xFF}
	PaleGreyGreen = sdl.Color{0x9E, 0xEE, 0xEE, 0xFF}
	YellowGreen   = sdl.Color{0x99, 0x99, 0x4C, 0xFF}
	MedBlue       = sdl.Color{0, 0, 0x99, 0xFF}
	GreyBlue      = sdl.Color{0, 0x5D, 0xBB, 0xFF}
	PaleGreyBlue  = sdl.Color{0x49, 0x93, 0xDD, 0xFF}
	PurpleBlue    = sdl.Color{0x88, 0x88, 0xCC, 0xFF}
	Pink          = sdl.Color{0xFF, 0xAA, 0x55, 0xFF}
	Lavender      = sdl.Color{0xFF, 0xAA, 0xFF, 0xFF}
	Maroon        = sdl.Color{0xBB, 0x00, 0x5D, 0xFF}
	PastelCyan    = sdl.Color{0xEA, 0xFF, 0xFF, 0xFF}
	PastelYellow  = sdl.Color{0xFF, 0xFF, 0xEA, 0xFF}
	PaleRed       = sdl.Color{0xD9, 0x4C, 0x38, 0xFF}
	CobaltViolet  = sdl.Color{0xA2, 0x52, 0x79, 0xFF}
	Crayola       = sdl.Color{0x96, 0x78, 0xB6, 0xFF}
	Purple        = sdl.Color{0x4B, 0x30, 0x8D, 0xFF}
	AliceBlue     = sdl.Color{240, 248, 255, 255}
	Lime          = sdl.Color{0, 255, 0, 255}
	Turquoise     = sdl.Color{64, 244, 208, 255}
)

var colors = []sdl.Color{
	Black,
	Red,
	Green,
	Blue,
	Cyan,
	Magenta,
	DarkYellow,
	DarkGreen,
	PaleGreen,
	MedGreen,
	DarkBlue,
	PaleBlueGreen,
	PaleBlue,
	BlueGreen,
	GreyGreen,
	PaleGreyGreen,
	YellowGreen,
	MedBlue,
	GreyBlue,
	PaleGreyBlue,
	PurpleBlue,
}
