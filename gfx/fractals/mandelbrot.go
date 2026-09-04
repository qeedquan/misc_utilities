package main

import (
	"flag"
	"log"
	"math/cmplx"
	"runtime"

	"github.com/qeedquan/go-media/image/chroma"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlimage/sdlcolor"
)

const (
	W = 800
	H = 600
)

var (
	screen   *Display
	texture  *sdl.Texture
	pixels   []uint32
	center   = complex(-0.4, 0)
	view     = 1.1
	seqlen   = 100
	run      = true
	palette  []sdl.Color
	div      [][]int
	limit    = 2.0
	drawchan = make(chan int, H)
)

type Display struct {
	*sdl.Window
	*sdl.Renderer
}

func NewDisplay(w, h int, flags sdl.WindowFlags) *Display {
	window, renderer, err := sdl.CreateWindowAndRenderer(w, h, flags)
	ck(err)
	return &Display{window, renderer}
}

func main() {
	runtime.LockOSThread()

	flag.Parse()
	err := sdl.Init(sdl.INIT_EVERYTHING)
	ck(err)
	defer sdl.Quit()

	sdl.GLSetAttribute(sdl.GL_DOUBLEBUFFER, 1)
	screen = NewDisplay(W, H, sdl.WINDOW_OPENGL|sdl.WINDOW_RESIZABLE)
	ck(err)
	screen.SetLogicalSize(W, H)

	texture, err = screen.CreateTexture(sdl.PIXELFORMAT_ARGB8888, sdl.TEXTUREACCESS_STREAMING, W, H)
	ck(err)
	pixels = make([]uint32, W*H)

	screen.SetTitle("Mandelbrot Set")
	palette = make([]sdl.Color, seqlen)
	buf := make([]int, W*H)
	div = make([][]int, H)
	for i := range div {
		div[i] = buf[W*i:]
	}

	for i := range palette {
		hsv := chroma.HSV{0.55, 1, float64(i) / float64(len(palette)) / 0.1}
		palette[i] = sdl.ColorModel.Convert(hsv).(sdl.Color)
	}

	for run {
		event()
		draw()
	}
}

func event() {
	const step = 0.1
	for {
		ev := sdl.PollEvent()
		if ev == nil {
			break
		}
		switch e := ev.(type) {
		case sdl.QuitEvent:
			run = false
		case sdl.KeyDownEvent:
			switch e.Sym {
			case sdl.K_ESCAPE:
				run = false
			case sdl.K_LEFT:
				center -= complex(step, 0)
			case sdl.K_RIGHT:
				center += complex(step, 0)
			case sdl.K_DOWN:
				center -= complex(0, step)
			case sdl.K_UP:
				center += complex(0, step)
			case sdl.K_a:
				view += 0.1
			case sdl.K_z:
				view -= 0.1
			}
		}
	}
}

func draw() {
	screen.SetDrawColor(sdlcolor.Black)
	screen.Clear()

	for y := 0; y < H; y++ {
		go func(y int) {
			for x := 0; x < W; x++ {
				div[y][x] = divergent(center, x, y)
			}
			drawchan <- y
		}(y)
	}

	for y := 0; y < H; y++ {
		cy := <-drawchan
		for x := 0; x < W; x++ {
			i := div[cy][x]
			if i < 0 {
				pixels[cy*W+x] = 0xFF000000
			} else {
				pixels[cy*W+x] = c32(palette[i])
			}
		}
	}
	texture.Update(nil, pixels, W*4)
	screen.Copy(texture, nil, nil)
	screen.Present()
}

func divergent(c complex128, x, y int) int {
	z := coord(c, x, y)
	z0 := z
	for i := 0; i < seqlen; i++ {
		if cmplx.Abs(z) > limit {
			return i
		}
		z = z0 + z*z
	}
	return -1
}

func coord(c complex128, x, y int) complex128 {
	fx, fy := float64(x), float64(y)
	nr := (real(c) - view) + ((2 * (view * fx)) / W)
	ni := (imag(c) + view) - ((2 * (view * fy)) / H)
	return complex(nr, ni)
}

func c32(c sdl.Color) uint32 {
	return uint32(c.A)<<24 | uint32(c.R)<<16 | uint32(c.G)<<8 | uint32(c.B)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}
