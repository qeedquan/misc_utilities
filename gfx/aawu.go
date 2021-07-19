package main

import (
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

	"github.com/qeedquan/go-media/image/chroma"
	"github.com/qeedquan/go-media/math/f64"
	"github.com/qeedquan/go-media/sdl"
)

var (
	window    *sdl.Window
	renderer  *sdl.Renderer
	canvas    *image.RGBA
	texture   *sdl.Texture
	theta     float64
	ptsz            = 1
	ptcol           = color.RGBA{0, 0, 0, 255}
	aa              = true
	death     uint8 = 255
	deathnote uint8 = 1
	ticker          = time.NewTicker(16 * time.Millisecond)
)

func main() {
	runtime.LockOSThread()
	rand.Seed(time.Now().UnixNano())
	initsdl()
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

func initsdl() {
	err := sdl.Init(sdl.INIT_VIDEO)
	ck(err)

	w, h := 1024, 768
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, sdl.WINDOW_RESIZABLE)
	ck(err)

	window.SetTitle("Antialiased Shapes")

	resize(w, h)
}

func resize(w, h int) {
	var err error
	texture, err = renderer.CreateTexture(sdl.PIXELFORMAT_ARGB8888, sdl.TEXTUREACCESS_STREAMING, w, h)
	ck(err)

	canvas = image.NewRGBA(image.Rect(0, 0, w, h))
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
			case sdl.K_RIGHT:
				ptsz += 1
				fmt.Printf("point size: %v\n", ptsz)
			case sdl.K_LEFT:
				ptsz -= 1
				fmt.Printf("point size: %v\n", ptsz)
			case sdl.K_SPACE:
				aa = !aa
			case sdl.K_c:
				ptcol = chroma.RandRGBA()
			}
		}
	}
}

func update() {
	_, _, mb := sdl.GetMouseState()
	if mb != 0 {
		theta += 5
	}

	select {
	case <-ticker.C:
		if death == 0 || death == 255 {
			deathnote = -deathnote
		}
		death += deathnote
	default:
	}
}

func blitshapes() {
	mx, my, _ := sdl.GetMouseState()
	for t := theta; t < 360+theta; t += 15 {
		r := 100.0
		y, x := math.Sincos(f64.Deg2Rad(t))
		linewu(canvas, mx, my, mx+f64.Iround(x*r), my+f64.Iround(y*r), ptsz, ptcol)
	}

	circlewu(canvas, mx, my, 50, ptsz, ptcol)

	for i := 0; i < 2; i++ {
		off := 100 * (i + 1)
		ftcol := color.RGBA{127, 127, 127, 255}
		filledcirclewu(canvas, mx, my-off, 50, ptsz, ptcol, ftcol)
		circlewu(canvas, mx, my+off, 50, ptsz, ptcol)
		filledcirclewu(canvas, mx-off, my, 50, ptsz, ptcol, ftcol)
		circlewu(canvas, mx+off, my, 50, ptsz, ptcol)
	}

	dp := image.Rect(512, 512, 512+50, 512+50)
	draw.Draw(canvas, dp, image.NewUniform(color.RGBA{14, 25, 100, death}), image.ZP, draw.Over)

	dp = image.Rect(540, 512, 540+50, 512+50)
	draw.Draw(canvas, dp, image.NewUniform(color.RGBA{14, 25, 100, 255 - death}), image.ZP, draw.Over)
}

func blit() {
	draw.Draw(canvas, canvas.Bounds(), image.NewUniform(color.White), image.ZP, draw.Src)
	blitshapes()

	renderer.Clear()
	texture.Update(nil, canvas.Pix, canvas.Stride)
	renderer.Copy(texture, nil, nil)
	renderer.Present()
}

// https://en.wikipedia.org/wiki/Xiaolin_Wu%27s_line_algorithm
func linewu(m *image.RGBA, ix0, iy0, ix1, iy1, wd int, col color.RGBA) {
	x0, y0, x1, y1 := float64(ix0), float64(iy0), float64(ix1), float64(iy1)
	steep := math.Abs(y1-y0) > math.Abs(x1-x0)
	if steep {
		x0, y0 = y0, x0
		x1, y1 = y1, x1
	}
	if x0 > x1 {
		x0, x1 = x1, x0
		y0, y1 = y1, y0
	}

	dx := x1 - x0
	dy := y1 - y0
	gradient := 1.0
	if dx != 0 {
		gradient = dy / dx
	}

	xend := math.Round(x0)
	yend := y0 + gradient*(xend-x0)
	xgap := rfpart(x0 + 0.5)
	xpxl1 := xend
	ypxl1 := ipart(yend)
	if steep {
		plot(m, ypxl1, xpxl1, rfpart(yend)*xgap, wd, col)
		plot(m, ypxl1+1, xpxl1, fpart(yend)*xgap, wd, col)
	} else {
		plot(m, xpxl1, ypxl1, rfpart(yend)*xgap, wd, col)
		plot(m, xpxl1, ypxl1+1, fpart(yend)*xgap, wd, col)
	}
	intery := yend + gradient

	xend = math.Round(x1)
	yend = y1 + gradient*(xend-x1)
	xgap = fpart(x1 + 0.5)
	xpxl2 := xend
	ypxl2 := ipart(yend)
	if steep {
		plot(m, ypxl2, xpxl2, rfpart(yend)*xgap, wd, col)
		plot(m, ypxl2+1, xpxl2, fpart(yend)*xgap, wd, col)
	} else {
		plot(m, xpxl2, ypxl2, rfpart(yend)*xgap, wd, col)
		plot(m, xpxl2, ypxl2+1, fpart(yend)*xgap, wd, col)
	}

	if steep {
		for x := xpxl1 + 1; x < xpxl2; x++ {
			plot(m, ipart(intery), x, rfpart(intery), wd, col)
			plot(m, ipart(intery)+1, x, fpart(intery), wd, col)
			intery = intery + gradient
		}
	} else {
		for x := xpxl1 + 1; x < xpxl2; x++ {
			plot(m, x, ipart(intery), rfpart(intery), wd, col)
			plot(m, x, ipart(intery)+1, fpart(intery), wd, col)
			intery = intery + gradient
		}
	}
}

