// http://www.sunshine2k.de/coding/java/Houghtransformation/HoughTransform.html
// http://www.di.univr.it/documenti/OccorrenzaIns/matdid/matdid666794.pdf
// https://www.ics.uci.edu/~majumder/DIP/classes/EdgeDetect.pdf
// http://www.hlevkin.com/articles/SobelScharrGradients5x5.pdf
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
	threshold = flag.Float64("t", 100, "edge operator threshold value [0,255]")
	houghperc = flag.Float64("p", 0.3, "hough percentage threshold value [0,1]")
	edgeop    = flag.String("ep", "sobel3", "edge operator to use")
	outedge   = flag.Bool("e", false, "output edge image")
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 2 {
		usage()
	}
	log.SetFlags(0)
	log.SetPrefix("hough-transform: ")
	ck(detect(flag.Arg(0), flag.Arg(1)))
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] infile outfile")
	flag.PrintDefaults()
	fmt.Fprintln(os.Stderr, "supported edge operators: freichen3 pixdiff3 robert3 seppixdiff3 sobel3 sobel5 prewitt3 prewitt5 scharr3 scharr5")
	os.Exit(2)
}

func detect(infile, outfile string) error {
	f, err := os.Open(infile)
	if err != nil {
		return err
	}

	m, _, err := image.Decode(f)
	if err != nil {
		return err
	}

	w, err := os.Create(outfile)
	if err != nil {
		return err
	}

	Ex, Ey := getedgeop(*edgeop)
	p := grayscale(m)
	G := edges(p, Ex, Ey)
	T := holdem(G, *threshold)

	var L *image.RGBA
	if *outedge {
		L = fl2img(T)
	} else {
		_, HC, hc, hm := hough(T)
		JC := holdem(HC, hm**houghperc)

		L = image.NewRGBA(m.Bounds())
		hough2lines(L, JC, hc)
	}

	png.Encode(w, L)
	return w.Close()
}

func grayscale(m image.Image) *image.Gray {
	r := m.Bounds()
	p := image.NewGray(r)
	draw.Draw(p, r, m, image.ZP, draw.Over)
	return p
}

func holdem(G [][]float64, threshold float64) [][]float64 {
	w, h := 0, len(G)
	if h > 0 {
		w = len(G[0])
	}
	T := make([][]float64, h)
	p := make([]float64, w*h)
	for y := range T {
		T[y] = p[y*w : (y+1)*w]
	}
	for y := range G {
		for x := range G[y] {
			T[y][x] = G[y][x]
			if T[y][x] < threshold {
				T[y][x] = 0
			}
		}
	}
	return T
}

func getedgeop(op string) (Ex, Ey [][]float64) {
	// not normalized, for edge detection
	// we just scale the threshold
	// (if we were calculating gradient, it would matter)
	switch op {
	case "scharr5":
		Ex = [][]float64{
			{-1, -1, 0, 1, 1},
			{-2, -2, 0, 2, 2},
			{-3, -6, 0, 6, 3},
			{-2, -2, 0, 2, 2},
			{-1, -1, 0, 1, 1},
		}
		Ey = [][]float64{
			{-1, -2, -3, -2, -1},
			{-1, -2, -6, -2, -1},
			{0, 0, 0, 0, 0},
			{1, 2, 6, 2, 1},
			{1, 2, 3, 2, 1},
		}
	case "prewitt5":
		Ex = [][]float64{
			{-2, -1, 0, 1, 2},
			{-2, -1, 0, 1, 2},
			{-2, -1, 0, 1, 2},
			{-2, -1, 0, 1, 2},
			{-2, -1, 0, 1, 2},
		}
		Ey = [][]float64{
			{-2, -2, -2, -2, -2},
			{-1, -1, -1, -1, -1},
			{0, 0, 0, 0, 0},
			{1, 1, 1, 1, 1},
			{2, 2, 2, 2, 2},
		}
	case "sobel5":
		Ex = [][]float64{
			{1, 2, 0, -2, -1},
			{4, 8, 0, -8, -4},
			{6, 12, 0, -12, -6},
			{4, 8, 0, -8, -4},
			{1, 2, 0, -2, -1},
		}
		Ey = [][]float64{
			{1, 4, 6, 4, 1},
			{2, 8, 12, 8, 2},
			{0, 0, 0, 0, 0},
			{-2, -8, -12, -8, -2},
			{-1, -4, -6, -4, -1},
		}
	case "pixdiff3":
		Ex = [][]float64{
			{0, 0, 0},
			{1, 0, -1},
			{0, 0, 0},
		}
		Ey = [][]float64{
			{0, -1, 0},
			{0, 0, 0},
			{0, 1, 0},
		}
	case "seppixdiff3":
		Ex = [][]float64{
			{0, 0, 0},
			{1, 0, -1},
			{0, 0, 0},
		}
		Ey = [][]float64{
			{0, -1, 0},
			{0, 0, 0},
			{0, 1, 0},
		}
	case "freichen3":
		Ex = [][]float64{
			{1, 0, -1},
			{math.Sqrt(2), 0, -math.Sqrt(2)},
			{1, 0, -1},
		}
		Ey = [][]float64{
			{-1, -math.Sqrt(2), -1},
			{0, 0, 0},
			{1, math.Sqrt(2), 1},
		}
	case "robert3":
		Ex = [][]float64{
			{0, 0, -1},
			{0, 1, 0},
			{0, 0, 0},
		}
		Ey = [][]float64{
			{-1, 0, 0},
			{0, 1, 0},
			{0, 0, 0},
		}
	case "scharr3":
		Ex = [][]float64{
			{3, 0, -3},
			{10, 0, -10},
			{3, 0, -3},
		}
		Ey = [][]float64{
			{3, 10, 3},
			{0, 0, 0},
			{-3, -10, -3},
		}
	case "prewitt3":
		Ex = [][]float64{
			{1, 0, -1},
			{1, 0, -1},
			{1, 0, -1},
		}
		Ey = [][]float64{
			{1, 1, 1},
			{0, 0, 0},
			{-1, -1, -1},
		}
	case "sobel3":
		Ex = [][]float64{
			{1, 0, -1},
			{2, 0, -2},
			{1, 0, -1},
		}
		Ey = [][]float64{
			{1, 2, 1},
			{0, 0, 0},
			{-1, -2, -1},
		}
	default:
		log.Fatalf("unknown edge operator %q", op)
	}
	return
}

