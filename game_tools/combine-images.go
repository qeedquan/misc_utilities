package main

import (
	"flag"
	"fmt"
	"image"
	"image/draw"
	"log"
	"os"
	"strconv"
	"strings"
	"text/scanner"

	"github.com/qeedquan/go-media/image/imageutil"
	"github.com/qeedquan/go-media/math/mathutil"
)

var (
	rule = flag.String("r", "", "specify ruleset")
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("combine-images: ")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 2 {
		usage()
	}

	var imgs []*image.RGBA
	args := flag.Args()
	for _, arg := range args[:len(args)-1] {
		img, err := imageutil.LoadRGBAFile(arg)
		ck(err)
		imgs = append(imgs, img)
	}
	out := combine(*rule, imgs)
	err := imageutil.WriteRGBAFile(args[len(args)-1], out)
	ck(err)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: combine-images [options] input ... output")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func combine(rule string, imgs []*image.RGBA) *image.RGBA {
	var (
		bounds     []image.Rectangle
		scan       scanner.Scanner
		pos        image.Point
		maxw, maxh int
		lineh      int
		num        int
	)
	scan.Init(strings.NewReader(rule))

	for i := 0; i < len(imgs); i++ {
		bound := imgs[i].Bounds()
		if num > 0 {
			num--
		} else {
			ch := scan.Next()
			if ch == ';' {
				pos.Y += lineh
				lineh = 0
			} else if ch == scanner.Int {
				text := scan.TokenText()
				num, _ = strconv.Atoi(text)
			}
		}

		bound = bound.Add(pos)
		maxw += bound.Dx()
		lineh = mathutil.Max(lineh, bound.Dy())
		pos.X += bound.Dx()
		bounds = append(bounds, bound)
	}

	for _, r := range bounds {
		maxh = mathutil.Max(maxh, r.Max.Y)
	}

	out := image.NewRGBA(image.Rect(0, 0, maxw, maxh))
	for i, img := range imgs {
		draw.Draw(out, bounds[i], img, image.ZP, draw.Src)
	}
	return out
}
