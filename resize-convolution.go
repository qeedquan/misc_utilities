package main

import (
	"flag"
	"fmt"
	"image"
	"log"
	"os"
	"strconv"

	"github.com/qeedquan/go-media/image/imageutil"
	"github.com/qeedquan/go-media/image/resampler"
	"github.com/qeedquan/go-media/math/f64"
)

var (
	filter = flag.String("f", "blackman", "filter to use")
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("resize: ")

	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 4 {
		usage()
	}

	f := resampler.GetFilter(*filter)
	if f.Name == "" {
		log.Fatalf("unknown filter %q", *filter)
	}

	m, err := imageutil.LoadRGBAFile(flag.Arg(0))
	ck(err)

	w, err := strconv.Atoi(flag.Arg(2))
	ck(err)

	h, err := strconv.Atoi(flag.Arg(3))
	ck(err)

	if w <= 0 || h <= 0 {
		log.Fatalf("invalid dimension %dx%d!", w, h)
	}

	o := &resampler.Options{
		BoundaryOp:  resampler.BOUNDARY_CLAMP,
		Filter:      f,
		SampleRange: f64.Vec2{0, 1},
		FilterScale: f64.Vec2{1, 1},
		SourceOff:   f64.Vec2{0, 0},
	}
	p := image.NewRGBA(image.Rect(0, 0, w, h))
	resampler.ResizeImage(m, p, o)

	err = imageutil.WriteRGBAFile(flag.Arg(1), p)
	ck(err)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: resize input output width height")
	flag.PrintDefaults()
	fmt.Fprintf(os.Stderr, "\n")
	fmt.Fprintf(os.Stderr, "available filters:\n")
	for i, f := range resampler.Filters {
		fmt.Fprintf(os.Stderr, "%v ", f.Name)
		if i&3 == 3 {
			fmt.Fprintln(os.Stderr)
		}
	}
	fmt.Fprintln(os.Stderr)
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}
