// suppress some gradients to some colors for an image
package main

import (
	"flag"
	"image"
	"image/color"
	"image/gif"
	"image/jpeg"
	"image/png"
	"log"
	"math"
	"os"
	"path/filepath"
	"strconv"
	"strings"
)

func main() {
	log.SetFlags(0)

	transparent := flag.Bool("transparent", false, "suppress the color by making it transparent")
	threshold := flag.Float64("threshold", 10, "threshold")
	flag.Parse()
	if flag.NArg() < 3 {
		log.Fatal("usage: color1 ... colorN <input> <output>")
	}

	var c []color.RGBA
	for i := 0; i < flag.NArg()-2; i++ {
		n, err := strconv.ParseInt(flag.Arg(i), 0, 64)
		ck(err)
		c = append(c, color.RGBA{uint8(n), uint8(n >> 8), uint8(n >> 16), 255})
	}

	f, err := os.Open(flag.Arg(flag.NArg() - 2))
	ck(err)
	defer f.Close()

	m, _, err := image.Decode(f)
	ck(err)

	r := m.Bounds()
	p := image.NewRGBA(r)

	for y := r.Min.Y; y < r.Max.Y; y++ {
		for x := r.Min.X; x < r.Max.X; x++ {
			w := m.At(x, y)
			p.Set(x, y, w)

			ar, ag, ab, _ := w.RGBA()
			br, bg, bb := uint8(ar>>8), uint8(ag>>8), uint8(ab>>8)
			for _, c := range c {
				xx := int(c.R) - int(br)
				yy := int(c.G) - int(bg)
				zz := int(c.B) - int(bb)
				diff := math.Sqrt(float64(xx*xx + yy*yy + zz*zz))
				if diff <= *threshold {
					if *transparent {
						p.Set(x, y, color.RGBA{})
					} else {
						p.Set(x, y, c)
					}
					break
				}
			}
		}
	}

	output := flag.Arg(flag.NArg() - 1)
	w, err := os.Create(output)
	ck(err)

	switch strings.ToLower(filepath.Ext(output)) {
	case ".jpeg", ".jpg":
		err = jpeg.Encode(w, p, nil)
	case ".gif":
		err = gif.Encode(w, p, &gif.Options{NumColors: 255})
	case ".png":
		fallthrough
	default:
		err = png.Encode(w, p)
	}
	ck(err)

	err = w.Close()
	ck(err)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}
