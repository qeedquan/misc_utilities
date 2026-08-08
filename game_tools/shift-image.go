package main

import (
	"flag"
	"fmt"
	"image"
	"image/draw"
	"log"
	"os"
	"strconv"

	"github.com/qeedquan/go-media/image/imageutil"
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("shift-image: ")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 3 {
		usage()
	}

	xs, _ := strconv.Atoi(flag.Arg(0))
	ys, _ := strconv.Atoi(flag.Arg(1))
	m, err := imageutil.LoadRGBAFile(flag.Arg(2))
	ck(err)

	r := m.Bounds()
	p := image.NewRGBA(r)
	draw.Draw(p, p.Bounds(), m, image.Pt(xs, ys), draw.Src)

	name := flag.Arg(2)
	if flag.NArg() >= 4 {
		name = flag.Arg(3)
	}
	err = imageutil.WriteRGBAFile(name, p)
	ck(err)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: shift-image xshift yshift input [output]")
	flag.PrintDefaults()
	os.Exit(2)
}
