// http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
// http://blackpawn.com/texts/pointinpoly/
// https://fgiesen.wordpress.com/2013/02/10/optimizing-the-basic-rasterizer/
package main

import (
	"fmt"
	"image"
	"image/color"
	"image/draw"
	"log"
	"os"
	"runtime"

	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
)

var (
	window   *sdl.Window
	renderer *sdl.Renderer
	texture  *sdl.Texture
	canvas   *image.RGBA
	mode     int
	triangle = [3]image.Point{
		{150, 641},
		{356, 67},
		{56, 245},
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
	err := sdl.Init(sdl.INIT_VIDEO | sdl.INIT_TIMER)
	ck(err)

	sdl.SetHint(sdl.HINT_RENDER_SCALE_QUALITY, "best")
	w, h := 1280, 800
	wflag := sdl.WINDOW_RESIZABLE
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, wflag)
	ck(err)

	window.SetTitle("Fill Triangles")

	resizeWindow(w, h)
}

func resizeWindow(w, h int) {
	var err error
	texture, err = renderer.CreateTexture(sdl.PIXELFORMAT_ABGR8888, sdl.TEXTUREACCESS_STREAMING, w, h)
	ck(err)

	canvas = image.NewRGBA(image.Rect(0, 0, w, h))
}

func event() {
	modes := []string{
		"Scanline",
		"Bresenham",
		"Barycentric1",
		"Barycentric2",
	}
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
			case sdl.K_1:
				mode = 0
			case sdl.K_2:
				mode = 1
			case sdl.K_3:
				mode = 2
			case sdl.K_4:
				mode = 3
			}
			fmt.Println("Drawing Mode: ", modes[mode])
		case sdl.MouseButtonDownEvent:
			if 0 <= ev.Button-1 && int(ev.Button-1) < len(triangle) {
				triangle[ev.Button-1] = image.Pt(int(ev.X), int(ev.Y))
			}
		case sdl.WindowEvent:
			switch ev.Event {
			case sdl.WINDOWEVENT_RESIZED:
				w, h := int(ev.Data[0]), int(ev.Data[1])
				resizeWindow(w, h)
			}
		}
	}
}

func blit() {
	renderer.Clear()
	draw.Draw(canvas, canvas.Bounds(), image.NewUniform(color.RGBA{100, 100, 120, 255}), image.ZP, draw.Over)
	switch mode {
	case 0:
		blitScanline(triangle[0], triangle[1], triangle[2], color.RGBA{255, 255, 255, 255})
	case 1:
		blitBresenham(triangle[0], triangle[1], triangle[2], color.RGBA{255, 255, 255, 255})
	case 2:
		blitBarycentric1(triangle[0], triangle[1], triangle[2], color.RGBA{255, 255, 255, 255})
	case 3:
		blitBarycentric2(triangle[0], triangle[1], triangle[2], color.RGBA{255, 255, 255, 255})
	}
	texture.Update(nil, canvas.Pix, canvas.Stride)
	renderer.Copy(texture, nil, nil)
	pal := []color.RGBA{
		color.RGBA{255, 0, 0, 255},
		color.RGBA{0, 255, 0, 255},
		color.RGBA{0, 0, 255, 255},
	}
	for i, p := range triangle {
		sdlgfx.FilledCircle(renderer, p.X, p.Y, 10, pal[i])
	}
	renderer.Present()
}

// scanline algorithm:
// sort the triangle vertices in order of increasing y
// this makes it so that as we loop through we are moving down
// (more importantly, all edges are moving in the same direction if we do this)
// the next step is to split the triangle into two parts, top half and bottom half
// the top half and bottom half of the triangle will have a constant slope
// such that as dy increase by 1, dx/dy is the same for that portion
// so we can just increase the x coordinate by dx/dy and draw a horizontal line
// for them.
func blitScanline(a, b, c image.Point, col color.RGBA) {
	a, b, c = sortPointsByY(a, b, c)
	d := image.Pt(
		int(float64(a.X)+(float64(b.Y-a.Y)/float64(c.Y-a.Y))*float64(c.X-a.X)),
		b.Y,
	)
	blitScanlineBottom(a, b, d, col)
	blitScanlineTop(b, d, c, color.RGBA{255 - col.R, 255 - col.G, 255 - col.B, 255})
}

func blitScanlineBottom(a, b, c image.Point, col color.RGBA) {
	invslope1 := float64(b.X-a.X) / float64(b.Y-a.Y)
	invslope2 := float64(c.X-a.X) / float64(c.Y-a.Y)

	curx1 := float64(a.X)
	curx2 := float64(a.X)

	for scanlineY := a.Y; scanlineY <= b.Y; scanlineY++ {
		blitHline(int(curx1), int(curx2), scanlineY, col)
		curx1 += invslope1
		curx2 += invslope2
	}
}

func blitScanlineTop(a, b, c image.Point, col color.RGBA) {
	invslope1 := float64(c.X-a.X) / float64(c.Y-a.Y)
	invslope2 := float64(c.X-b.X) / float64(c.Y-b.Y)

	curx1 := float64(c.X)
	curx2 := float64(c.X)

	for scanlineY := c.Y; scanlineY > a.Y; scanlineY-- {
		blitHline(int(curx1), int(curx2), scanlineY, col)
		curx1 -= invslope1
		curx2 -= invslope2
	}
}

