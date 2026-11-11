// scan an image and dump all the colors per scanline
package main

import (
	"flag"
	"fmt"
	"os"

	"github.com/qeedquan/go-media/gu"
	"github.com/qeedquan/go-media/image/imageutil"
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}
	analyze(flag.Arg(0))
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] file")
	flag.PrintDefaults()
	os.Exit(2)
}

func analyze(name string) {
	m, err := imageutil.LoadRGBAFile(name)
	gu.Check(err)

	b := m.Bounds()
	for y := b.Min.Y; y < b.Max.Y; y++ {
		fmt.Printf("Scanline %d\n", y+1)

		n := 1
		for x0 := b.Min.X; x0 < b.Max.X; {
			x1 := x0 + 1
			for x1 < b.Max.X && m.RGBAAt(x0, y) == m.RGBAAt(x1, y) {
				x1 += 1
			}

			col := m.RGBAAt(x0, y)
			fmt.Printf("Color #%d (%d-%d) %v ", n, x0, x1-1, col)
			fmt.Printf("\t\x1b[48;2;%d;%d;%dm    \033[0m", col.R, col.G, col.B)
			fmt.Println()
			x0 = x1
		}
		fmt.Println()
	}
}
