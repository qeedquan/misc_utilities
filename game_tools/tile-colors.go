package main

import (
	"flag"
	"fmt"
	"image"
	"log"
	"os"
	"strconv"

	"github.com/qeedquan/go-media/image/imageutil"
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 3 {
		usage()
	}

	xs, _ := strconv.Atoi(flag.Arg(0))
	ys, _ := strconv.Atoi(flag.Arg(1))
	m, err := imageutil.LoadRGBAFile(flag.Arg(2))
	ck(err)

	r := m.Bounds()
	i := 0
	for y := r.Min.Y; y < r.Max.Y; y += ys {
		for x := r.Min.X; x < r.Max.X; x += xs {
			p := m.SubImage(image.Rect(x, y, x+xs, y+ys)).(*image.RGBA)
			if imageutil.IsTransparent(p) {
				continue
			}

			s := p.Bounds()
			fmt.Printf("region #%d\n", i)
			for yy := s.Min.Y; yy < s.Max.Y; yy++ {
				for xx := s.Min.X; xx < s.Max.X; xx++ {
					c := p.RGBAAt(xx, yy)
					fmt.Printf("[%d,%d] (%d,%d,%d,%d)\n", xx, yy, c.R, c.G, c.B, c.A)
				}
			}
			fmt.Println()
			i++
		}
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: tile-colors xs ys image")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}