func blitHline(x0, x1, y int, col color.RGBA) {
	if x1 < x0 {
		x0, x1 = x1, x0
	}
	for i := x0; i <= x1; i++ {
		canvas.Set(i, y, col)
	}
}

// bresenham algorithm works the same as scanline where
// you split the triangle into top and bottom, but it is fancier because
// it uses bresenham line drawing to figure out the x coordinate boundary
// for each scanline rather than increasing by constant slope, but it
// gives the same answer
func blitBresenham(a, b, c image.Point, col color.RGBA) {
	a, b, c = sortPointsByY(a, b, c)
	a, b = blitBresenhamFill(a, b, a, c, col)
	blitBresenhamFill(a, c, b, c, color.RGBA{255 - col.R, 255 - col.G, 255 - col.B, 255})
}

func blitBresenhamFill(a, b, c, d image.Point, col color.RGBA) (p, q image.Point) {
	x0, y0 := a.X, a.Y
	x1, y1 := b.X, b.Y
	dx := abs(x1 - x0)
	dy := -abs(y1 - y0)
	sx, sy := -1, -1
	if x0 < x1 {
		sx = 1
	}
	if y0 < y1 {
		sy = 1
	}
	e := dx + dy

	x2, y2 := c.X, c.Y
	x3, y3 := d.X, d.Y
	ndx := abs(x3 - x2)
	ndy := -abs(y3 - y2)
	nsx, nsy := -1, -1
	if x2 < x3 {
		nsx = 1
	}
	if y2 < y3 {
		nsy = 1
	}
	ne := ndx + ndy

loop:
	for {
		blitHline(x0, x2, y0, col)

		for {
			if x0 == x1 && y0 == y1 {
				break loop
			}

			e2 := 2 * e
			if e2 >= dy {
				e += dy
				x0 += sx
			}
			if e2 <= dx {
				e += dx
				y0 += sy
				break
			}
		}

		for {
			if x2 == x3 && y2 == y3 {
				break loop
			}

			e2 := 2 * ne
			if e2 >= ndy {
				ne += ndy
				x2 += nsx
			}
			if e2 <= ndx {
				ne += ndx
				y2 += nsy
				break
			}
		}
	}

	p = image.Pt(x0, y0)
	q = image.Pt(x2, y2)
	return
}

// barycentric algorithm is where you find the bounding box
// of the triangle, loop through the box and do a point
// in triangle test, if it is inside, color it
// slowest algorithm but very elegant
func blitBarycentric1(a, b, c image.Point, col color.RGBA) {
	x0 := min(a.X, min(b.X, c.X))
	x1 := max(a.X, max(b.X, c.X))
	y0 := min(a.Y, min(b.Y, c.Y))
	y1 := max(a.Y, max(b.Y, c.Y))

	for y := y0; y <= y1; y++ {
		for x := x0; x <= x1; x++ {
			p := image.Pt(x, y)
			if insideTriangle(p, a, b, c) {
				canvas.Set(x, y, col)
			}
		}
	}
}

// optimized version of barycentric blitting, where instead
// of testing per pixel whether or not it is in a triangle
// we can walk directly in barycentric space (w0, w1, w2)
// and figure out using those coefficients whether or not
// we are inside the triangle or not
func blitBarycentric2(a, b, c image.Point, col color.RGBA) {
	x0 := min(a.X, min(b.X, c.X))
	x1 := max(a.X, max(b.X, c.X))
	y0 := min(a.Y, min(b.Y, c.Y))
	y1 := max(a.Y, max(b.Y, c.Y))

	A01 := a.Y - b.Y
	B01 := b.X - a.X
	A12 := b.Y - c.Y
	B12 := c.X - b.X
	A20 := c.Y - a.Y
	B20 := a.X - c.X

	p := image.Pt(x0, y0)
	w0r := sign(b, c, p)
	w1r := sign(c, a, p)
	w2r := sign(a, b, p)

	for y := y0; y <= y1; y++ {
		w0, w1, w2 := w0r, w1r, w2r
		for x := x0; x <= x1; x++ {
			s1, s2, s3 := w0 < 0, w1 < 0, w2 < 0
			if s1 == s2 && s2 == s3 {
				canvas.Set(x, y, col)
			}

			w0 += A12
			w1 += A20
			w2 += A01
		}
		w0r += B12
		w1r += B20
		w2r += B01
	}
}

func abs(x int) int {
	if x < 0 {
		return -x
	}
	return x
}

func min(a, b int) int {
	if a < b {
		return a
	}
	return b
}

func max(a, b int) int {
	if a > b {
		return a
	}
	return b
}

func sign(a, b, c image.Point) int {
	return (a.X-c.X)*(b.Y-c.Y) - (b.X-c.X)*(a.Y-c.Y)
}

func insideTriangle(p, a, b, c image.Point) bool {
	b1 := sign(p, a, b) < 0
	b2 := sign(p, b, c) < 0
	b3 := sign(p, c, a) < 0
	return b1 == b2 && b2 == b3
}

func sortPointsByY(a, b, c image.Point) (p, q, r image.Point) {
	if a.Y > b.Y {
		a, b = b, a
	}
	if a.Y > c.Y {
		a, c = c, a
	}
	if b.Y > c.Y {
		b, c = c, b
	}
	return a, b, c
}
