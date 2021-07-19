package main

import (
	"fmt"
	"image"
	"log"
	"os"
	"runtime"
	"sort"

	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
)

var (
	window   *sdl.Window
	renderer *sdl.Renderer
	seed     image.Rectangle
	rects    []image.Rectangle
	ridx     int
)

func main() {
	runtime.LockOSThread()
	derivative()
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

// derivative of the area is a polynomial
// of a square function
func derivative() {
	r := image.Rect(0, 0, 1, 1)
	b := 0.0
	for i := 0; i < 256; i++ {
		s := mrc(r, i)
		a := area(s) - area(r)
		fmt.Println(a, a-b)
		b = a
	}
}

func initSDL() {
	err := sdl.Init(sdl.INIT_VIDEO | sdl.INIT_TIMER)
	ck(err)

	w, h := calcDPS()
	wflag := sdl.WINDOW_RESIZABLE
	w, h = w*5/6, h*5/6
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, wflag)

	if w >= 400 && h >= 300 {
		sdlgfx.SetFont(sdlgfx.Font10x20, 10, 20)
	}
	window.SetTitle("Rectangle Sizing")

	const O = 100
	rects = append(rects, image.Rect(w/2-O, h/2-O, w/2+O, h/2+O))
	seed = rects[0]

	// we split it into positive and negative halves
	// since the points of the rectangle min and max
	// gets added/subtracted based on if we want to enlarge
	// or shrink, however, this can be unified into just
	// the positive branch and then canonicalizing the
	// rectangle at the end
	const N = 300
	if false {
		for i := 0; i <= N; i++ {
			rects = append(rects, mrp(rects[0], i))
		}

		for i := 0; i <= N/2; i++ {
			rects = append(rects, mrn(rects[0], i))
		}
	}

	// unified way
	for i := -N / 2; i <= N; i++ {
		rects = append(rects, mrc(rects[0], i))
	}
	sort.Slice(rects, func(i, j int) bool {
		return rects[i].Max.Y < rects[j].Max.Y
	})

	// we need to remove the dups as one contract to a point
	// where it starts expanding again
	// it could generate the same rectangles again
	var nodup []image.Rectangle
loop:
	for i := range rects {
		for j := range nodup {
			if nodup[j] == rects[i] {
				continue loop
			}
		}
		nodup = append(nodup, rects[i])
	}
	rects = nodup
}

func calcDPS() (w, h int) {
	dm, err := sdl.GetDisplayMode(0, 0)
	ck(err)
	return dm.W, dm.H
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
		case sdl.MouseMotionEvent:
			ridx = -1
			for i, r := range rects {
				if onrect(r, int(ev.X), int(ev.Y)) {
					ridx = i
				}
			}
		case sdl.KeyDownEvent:
			switch ev.Sym {
			case sdl.K_ESCAPE:
				os.Exit(0)
			}
		}
	}
}

func blit() {
	renderer.SetDrawColor(sdl.Color{4, 35, 39, 255})
	renderer.Clear()
	for i, r := range rects {
		s := sdl.Recti(r)
		renderer.SetDrawColor(sdl.Color{uint8(i * 40), uint8(i * 40), 100 + uint8(i*40), 255})
		renderer.DrawRect(&s)

		// need this because whenever we make a rectangle bigger/smaller by a constant amount
		// on all sides, the next line needs to be filled with n+1 pixels or else you
		// get a stipple on the diagonals
		s.W++
		renderer.DrawRect(&s)
	}

	// the limit of rect[i]/rect[i-1] should approach 1 since it is close to
	// x/(x+c) as c goes to infinity
	if ridx != -1 {
		text := fmt.Sprintf("%d %v", ridx, rects[ridx])
		if ridx > 0 {
			r1 := area(rects[ridx])
			r2 := area(rects[ridx-1])
			text = fmt.Sprintf("%d %v %v %f/%f = %f %f/%f = %f (%f)",
				ridx, rects[ridx], rects[ridx-1], r2, r1, r2/r1, area(seed), r1, area(seed)/r1, r1-r2)
		}
		sdlgfx.String(renderer, 0, 0, sdl.Color{255, 255, 255, 255}, text)
	}
	renderer.Present()
}

// one can think of it working as a line expanding or contracting
// if it is contracting, at some point it will go to 0 and then
// the math will force it to start expanding again
func mrc(r image.Rectangle, c int) image.Rectangle {
	return image.Rect(
		r.Min.X-c,
		r.Min.Y-c,
		r.Max.X+c,
		r.Max.Y+c,
	)
}

func mrp(r image.Rectangle, c int) image.Rectangle {
	return image.Rect(
		r.Min.X-c,
		r.Min.Y-c,
		r.Max.X+c,
		r.Max.Y+c,
	)
}

func mrn(r image.Rectangle, c int) image.Rectangle {
	return image.Rect(
		r.Min.X+c,
		r.Min.Y+c,
		r.Max.X-c,
		r.Max.Y-c,
	)
}

func onrect(r image.Rectangle, x, y int) bool {
	switch {
	case r.Min.X <= x && x <= r.Max.X && (y == r.Min.Y || y == r.Max.Y),
		r.Min.Y <= y && y <= r.Max.Y && (x == r.Min.X || x == r.Max.X):
		return true
	}
	return false
}

func area(r image.Rectangle) float64 {
	return float64(r.Dx() * r.Dy())
}