func edges(m *image.Gray, Ex, Ey [][]float64) [][]float64 {
	r := m.Bounds()
	w, h := r.Dx(), r.Dy()
	G := make([][]float64, h)
	p := make([]float64, w*h)
	for y := 0; y < h; y++ {
		G[y] = p[y*w : (y+1)*w]
	}

	n := len(Ex)
	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			cx, cy := 0.0, 0.0
			for i := -n / 2; i <= n/2; i++ {
				for j := -n / 2; j <= n/2; j++ {
					c := m.GrayAt(x-j, y-i)
					cx += float64(c.Y) * Ex[n/2+i][n/2+j]
					cy += float64(c.Y) * Ey[n/2+i][n/2+j]
				}
			}
			G[y][x] = math.Hypot(cx, cy)
			G[y][x] = clamp(G[y][x], 0, 255)
		}
	}
	return G
}

func fl2img(fl [][]float64) *image.RGBA {
	w, h := 0, len(fl)
	if h > 0 {
		w = len(fl[0])
	}
	m := image.NewRGBA(image.Rect(0, 0, w, h))
	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			c := uint8(clamp(fl[y][x], 0, 255))
			m.SetRGBA(x, y, color.RGBA{c, c, c, 255})
		}
	}
	return m
}

func hough(G [][]float64) (HT, HC [][]float64, hc, hm float64) {
	const A = 180
	HT = make([][]float64, A)
	HC = make([][]float64, A)

	w, h := 0, len(G)
	if h > 0 {
		w = len(G[0])
	}

	n := int(math.Ceil(math.Sqrt(float64(w*w + h*h))))
	p := make([]float64, A*n)
	for i := range HT {
		HT[i] = p[i*n : (i+1)*n]
	}

	n *= 2
	p = make([]float64, A*n)
	for i := range HC {
		HC[i] = p[i*n : (i+1)*n]
	}

	hc = float64(n >> 1)
	for y := range G {
		for x := range G[y] {
			if G[y][x] == 0 {
				continue
			}

			for i := 0; i < A; i++ {
				a := deg2rad(float64(i))
				r := float64(x)*math.Cos(a) + float64(y)*math.Sin(a)

				j := int(math.Abs(r))
				HT[i][j]++

				j = int(r + hc)
				HC[i][j]++

				if hm < HC[i][j] {
					hm = HC[i][j]
				}
			}
		}
	}
	return
}

func hough2lines(m draw.Image, H [][]float64, hc float64) {
	r := m.Bounds()
	l := math.Ceil(math.Hypot(float64(r.Dx()), float64(r.Dy())))
	for i := range H {
		for j := range H[i] {
			if H[i][j] == 0 {
				continue
			}
			a := deg2rad(float64(i))
			r := float64(j)
			si, co := math.Sincos(a)
			px := (r - hc) * co
			py := (r - hc) * si

			var na float64
			if i < 90 {
				na = 90 + float64(i)
			} else {
				na = 90 - (180 - float64(i))
			}
			na = deg2rad(na)
			si, co = math.Sincos(na)

			x0 := int(px + l*co)
			x1 := int(px - l*co)
			y0 := int(py + l*si)
			y1 := int(py - l*si)
			fmt.Printf("(%d,%d) - (%d,%d)\n", x0, y0, x1, y1)
			line(m, x0, y0, x1, y1, color.RGBA{255, 255, 255, 255})
		}
	}
}

func clamp(x, a, b float64) float64 {
	if x < a {
		x = a
	}
	if x > b {
		x = b
	}
	return x
}

func abs(x int) int {
	if x < 0 {
		x = -x
	}
	return x
}

func deg2rad(x float64) float64 { return x * math.Pi / 180 }
func rad2deg(x float64) float64 { return x * 180 / math.Pi }

func line(m draw.Image, x0, y0, x1, y1 int, col color.RGBA) {
	dx := abs(x1 - x0)
	dy := -abs(y1 - y0)
	sx, sy := -1, -1
	if x0 < x1 {
		sx = 1
	}
	if y0 < y1 {
		sy = 1
	}
	err := dx + dy
	for {
		m.Set(x0, y0, col)
		if x0 == x1 && y0 == y1 {
			break
		}
		e2 := 2 * err
		if e2 >= dy {
			err += dy
			x0 += sx
		}
		if e2 <= dx {
			err += dx
			y0 += sy
		}
	}
}
