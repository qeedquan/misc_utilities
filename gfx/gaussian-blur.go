// http://www.ohiouniversityfaculty.com/youngt/IntNumMeth/
// https://blog.demofox.org/2015/08/19/gaussian-blur/
// http://northstar-www.dartmouth.edu/doc/idl/html_6.2/Filtering_an_Imagehvr.html#wp1027824
package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	"image/draw"
	_ "image/gif"
	_ "image/jpeg"
	"image/png"
	"log"
	"math"
	"os"
)

var (
	blursigma = flag.Float64("blursigma", 1.4, "sigma coefficient")
	blursize  = flag.Int("blursize", -1, "kernel size, if less than zero, auto-detect size based on sigma)")
	brighten  = flag.Float64("brighten", 0.0, "brighten additive factor")
	grayscale = flag.Bool("grayscale", false, "grayscale the image")
	dumpkern  = flag.Bool("dumpkern", false, "dump kernel coefficients")
	filters   MultiFlag
	afilters  MultiFlag
)

type MultiFlag []string

func (m *MultiFlag) String() string {
	return fmt.Sprint(*m)
}

func (m *MultiFlag) Set(s string) error {
	*m = append(*m, s)
	return nil
}

func main() {
	log.SetPrefix("gaussian-blur: ")
	log.SetFlags(0)

	flag.Var(&filters, "filter", "apply filter")
	flag.Var(&afilters, "afilter", "apply additive filter")
	flag.Usage = usage
	flag.Parse()

	if *blursize < 0 {
		*blursize = kernelsize(*blursigma)
	}
	if *dumpkern {
		B := gengauss(*blursize, *blursigma)
		printmat("Gaussian Kernel", B)
		return
	}

	if flag.NArg() < 2 {
		usage()
	}
	err := blur(flag.Arg(0), flag.Arg(1))
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: gaussian-blur infile outfile")
	flag.PrintDefaults()
	fmt.Fprintln(os.Stderr, "available filters: lowpass highpass sharpen1 sharpen2 direction laplacian1 laplacian2")
	os.Exit(2)
}

func kernelsize(s float64) int {
	return 1 + int(2*math.Ceil(math.Sqrt(-2*s*s*math.Log(0.005))))
}

func getfilter(s string) [][]float64 {
	switch s {
	case "lowpass":
		return [][]float64{
			{1.0 / 9, 1.0 / 9, 1.0 / 9},
			{1.0 / 9, 1.0 / 9, 1.0 / 9},
			{1.0 / 9, 1.0 / 9, 1.0 / 9},
		}
	case "highpass":
		return [][]float64{
			{-1.0 / 9, -1.0 / 9, -1.0 / 9},
			{-1.0 / 9, 8.0 / 9, -1.0 / 9},
			{-1.0 / 9, -1.0 / 9, -1.0 / 9},
		}
	case "sharpen1":
		return [][]float64{
			{-1.0 / 9, -1.0 / 9, -1.0 / 9},
			{-1.0 / 9, 1, -1.0 / 9},
			{-1.0 / 9, -1.0 / 9, -1.0 / 9},
		}
	case "sharpen2":
		return [][]float64{
			{0, -1, 0},
			{-1, 5, -1},
			{0, -1, 0},
		}
	case "direction":
		return [][]float64{
			{-1, 0, 1},
			{-1, 0, 1},
			{-1, 0, 1},
		}
	case "laplacian1":
		return [][]float64{
			{0, -1, 0},
			{-1, 4, -1},
			{0, -1, 0},
		}
	case "laplacian2":
		return [][]float64{
			{-1, -1, -1},
			{-1, 8, -1},
			{-1, -1, -1},
		}
	default:
		log.Fatalf("unknown filter %q", s)
	}
	panic("unreachable")
}

func blur(infile, outfile string) error {
	f, err := os.Open(infile)
	if err != nil {
		return err
	}
	defer f.Close()
	m, _, err := image.Decode(f)
	if err != nil {
		return err
	}

	B := gengauss(*blursize, *blursigma)
	cr, cg, cb := rgb2float(m)
	if *brighten != 0 {
		cr = bright(cr, *brighten)
		cg = bright(cg, *brighten)
		cb = bright(cb, *brighten)
	}
	cr = convolve(cr, B)
	cg = convolve(cg, B)
	cb = convolve(cb, B)
	for i := range filters {
		M := getfilter(filters[i])
		cr = convolve(cr, M)
		cg = convolve(cg, M)
		cb = convolve(cb, M)
	}
	for i := range afilters {
		M := getfilter(afilters[i])
		c1 := convolve(cr, M)
		c2 := convolve(cg, M)
		c3 := convolve(cb, M)
		add(cr, c1)
		add(cg, c2)
		add(cb, c3)
	}

	w, err := os.Create(outfile)
	if err != nil {
		return err
	}

	m = float2rgb(cr, cg, cb)
	if *grayscale {
		g := image.NewGray(m.Bounds())
		draw.Draw(g, g.Bounds(), m, image.ZP, draw.Over)
		m = g
	}

	err = png.Encode(w, m)
	xerr := w.Close()
	if err == nil {
		err = xerr
	}
	return err
}

