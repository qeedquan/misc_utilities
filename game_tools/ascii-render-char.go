/*

Renders font characters as ASCII art

*/

package main

import (
	"flag"
	"fmt"
	"log"
	"os"

	"github.com/qeedquan/go-media/stb/stbtt"
)

var flags struct {
	Font  string
	Scale float64
}

func main() {
	flags.Scale = 20
	flag.StringVar(&flags.Font, "f", flags.Font, "use font file")
	flag.Float64Var(&flags.Scale, "s", flags.Scale, "use scale")
	flag.Usage = usage
	flag.Parse()
	if flags.Font == "" || flag.NArg() < 1 {
		usage()
	}

	f, err := loadfont(flags.Font)
	ck(err)
	defer f.Free()

	for _, arg := range flag.Args() {
		for _, ch := range arg {
			render(f, ch)
		}
	}
}

func render(f *stbtt.FontInfo, ch rune) {
	b, w, h, _, _ := f.CodepointBitmap(0, f.ScaleForPixelHeight(flags.Scale), ch)
	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			const p = "  .:ioVM@"
			fmt.Printf("%c", p[b[y*w+x]>>5])
		}
		fmt.Println()
	}
	fmt.Println()
}

func loadfont(name string) (*stbtt.FontInfo, error) {
	data, err := os.ReadFile(name)
	if err != nil {
		return nil, err
	}

	var f stbtt.FontInfo
	err = f.Init(data, stbtt.GetFontOffsetForIndex(data, 0))
	return &f, nil
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] chars ...")
	flag.PrintDefaults()
	os.Exit(2)
}
