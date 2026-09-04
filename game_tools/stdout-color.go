package main

import (
	"bufio"
	"flag"
	"fmt"
	"image/color"
	"io"
	"log"
	"math"
	"os"
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("stdout-color: ")

	flag.Usage = usage
	flag.Parse()

	f := os.Stdin
	var err error
	if flag.NArg() >= 1 {
		f, err = os.Open(flag.Arg(0))
		ck(err)
		defer f.Close()
	}

	colorize(bufio.NewReader(f))
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] <file>")
	flag.PrintDefaults()
	os.Exit(2)
}

func colorize(r io.Reader) {
	s := bufio.NewScanner(r)
	for s.Scan() {
		ln := s.Text()

		var f [4]float64
		n, _ := fmt.Sscanf(ln, "%v %v %v %v", &f[0], &f[1], &f[2], &f[3])
		if n == 0 {
			var u [4]uint8
			n, _ = fmt.Sscanf(ln, "#%02x%02x%02x%02x", &u[0], &u[1], &u[2])
			for i := range u {
				f[i] = float64(u[i])
			}
		}
		flt, col, valid := pixel(f[:n])
		if !valid {
			continue
		}

		fmt.Printf("(%.6f, %.6f, %.6f) ", flt[0], flt[1], flt[2])
		fmt.Printf("(%d, %d, %d) ", col.R, col.G, col.B)
		fmt.Printf("\t\x1b[48;2;%d;%d;%dm    \033[0m", col.R, col.G, col.B)
		fmt.Printf("\n")
	}
}

func pixel(f []float64) (flt [4]float64, col color.RGBA, valid bool) {
	var c [4]float64
	switch len(f) {
	case 1:
		c[0], c[1], c[2], c[3] = f[0], f[0], f[0], 1
	case 3:
		c[3] = 1
		fallthrough
	case 4:
		copy(c[:], f)
	default:
		return
	}

	mi, ma := minf(c[:]), maxf(c[:])
	switch {
	case 0 <= mi && ma <= 1:
		scalef(c[:], 256)
		fallthrough
	default:
		clampf(c[:], 0, 255)
	}

	col = color.RGBA{uint8(c[0]), uint8(c[1]), uint8(c[2]), uint8(c[3])}
	flt = [4]float64{float64(col.R) / 255, float64(col.G) / 255, float64(col.B) / 255, 255}
	valid = true
	return
}

func minf(f []float64) float64 {
	v := math.MaxFloat32
	for i := range f {
		v = math.Min(v, f[i])
	}
	return v
}

func maxf(f []float64) float64 {
	v := -math.MaxFloat32
	for i := range f {
		v = math.Max(v, f[i])
	}
	return v
}

func scalef(f []float64, s float64) {
	for i := range f {
		f[i] *= s
	}
}

func clampf(f []float64, a, b float64) {
	for i := range f {
		f[i] = math.Min(math.Max(f[i], a), b)
	}
}