func printmat(label string, p [][]float64) {
	w, h := len(p), 0
	if w > 0 {
		h = len(p[0])
	}
	fmt.Printf("%s (%dx%d)\n", label, w, h)
	for i := range p {
		for j := range p[i] {
			fmt.Printf("%.6f ", p[i][j])
		}
		fmt.Println()
	}
}

func add(p, q [][]float64) {
	for i := range p {
		for j := range p[i] {
			p[i][j] += q[i][j]
		}
	}
}

func bright(m [][]float64, d float64) [][]float64 {
	p := dupz(m)
	for i := range p {
		for j := range p[i] {
			p[i][j] = m[i][j] + d
		}
	}
	return p
}

func rgb2float(m image.Image) (cr, cg, cb [][]float64) {
	b := m.Bounds()
	cr = genmat(b.Dy(), b.Dx())
	cg = genmat(b.Dy(), b.Dx())
	cb = genmat(b.Dy(), b.Dx())
	for y := b.Min.Y; y < b.Max.Y; y++ {
		for x := b.Min.X; x < b.Max.X; x++ {
			c := color.RGBAModel.Convert(m.At(x, y)).(color.RGBA)
			cr[y][x] = float64(c.R)
			cg[y][x] = float64(c.G)
			cb[y][x] = float64(c.B)
		}
	}
	return
}

func float2rgb(r, g, b [][]float64) *image.RGBA {
	w, h := len(r[0]), len(r)
	m := image.NewRGBA(image.Rect(0, 0, w, h))
	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			m.SetRGBA(x, y, color.RGBA{clamp8(r[y][x]), clamp8(g[y][x]), clamp8(b[y][x]), 255})
		}
	}
	return m
}

func genmat(r, c int) [][]float64 {
	p := make([][]float64, r)
	q := make([]float64, r*c)
	for i := range p {
		p[i] = q[i*c : (i+1)*c]
	}
	return p
}

func gengauss(n int, s float64) [][]float64 {
	w := genmat(n, n)

	f := func(x, y float64) float64 {
		return gauss2d(x, y, 0, 0, s)
	}

	const N = 50
	wn := 0.0
	for i := 0; i < n; i++ {
		y0 := float64(i-n/2-1) + 0.5
		y1 := y0 + 1
		for j := 0; j < n; j++ {
			x0 := float64(j-n/2-1) + 0.5
			x1 := x0 + 1
			w[i][j] = simpson2d(f, x0, x1, y0, y1, N, N)
			wn += w[i][j]
		}
	}
	for i := range w {
		for j := range w[i] {
			w[i][j] /= wn
		}
	}
	return w
}

func gauss2d(x, y, mx, my, s float64) float64 {
	dx := x - mx
	dy := y - my
	n := 1 / (2 * math.Pi * s * s)
	r := n * math.Exp(-(dx*dx+dy*dy)/(2*s*s))
	return r
}

func midpoint2d(f func(x, y float64) float64, x0, x1, y0, y1 float64, m, n int) float64 {
	dx := (x1 - x0) / float64(m)
	dy := (y1 - y0) / float64(n)
	r := 0.0
	for i := 0; i < m; i++ {
		ly := y0 + float64(i)*dy
		ry := y0 + float64(i+1)*dy
		y := (ly + ry) / 2
		for j := 0; j < n; j++ {
			lx := x0 + float64(j)*dx
			rx := x0 + float64(j+1)*dx
			x := (lx + rx) / 2
			r += f(x, y)
		}
	}
	r *= dx * dy
	return r
}

func simpsonweight(i, n int) float64 {
	if i == 0 || i == n {
		return 1
	}
	if i%2 != 0 {
		return 4
	}
	return 2
}

func simpson2d(f func(x, y float64) float64, x0, x1, y0, y1 float64, m, n int) float64 {
	if n%2 != 0 || m%2 != 0 {
		panic("integration range must be even")
	}

	dx := (x1 - x0) / float64(m)
	dy := (y1 - y0) / float64(n)
	r := 0.0
	for i := 0; i <= n; i++ {
		y := y0 + float64(i)*dy
		wy := simpsonweight(i, n)
		for j := 0; j <= m; j++ {
			x := x0 + float64(j)*dx
			wx := simpsonweight(j, m)
			r += f(x, y) * wx * wy
		}
	}
	r *= dx * dy / (9 * float64(m) * float64(n))
	return r
}

func clamp8(c float64) uint8 {
	c = math.Round(c)
	if c < 0 {
		c = 0
	} else if c > 255 {
		c = 255
	}
	return uint8(c)
}

func dupz(m [][]float64) [][]float64 {
	if len(m) == 0 || len(m[0]) == 0 {
		return nil
	}

	p := genmat(len(m), len(m[0]))
	return p
}

func convolve(m, c [][]float64) [][]float64 {
	if len(c) == 0 || len(c[0]) == 0 {
		return m
	}
	a := len(c)
	b := len(c[0])
	p := dupz(m)
	for i := range m {
		for j := range m[i] {
			for k := -a / 2; k <= a/2; k++ {
				for l := -b / 2; l <= b/2; l++ {
					y := i - k
					x := j - l
					if 0 <= y && y < len(m) && 0 <= x && x < len(m[y]) {
						p[i][j] += m[y][x] * c[a/2+k][b/2+l]
					}
				}
			}
		}
	}
	return p
}
