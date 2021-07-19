// http://jamie-wong.com/2014/08/19/metaballs-and-marching-squares/
package main

import (
	"image"
	"image/color"
	"image/draw"
	"log"
	"math"
	"math/rand"
	"os"
	"runtime"
	"time"

	"github.com/qeedquan/go-media/image/chroma"
	"github.com/qeedquan/go-media/math/f64"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
	"github.com/qeedquan/go-media/sdl/sdlimage/sdlcolor"
	"github.com/qeedquan/go-media/sdl/sdlttf"
)

type Circle struct {
	Pos f64.Vec2
	Vel f64.Vec2
	Rad float64
	Col color.RGBA
}

var (
	window   *sdl.Window
	renderer *sdl.Renderer
	texture  *sdl.Texture
	canvas   *image.RGBA
	circles  []Circle

	debug     = false
	threshold = 5.0
	pixelSize = 2
)

func main() {
	runtime.LockOSThread()
	initSDL()
	reset()
	for {
		event()
		update()
		blit()
	}
}

func initSDL() {
	err := sdl.Init(sdl.INIT_VIDEO | sdl.INIT_TIMER)
	ck(err)

	err = sdlttf.Init()
	ck(err)

	sdl.SetHint(sdl.HINT_RENDER_SCALE_QUALITY, "best")
	w, h := 1280, 800
	wflag := sdl.WINDOW_RESIZABLE
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, wflag)
	ck(err)

	window.SetTitle("2D Metaballs")

	texture, err = renderer.CreateTexture(sdl.PIXELFORMAT_ABGR8888, sdl.TEXTUREACCESS_STREAMING, w, h)
	ck(err)

	canvas = image.NewRGBA(image.Rect(0, 0, w, h))
}

func reset() {
	rand.Seed(time.Now().UnixNano())

	circles = circles[:0]
	for i := 0; i < 10+rand.Intn(100); i++ {
		circles = append(circles, randCircle())
	}
}

func randCircle() Circle {
	w, h, _ := renderer.OutputSize()

	return Circle{
		Pos: f64.Vec2{
			rand.Float64() * float64(w),
			rand.Float64() * float64(h),
		},
		Vel: f64.Vec2{
			rand.Float64()*32 - 16,
			rand.Float64()*32 - 16,
		},
		Rad: rand.Float64()*40 + 40,
		Col: chroma.RandRGBA(),
	}
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
				reset()
			case sdl.K_LEFT:
				pixelSize -= 1
				if pixelSize <= 0 {
					pixelSize = 1
				}
			case sdl.K_RIGHT:
				pixelSize += 1
			case sdl.K_UP:
				threshold -= 1.0
			case sdl.K_DOWN:
				threshold += 1.0
			case sdl.K_1:
				circles = append(circles, randCircle())
			case sdl.K_2:
				if len(circles) > 0 {
					circles = circles[:len(circles)-1]
				}
			case sdl.K_BACKSPACE:
				debug = !debug
			}
		}
	}
}

func update() {
	w, h, _ := renderer.OutputSize()
	for i := range circles {
		c := &circles[i]
		c.Pos = c.Pos.Add(c.Vel)
		if c.Pos.X-c.Rad < 0 {
			c.Vel.X = math.Abs(c.Vel.X)
		}
		if c.Pos.X+c.Rad > float64(w) {
			c.Vel.X = -math.Abs(c.Vel.X)
		}
		if c.Pos.Y-c.Rad < 0 {
			c.Vel.Y = math.Abs(c.Vel.Y)
		}
		if c.Pos.Y+c.Rad > float64(h) {
			c.Vel.Y = -math.Abs(c.Vel.Y)
		}
	}
}

func blit() {
	renderer.SetDrawColor(sdlcolor.Black)
	renderer.Clear()
	blitClear(color.RGBA{100, 100, 100, 255})
	blitMetaballs()
	texture.Update(nil, canvas.Pix, canvas.Stride)
	renderer.Copy(texture, nil, nil)
	blitDebug()
	renderer.Present()
}

func blitClear(col color.RGBA) {
	r := canvas.Bounds()
	l := r.Dx() * r.Dy() * 4
	s := canvas.Pix[0:l:l]
	for y := r.Min.Y; y < r.Max.Y; y++ {
		for x := r.Min.X; x < r.Max.X; x++ {
			s[0] = col.R
			s[1] = col.G
			s[2] = col.B
			s[3] = col.A
			s = s[4:]
		}
	}
}

func blitDebug() {
	if !debug {
		return
	}
	for _, c := range circles {
		sdlgfx.Circle(renderer, int(c.Pos.X), int(c.Pos.Y), int(c.Rad), color.RGBA{34, 45, 56, 255})
	}
}

func blitMetaballs() {
	w, h, _ := renderer.OutputSize()
	for y := 0; y < h; y += pixelSize {
		for x := 0; x < w; x += pixelSize {
			var col color.RGBA
			dist := math.MaxFloat32
			sum := 0.0

			// figure out the distances of point to all circles
			// the color will be based on the closest circle
			// metaballs is defined as a sum of all the circles
			// the farther away it is, the less contribution
			// it has, and then threshold it to draw

			// x^2 + y^2 <= r^2
			// divide both sides by r^2 and flip denominator
			// r^2 / (x^2 + y^2) >= 1
			// for a point inside a circle, if we take the max
			// then it is a point in any circles in the region
			// but if we sum it all up, we get a metaball, compare
			// against threshold to see if we should draw it, the higher
			// threshold the more blocky it looks due to the fact the edges
			// passes the threshold less than the ones in the center
			for _, c := range circles {
				dx := float64(x) - c.Pos.X
				dy := float64(y) - c.Pos.Y
				d2 := dx*dx + dy*dy
				sum += c.Rad * c.Rad / d2

				if d2 < dist {
					dist = d2
					col = c.Col
				}
			}

			if sum > threshold {
				r := image.Rect(x, y, x+pixelSize, y+pixelSize)
				draw.Draw(canvas, r, image.NewUniform(col), image.ZP, draw.Over)
			}
		}
	}
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}
