package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	"image/draw"
	"image/png"
	"log"
	"math/rand"
	"os"
	"time"
)

var (
	gridsz = flag.Int("g", 50, "grid size")
	sqsz   = flag.Int("s", 16, "square size")
	rule   = flag.Int("r", 30, "use rule")
	rnd    = flag.Bool("x", false, "randomize")
)

var pal = [8]color.RGBA{
	{255, 255, 255, 255},
	{147, 149, 152, 255},
	{149, 162, 181, 255},
	{185, 145, 165, 255},
	{129, 169, 189, 255},
	{23, 245, 100, 255},
	{100, 42, 24, 255},
	{34, 66, 100, 255},
}

func main() {
	log.SetFlags(0)
	log.SetPrefix("ca1d: ")

	rand.Seed(time.Now().UnixNano())

	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	c := NewCA(*gridsz, *rule)
	c.Reset(*rnd)

	s := *gridsz * *sqsz
	m := image.NewRGBA(image.Rect(0, 0, s, s))
	draw.Draw(m, m.Bounds(), image.NewUniform(color.Black), image.ZP, draw.Over)
	for y := 0; y < c.Size; y++ {
		for x := 0; x < c.Size; x++ {
			blit(m, x, y, pal[c.At(x, y)])
		}
	}

	f, err := os.Create(flag.Arg(0))
	ck(err)
	ck(png.Encode(f, m))
	ck(f.Close())
}

func blit(m *image.RGBA, x, y int, c color.Color) {
	t := *sqsz / 10
	r := image.Rect(x**sqsz+t, y**sqsz+t, (x+1)**sqsz, (y+1)**sqsz)
	draw.Draw(m, r, image.NewUniform(c), image.ZP, draw.Over)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: ca1d [options] file")
	flag.PrintDefaults()
	os.Exit(2)
}

type CA struct {
	Pix  [][]uint8
	Size int
	Rule int
}

func NewCA(size, rule int) *CA {
	p := make([][]uint8, size)
	for i := range p {
		p[i] = make([]uint8, size)
	}
	return &CA{
		Pix:  p,
		Size: size,
		Rule: rule,
	}
}

func (c *CA) Reset(rnd bool) {
	for x := 0; x < c.Size; x++ {
		c.Set(x, 0, 0)
	}
	if !rnd {
		c.Set(c.Size/2, 0, 1)
	} else {
		for x := 0; x < c.Size; x++ {
			if rand.Intn(1000) >= 500 {
				c.Set(x, 0, 1)
			}
		}
	}
	c.Unfold()
}

func (c *CA) At(x, y int) uint8 {
	if !(0 <= y && y < len(c.Pix)) {
		return 0
	}
	if !(0 <= x && x < len(c.Pix[y])) {
		return 0
	}
	return c.Pix[y][x]
}

func (c *CA) Set(x, y int, v uint8) {
	if !(0 <= y && y < len(c.Pix)) {
		return
	}
	if !(0 <= x && x < len(c.Pix[y])) {
		return
	}
	c.Pix[y][x] = v
}

func (c *CA) State(x, y int) uint8 {
	l := c.At(x-1, y)
	t := c.At(x, y)
	r := c.At(x+1, y)
	if l != 0 {
		l = 1
	}
	if t != 0 {
		t = 1
	}
	if r != 0 {
		r = 1
	}
	s := l<<2 | t<<1 | r
	if c.Rule&(1<<s) != 0 {
		return s
	}
	return 0
}

func (c *CA) Unfold() {
	for y := range c.Pix {
		for x := range c.Pix[y] {
			c.Set(x, y+1, c.State(x, y))
		}
	}
}
