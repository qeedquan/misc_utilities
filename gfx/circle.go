/*

Various algorithms to draw a circle

*/

package main

import (
	"image"
	"image/color"
	"image/draw"
	"image/png"
	"os"
)

func main() {
	b := image.Rect(0, 0, 512, 512)
	r := 224
	savecircle("casey_circle.png", b, r, caseycircle)
	savecircle("jesko_circle.png", b, r, jeskocircle)
	savecircle("bresenham_circle.png", b, r, bresenhamcircle)
}

func savecircle(name string, b image.Rectangle, r int, circle func(*image.RGBA, int, int, int, color.RGBA)) {
	f, err := os.Create(name)
	if err != nil {
		return
	}
	defer f.Close()

	m := image.NewRGBA(b)
	draw.Draw(m, b, image.NewUniform(color.Black), image.ZP, draw.Src)
	circle(m, b.Dx()/2, b.Dy()/2, r, color.RGBA{255, 255, 255, 255})
	png.Encode(f, m)
}

// https://www.computerenhance.com/p/efficient-dda-circle-outlines
func caseycircle(m *image.RGBA, cx, cy, r int, col color.RGBA) {
	r2 := r + r
	x := r
	y := 0
	dy := -2
	dx := r2 + r2 - 4
	d := r2 - 1
	for y <= x {
		m.Set(cx-x, cy-y, col)
		m.Set(cx+x, cy-y, col)
		m.Set(cx-x, cy+y, col)
		m.Set(cx+x, cy+y, col)
		m.Set(cx-y, cx-x, col)
		m.Set(cx+y, cx-x, col)
		m.Set(cx-y, cx+x, col)
		m.Set(cx+y, cx+x, col)

		d += dy
		dy -= 4
		y += 1

		if d < 0 {
			d += dx
			dx -= 4
			x -= 1
		}
	}
}

// https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
func jeskocircle(m *image.RGBA, cx, cy, r int, col color.RGBA) {
	t1 := r / 16
	x := r
	y := 0
	for x >= y {
		m.Set(cx-x, cy-y, col)
		m.Set(cx+x, cy-y, col)
		m.Set(cx-x, cy+y, col)
		m.Set(cx+x, cy+y, col)
		m.Set(cx-y, cx-x, col)
		m.Set(cx+y, cx-x, col)
		m.Set(cx-y, cx+x, col)
		m.Set(cx+y, cx+x, col)

		y += 1
		t1 += y
		t2 := t1 - x
		if t2 >= 0 {
			t1 = t2
			x -= 1
		}
	}
}

// http://members.chello.at/easyfilter/bresenham.html
func bresenhamcircle(m *image.RGBA, cx, cy, r int, col color.RGBA) {
	x := -r
	y := 0
	err := 2 - 2*r
	for x < 0 {
		m.Set(cx-x, cy+y, col)
		m.Set(cx-y, cy-x, col)
		m.Set(cx+x, cy-y, col)
		m.Set(cx+y, cy+x, col)
		r = err
		if r <= y {
			y += 1
			err += 2*y + 1
		}
		if r > x || err > y {
			x += 1
			err += 2*x + 1
		}
	}
}
