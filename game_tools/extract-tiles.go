package main

import (
	"flag"
	"fmt"
	"image"
	"image/draw"
	"log"
	"math"
	"os"

	"github.com/qeedquan/go-media/image/imageutil"
	"github.com/qeedquan/go-media/math/mathutil"
)

var flags struct {
	TileSize   image.Point
	TileOffset image.Point
}

func main() {
	log.SetFlags(0)
	log.SetPrefix("extract-tiles: ")
	parseFlags()

	m, err := imageutil.LoadRGBAFile(flag.Arg(0))
	ck(err)

	p := extract(m, flags.TileSize, flags.TileOffset)
	err = imageutil.WriteRGBAFile(flag.Arg(1), p)
	ck(err)
}

func parseFlags() {
	var tileSize, tileOffset string
	flag.StringVar(&tileSize, "s", "32x32", "tile size")
	flag.StringVar(&tileOffset, "o", "0x0", "tile offset")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 2 {
		usage()
	}

	t := &flags.TileSize
	fmt.Sscanf(tileSize, "%vx%v", &t.X, &t.Y)

	o := &flags.TileOffset
	fmt.Sscanf(tileOffset, "%vx%v", &o.X, &o.Y)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: extract-tiles [options] input output")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func extract(m *image.RGBA, t, o image.Point) *image.RGBA {
	var u []image.Image
	var z []int
	r := m.Bounds()
	r = r.Add(o)
	c := 0
	for y := r.Min.Y; y < r.Max.Y; y += t.Y {
	loop:
		for x := r.Min.X; x < r.Max.X; x += t.X {
			v := m.SubImage(image.Rect(x, y, x+t.X, y+t.Y))
			for i := range u {
				if imageutil.IsTransparent(v) || imageutil.Equals(u[i], v, nil) {
					c++
					continue loop
				}
			}
			u = append(u, v)
			z = append(z, c)
			c++
		}
	}
	n := len(u) * t.X * t.Y
	s := int(math.Ceil(math.Sqrt(float64(n))))
	s = mathutil.Multiple(s, t.X)
	p := image.NewRGBA(image.Rect(0, 0, s, s))
	x, y, l := 0, 0, 0
	for i := range u {
		a := u[i].Bounds()
		r := image.Rect(x, y, x+a.Dx(), y+a.Dy())
		if s < r.Min.X || s < r.Max.X {
			r = image.Rect(0, y+l, a.Dx(), y+l+a.Dy())
			x = 0
			y = r.Min.Y
			l = 0
		}
		l = mathutil.Max(l, a.Dy())
		draw.Draw(p, r, u[i], a.Min, draw.Src)
		x = r.Max.X
		fmt.Printf("%d,", z[i])
		if i > 0 && i%16 == 0 {
			fmt.Println()
		}
	}
	fmt.Println()
	return p
}
