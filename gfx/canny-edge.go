// http://justin-liang.com/tutorials/canny/
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
	edgeop    = flag.String("edgeop", "sobel3", "edge operator")
	threshi   = flag.Float64("threshi", 0.7, "threshold high ratio")
	threslo   = flag.Float64("threslo", 0.1, "threshold low ratio")
	maxbright = flag.Bool("maxbright", true, "use maximum brightness in images when detecting edges")
)

func main() {
	log.SetPrefix("canny-edge: ")
	log.SetFlags(0)

	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 2 {
		usage()
	}

	if *blursize < 0 {
		*blursize = blurkernelsize(*blursigma)
	}
	err := detect(flag.Arg(0), flag.Arg(1))
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: canny-edge infile outfile")
	flag.PrintDefaults()
	os.Exit(2)
}

func blurkernelsize(s float64) int {
	return 1 + int(2*math.Ceil(math.Sqrt(-2*s*s*math.Log(0.005))))
}

func detect(infile, outfile string) error {
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
	g := grayscale(m)
	p := gray2float(g)
	p = convolve(p, B, 0x1)
	Ex, Ey := getedgeop(*edgeop)
	Gx := convolve(p, Ex, 0x1)
	Gy := convolve(p, Ey, 0x1)
	Gm := dupz(Gx)
	Ga := dupz(Gx)
	for i := range Gm {
		for j := range Gm[i] {
			Gm[i][j] = math.Hypot(Gx[i][j], Gy[i][j])
			Ga[i][j] = rad2deg(math.Atan2(Gy[i][j], Gx[i][j]))
		}
	}
	p = nonmaxsup(Gx, Gy, Gm, Ga)
	p, vm, vl, vh := dblthres(p, *threslo, *threshi)
	p = trackedges(p, vm, vl, vh)

	w, err := os.Create(outfile)
	if err != nil {
		return err
	}
	err = png.Encode(w, float2rgba(p))
	xerr := w.Close()
	if err == nil {
		err = xerr
	}
	return err
}

func grayscale(m image.Image) *image.Gray {
	g := image.NewGray(m.Bounds())
	draw.Draw(g, g.Bounds(), m, image.ZP, draw.Over)
	return g
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

func clamp(x, a, b int) int {
	if x < a {
		x = a
	}
	if x > b {
		x = b
	}
	return x
}

func dupz(m [][]float64) [][]float64 {
	if len(m) == 0 || len(m[0]) == 0 {
		return nil
	}

	p := genmat(len(m), len(m[0]))
	return p
}

func convolve(m, c [][]float64, f uint) [][]float64 {
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
					if f&0x1 != 0 {
						y = clamp(y, 0, len(m)-1)
						x = clamp(x, 0, len(m[y])-1)
					}
					if 0 <= y && y < len(m) && 0 <= x && x < len(m[y]) {
						p[i][j] += m[y][x] * c[a/2+k][b/2+l]
					}
				}
			}
		}
	}
	return p
}

func gray2float(m *image.Gray) [][]float64 {
	r := m.Bounds()
	p := genmat(r.Dy(), r.Dx())
	for i := range p {
		for j := range p[i] {
			c := m.GrayAt(j, i)
			p[i][j] = float64(c.Y)
		}
	}
	return p
}

func float2rgba(m [][]float64) *image.RGBA {
	if len(m) == 0 || len(m[0]) == 0 {
		return nil
	}
	w, h := len(m[0]), len(m)
	p := image.NewRGBA(image.Rect(0, 0, w, h))
	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			c := clamp8(m[y][x])
			p.Set(x, y, color.RGBA{c, c, c, 255})
		}
	}
	return p
}

func getedgeop(op string) (Ex, Ey [][]float64) {
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

func nonmaxsup(Gx, Gy, Gm, Ga [][]float64) [][]float64 {
	p := dupz(Gm)
	w, h := len(p[0]), len(p)
	for i := 2; i < h-1; i++ {
		for j := 2; j < w-1; j++ {
			var yb, yt [2]float64
			var xest float64
			switch a := Ga[i][j]; {
			case (a >= 0 && a <= 45) || (a < -135 && a >= -180):
				yb = [2]float64{Gm[i][j+1], Gm[i+1][j+1]}
				yt = [2]float64{Gm[i][j-1], Gm[i-1][j-1]}
				xest = math.Abs(Gy[i][j] / Gm[i][j])
			case (a > 45 && a <= 90) || (a < -90 && a >= -135):
				yb = [2]float64{Gm[i+1][j], Gm[i+1][j+1]}
				yt = [2]float64{Gm[i-1][j], Gm[i-1][j-1]}
				xest = math.Abs(Gx[i][j] / Gm[i][j])
			case (a > 90 && a <= 135) || (a < -45 && a >= -90):
				yb = [2]float64{Gm[i+1][j], Gm[i+1][j-1]}
				yt = [2]float64{Gm[i-1][j], Gm[i-1][j+1]}
				xest = math.Abs(Gx[i][j] / Gm[i][j])
			case (a > 135 && a <= 180) || (a < 0 && a >= -45):
				yb = [2]float64{Gm[i][j-1], Gm[i+1][j-1]}
				yt = [2]float64{Gm[i][j+1], Gm[i-1][j+1]}
				xest = math.Abs(Gx[i][j] / Gm[i][j])
			}
			if Gm[i][j] >= (yb[1]-yb[0])*xest+yb[0] && Gm[i][j] >= (yt[1]-yt[0])*xest+yt[0] {
				p[i][j] = Gm[i][j]
			}
		}
	}
	return p
}

func dblthres(m [][]float64, lo, hi float64) (p [][]float64, vm, vl, vh float64) {
	p = dupz(m)
	for i := range m {
		for j := range m[i] {
			if vm < m[i][j] {
				vm = m[i][j]
			}
		}
	}
	vl = vm * lo
	vh = vm * hi
	for i := range m {
		for j := range m[i] {
			if m[i][j] > vl {
				p[i][j] = m[i][j]
			}
		}
	}
	return
}

// edge tracking hysteresis does not seem to work well, loses too much edges,
// seems like there are small locations of 0 separating strong edge
// from weak edge or the double thresholding has destroyed connections
// between strong and weak edges so just connect them all
func trackedges(m [][]float64, vm, vl, vh float64) [][]float64 {
	p := dupz(m)
	for i := range m {
		for j := range m[i] {
			if m[i][j] != 0 {
				if *maxbright {
					p[i][j] = vm
				} else {
					p[i][j] = m[i][j]
				}
			}
		}
	}
	return p
}

func rad2deg(x float64) float64 {
	return x * 180 / math.Pi
}