func plot(m *image.RGBA, x, y, c float64, wd int, cr color.RGBA) {
	if wd <= 0 {
		return
	}

	op := draw.Over
	if !aa {
		op = draw.Src
		c = 1
	}

	ix := int(x)
	iy := int(y)

	// in rgb space scaling by a constant uniformly increases/decreases brightness
	col := color.RGBA{
		uint8(float64(cr.R) * c),
		uint8(float64(cr.G) * c),
		uint8(float64(cr.B) * c),
		uint8(c * 255),
	}

	// aa works by blending the current color in framebuffer
	// with our brightness (which is alpha), if we want no aa
	// just overwrite the color value since we are monochrome color
	// we are doing color*brightness + (1-brightness)*background_color
	// behind the scenes
	dp := image.Rect(ix-wd/2, iy-wd/2, ix+wd/2+1, iy+wd/2+1)
	draw.Draw(m, dp, image.NewUniform(col), image.ZP, op)
}

func ipart(x float64) float64 {
	return math.Floor(x)
}

func fpart(x float64) float64 {
	return x - math.Floor(x)
}

func rfpart(x float64) float64 {
	return 1 - fpart(x)
}

func cplot(m *image.RGBA, x, y int, c float64, wd int, cr color.RGBA) {
	plot(m, float64(x), float64(y), 1-c, wd, cr)
	plot(m, float64(x), float64(y-1), c, wd, cr)
}

// https://stackoverflow.com/questions/37589165/drawing-an-antialiased-circle-as-described-by-xaolin-wu
func circlewu(m *image.RGBA, cx, cy, r, wd int, cr color.RGBA) {
	lastfade := 0.0
	y := r
	for x := 0; x < y; x++ {
		height := math.Sqrt(math.Max(float64(r*r-x*x), 0))
		fade := math.Ceil(height) - height
		if fade < lastfade {
			y--
		}
		lastfade = fade

		cplot(m, cx+x, cy+y, fade, wd, cr)
		cplot(m, cx+x, cy-y, 1-fade, wd, cr)

		cplot(m, cx-x, cy+y, fade, wd, cr)
		cplot(m, cx-x, cy-y, 1-fade, wd, cr)

		cplot(m, cx+y, cy+x, fade, wd, cr)
		cplot(m, cx+y, cy-x, 1-fade, wd, cr)

		cplot(m, cx-y, cy+x, fade, wd, cr)
		cplot(m, cx-y, cy-x, 1-fade, wd, cr)
	}
}

// for filled circled we draw the background without aa but the border with aa
func filledcirclewu(m *image.RGBA, cx, cy, r, wd int, fg, bg color.RGBA) {
	lastfade := 0.0
	y := r
	for x := 0; x < y; x++ {
		height := math.Sqrt(math.Max(float64(r*r-x*x), 0))
		fade := math.Ceil(height) - height
		if fade < lastfade {
			y--
		}
		lastfade = fade

		dp := image.Rect(cx-x, cy-y, cx+x, cy+y)
		draw.Draw(m, dp, image.NewUniform(bg), image.ZP, draw.Over)

		dp = image.Rect(cx-y, cy-x, cx+y+1, cy+x)
		draw.Draw(m, dp, image.NewUniform(bg), image.ZP, draw.Over)

		cplot(m, cx+x, cy+y, fade, wd, fg)
		cplot(m, cx+x, cy-y, 1-fade, wd, fg)

		cplot(m, cx-x, cy+y, fade, wd, fg)
		cplot(m, cx-x, cy-y, 1-fade, wd, fg)

		cplot(m, cx+y, cy+x, fade, wd, fg)
		cplot(m, cx+y, cy-x, 1-fade, wd, fg)

		cplot(m, cx-y, cy+x, fade, wd, fg)
		cplot(m, cx-y, cy-x, 1-fade, wd, fg)

	}
}
