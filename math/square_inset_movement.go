package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	"image/draw"
	"image/png"
	"log"
	"math"
	"os"
)

func main() {
	w := 800
	h := 800
	g := &Grid{
		TileWidth:  16,
		TileHeight: 16,
		BorderSize: 2,
		Palette: []color.RGBA{
			color.RGBA{0xa9, 0x2f, 0x41, 0xff},
			color.RGBA{0xe5, 0xdf, 0xc5, 0xff},
			color.RGBA{0xb4, 0x83, 0x75, 0xff},
			color.RGBA{0x91, 0xc7, 0xa9, 0xff},
			color.RGBA{0x6a, 0xbe, 0xdb, 0xff},
			color.RGBA{0xb7, 0xa8, 0x5c, 0xff},
			color.RGBA{0x43, 0x44, 0x2b, 0xff},
			color.RGBA{0x60, 0x76, 0x25, 0xff},
			color.RGBA{0x22, 0x33, 0x37, 0xff},
		},
	}
	flag.IntVar(&w, "width", w, "width")
	flag.IntVar(&h, "height", h, "height")
	flag.IntVar(&g.BorderSize, "border-size", g.BorderSize, "border size")
	flag.IntVar(&g.TileWidth, "tile-width", g.TileWidth, "tile width")
	flag.IntVar(&g.TileHeight, "tile-height", g.TileHeight, "tile height")
	flag.IntVar(&g.Num, "num", 0, "only tile up to num tiles")

	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	m := image.NewRGBA(image.Rect(0, 0, w, h))
	g.Render(m)
	f, err := os.Create(flag.Arg(0))
	ck(err)
	ck(png.Encode(f, m))
	ck(f.Close())
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: square_inset_movement [options] file")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

// next square that tiles the area
// we just cube the square to make it
// bigger than needed for irregular areas
func nsq(x int) int {
	n := (math.Sqrt(float64(x)+1) - 1) / 2
	n = math.Ceil(n)
	s := 2*int(n) + 1
	return s * s * s
}

type Grid struct {
	TileWidth  int
	TileHeight int
	BorderSize int
	Num        int
	Palette    []color.RGBA
}

func (g *Grid) Render(m draw.Image) {
	b := m.Bounds()
	w, h := b.Dx(), b.Dy()
	s := (w * h) / (g.TileWidth * g.TileHeight)
	l := nsq(s)
	if g.Num != 0 {
		l = g.Num
	}

	for i := 0; i < l; i++ {
		px, py := g.at(i)
		px += w / (2 * g.TileWidth)
		py += h / (2 * g.TileHeight)

		x := px * g.TileWidth
		y := py * g.TileHeight
		r := image.Rect(x, y, x+g.TileWidth, y+g.TileHeight)
		c := image.NewUniform(color.Black)
		draw.Draw(m, r, c, image.ZP, draw.Src)

		r = r.Inset(g.BorderSize)
		c = image.NewUniform(g.Palette[i%len(g.Palette)])
		draw.Draw(m, r, c, image.ZP, draw.Src)
	}
}

// move in a square inset like fashion centered at origin
func (g *Grid) at(idx int) (x, y int) {
	if idx == 0 {
		return
	}

	// when we move in a inset fashion, the motion touches the sides of the
	// the squares that is of area (2*n + 1)^2
	fsq := (math.Sqrt(float64(idx+1)) - 1) / 2
	sq := int(math.Ceil(fsq))

	// solve for the current square area and the previous area
	// then use that information to split up the side into 4 pieces and figure
	// out which side we are walking to figure out the coordinates
	start := (2*(sq-1) + 1) * (2*(sq-1) + 1)
	end := (2*sq + 1) * (2*sq + 1)
	loc := idx - start
	side := (end - start) / 4
	phase := loc / side
	sideloc := loc % side

	switch phase {
	case 0:
		x = sideloc - sq
		y = -sq
	case 1:
		x = sq
		y = sideloc - sq
	case 2:
		x = -sq
		y = sideloc - sq + 1
	case 3:
		x = sideloc - sq + 1
		y = sq
	}

	return
}
