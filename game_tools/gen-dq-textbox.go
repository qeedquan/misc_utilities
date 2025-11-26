// old skool dw textbox:
// gen-dq-textbox dw.png

// old skool ff textbox:
// gen-dq-textbox -bg "rgb(100,100,200)" -xbg "rgb(0,0,0)" -mbg 0.125 ff.png
package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	"log"
	"math"
	"os"

	"github.com/qeedquan/go-media/image/chroma"
	"github.com/qeedquan/go-media/image/imageutil"
)

var flags struct {
	Width     int
	Height    int
	Thickness int
	MFG, MBG  float64
	FG, XFG   color.RGBA
	BG, XBG   color.RGBA
}

func main() {
	log.SetFlags(0)
	log.SetPrefix("gen-dq-textbox: ")
	parseFlags()

	m := gen(flags.Width, flags.Height, flags.Thickness, flags.FG, flags.BG,
		flags.XFG, flags.XBG, flags.MFG, flags.MBG)
	err := imageutil.WriteRGBAFile(flag.Arg(0), m)
	ck(err)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func parseFlags() {
	var dim, fg, bg, xfg, xbg string
	flag.StringVar(&dim, "d", "1024x128", "image dimension")
	flag.StringVar(&fg, "fg", "#ffffff", "foreground color")
	flag.StringVar(&bg, "bg", "#000000", "backround color")
	flag.StringVar(&xfg, "xfg", "", "foreground color gradient")
	flag.StringVar(&xbg, "xbg", "", "background color gradient")
	flag.IntVar(&flags.Thickness, "t", 8, "thickness")
	flag.Float64Var(&flags.MFG, "mfg", 0, "multiple step size for foreground linear gradient")
	flag.Float64Var(&flags.MBG, "mbg", 0, "multiple step size for background linear gradient")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	fmt.Sscanf(dim, "%dx%d", &flags.Width, &flags.Height)

	flags.FG = parseRGBA(fg)
	flags.BG = parseRGBA(bg)
	flags.XFG = flags.FG
	flags.XBG = flags.BG
	if xfg != "" {
		flags.XFG = parseRGBA(xfg)
	}
	if xbg != "" {
		flags.XBG = parseRGBA(xbg)
	}
	if flags.MFG == 0 {
		flags.MFG = 1.0 / float64(flags.Thickness)
	}
	if flags.MBG == 0 {
		flags.MBG = 1.0 / float64(flags.Height)
	}
}

func parseRGBA(s string) color.RGBA {
	c, err := chroma.ParseRGBA(s)
	ck(err)
	return c
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: gen-dq-textbox [options] output")
	flag.PrintDefaults()
	os.Exit(2)
}

func gen(w, h int, thickness int, fg, bg, xfg, xbg color.RGBA, mfg, mbg float64) *image.RGBA {
	r := image.Rect(0, 0, w, h)
	m := image.NewRGBA(r)

	// bg
	for y := r.Min.Y; y < r.Max.Y; y++ {
		for x := r.Min.X; x < r.Max.X; x++ {
			t := float64(y-r.Min.Y) / float64(r.Max.Y-r.Min.Y)
			t = multiple(t, mbg)
			c := mix(bg, xbg, t)
			m.Set(x, y, c)
		}
	}

	// border
	s := r.Inset(8)
	for x := s.Min.X; x < s.Max.X; x++ {
		for y := 0; y < thickness; y++ {
			t := float64(y) / float64(thickness)
			t = multiple(t, mfg)
			c := mix(fg, xfg, t)
			m.Set(x, s.Min.Y+y, c)
			m.Set(x, s.Max.Y-y-1, c)
		}
	}

	for y := s.Min.Y; y < s.Max.Y; y++ {
		for x := 0; x < thickness; x++ {
			t := float64(x) / float64(thickness)
			t = multiple(t, mfg)
			c := mix(fg, xfg, t)
			m.Set(s.Min.X+x, y, c)
			m.Set(s.Max.X-x-1, y, c)
		}
	}

	// round
	n := thickness * 3 / 4
	for y := 0; y < n; y++ {
		for x := 0; x < n; x++ {
			t := float64(s.Min.Y+y-s.Min.Y) / float64(s.Max.Y-s.Min.Y)
			t = multiple(t, mbg)
			bg1 := mix(bg, xbg, t)

			t = float64(s.Max.Y-y-s.Min.Y) / float64(s.Max.Y-s.Min.Y)
			t = multiple(t, mbg)
			bg2 := mix(bg, xbg, t)

			m.Set(s.Min.X+x, s.Min.Y+y, bg1)
			m.Set(s.Min.X+x+thickness, s.Min.Y+y+thickness, fg)

			m.Set(s.Max.X-x, s.Min.Y+y, bg1)
			m.Set(s.Max.X-x-thickness, s.Min.Y+y+thickness, fg)

			m.Set(s.Min.X+x, s.Max.Y-y, bg2)
			m.Set(s.Min.X+x+thickness, s.Max.Y-y-thickness, fg)

			m.Set(s.Max.X-x, s.Max.Y-y, bg2)
			m.Set(s.Max.X-x-thickness, s.Max.Y-y-thickness, fg)
		}
	}

	return m
}

func mix(a, b color.RGBA, t float64) color.RGBA {
	return color.RGBA{
		uint8(float64(a.R)*(1-t) + t*float64(b.R)),
		uint8(float64(a.G)*(1-t) + t*float64(b.G)),
		uint8(float64(a.B)*(1-t) + t*float64(b.B)),
		uint8(float64(a.A)*(1-t) + t*float64(b.A)),
	}
}

func multiple(a, m float64) float64 {
	return math.Ceil(a/m) * m
}
