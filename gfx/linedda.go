package main

import (
	"flag"
	"image"
	"image/color"
	"image/draw"
	"image/png"
	"log"
	"math/rand"
	"os"
	"time"
)

func linedda(m *image.RGBA, xa, ya, xb, yb int, col color.RGBA) {
	dx := xb - xa
	dy := yb - ya

	x := float64(xa)
	y := float64(ya)
	steps := 0
	if abs(dx) > abs(dy) {
		steps = abs(dx)
	} else {
		steps = abs(dy)
	}
	xi := float64(dx) / float64(steps)
	yi := float64(dy) / float64(steps)

	m.SetRGBA(round(x), round(y), col)
	for k := 0; k < steps; k++ {
		x += xi
		y += yi
		m.SetRGBA(round(x), round(y), col)
	}
}

func lineddathick(m *image.RGBA, xa, ya, xb, yb int, col color.RGBA) {
	linedda(m, xa, ya, xb, yb, col)
	linedda(m, xa+1, ya, xb+1, yb, col)
	linedda(m, xa, ya+1, xb, yb+1, col)
	linedda(m, xa+1, ya+1, xb+1, yb+1, col)
}

var (
	width  = flag.Int("w", 1024, "width")
	height = flag.Int("h", 1024, "height")
	nlines = flag.Int("n", 256, "number of lines")
)

func main() {
	flag.Parse()
	rand.Seed(time.Now().UnixNano())
	testlinedda(false)
	testlinedda(true)
}

func testlinedda(thick bool) {
	name := "line-dda.png"
	if thick {
		name = "line-dda-thick.png"
	}
	f, err := os.Create(name)
	ck(err)

	N := *nlines
	w, h := *width, *height
	m := image.NewRGBA(image.Rect(0, 0, w, h))
	draw.Draw(m, m.Bounds(), image.NewUniform(color.Black), image.ZP, draw.Over)
	for i := 0; i < N; i++ {
		if thick {
			lineddathick(m, rand.Int()%w, rand.Int()%h, rand.Int()%w, rand.Int()%h, randrgb())
		} else {
			linedda(m, rand.Int()%w, rand.Int()%h, rand.Int()%w, rand.Int()%h, randrgb())
		}
	}

	ck(png.Encode(f, m))
	ck(f.Close())
}

func abs(x int) int {
	if x < 0 {
		x = -x
	}
	return x
}

func round(x float64) int {
	return int(x + 0.5)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func randrgb() color.RGBA {
	return color.RGBA{
		uint8(rand.Int()),
		uint8(rand.Int()),
		uint8(rand.Int()),
		255,
	}
}
