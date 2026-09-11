/*

http://paulbourke.net/geometry/transformationprojection/
https://en.wikipedia.org/wiki/Aitoff_projection
https://en.wikipedia.org/wiki/Hammer_projection
https://en.wikipedia.org/wiki/Mercator_projection
https://mathworld.wolfram.com/MercatorProjection.html
https://en.wikipedia.org/wiki/Miller_cylindrical_projection

*/

package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	"image/draw"
	"image/png"
	"log"
	"math"
	"os"
	"runtime"
	"sync"
	"sync/atomic"
)

var opt struct {
	width   int
	height  int
	slices  int
	spsize  float64
	spstep  float64
	threads int
}

func main() {
	parseflags()
	name := flag.Arg(1)
	switch typ := flag.Arg(0); typ {
	case "aitoff":
		genprojection(name, ll2ai)
	case "hammer":
		genprojection(name, ll2ha)
	case "mercator":
		genprojection(name, ll2mrc)
	case "miller":
		genprojection(name, ll2miller)
	default:
		log.Fatalf("unknown projection %q\n", typ)
	}
}

func genprojection(name string, prj func(lo, la float64) (x, y float64)) {
	f, err := os.Create(name)
	ck(err)

	m := image.NewRGBA(image.Rect(0, 0, opt.width, opt.height))
	drawgrid(m, opt.slices, opt.spsize, opt.spstep, prj, opt.threads)

	ck(png.Encode(f, m))
	ck(f.Close())
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] type projection.png")
	flag.PrintDefaults()
	fmt.Fprintln(os.Stderr, "available projections: aitoff hammer mercator miller")
	os.Exit(2)
}

func parseflags() {
	const mag = 8
	flag.IntVar(&opt.width, "w", 360*mag, "image width")
	flag.IntVar(&opt.height, "h", 180*mag, "image height")
	flag.IntVar(&opt.slices, "n", 60, "grid slice size")
	flag.Float64Var(&opt.spsize, "s", 0.5, "supersampling size")
	flag.Float64Var(&opt.spstep, "t", 0.01, "supersampling step")
	flag.IntVar(&opt.threads, "p", runtime.NumCPU(), "number of threads to use")

	flag.Parse()
	if flag.NArg() != 2 {
		usage()
	}
}

func drawgrid(m *image.RGBA, n int, sp, dsp float64, prj func(lo, la float64) (x, y float64), nt int) {
	r := m.Bounds()
	w := r.Dx()
	h := r.Dy()

	draw.Draw(m, r, image.NewUniform(color.White), image.ZP, draw.Src)

	pix := make([]uint32, w*h)
	var ln uint32
	var wg sync.WaitGroup
	for i := 0; i < nt; i++ {
		wg.Add(1)
		go drawline(pix, w, h, n, sp, dsp, prj, &wg, &ln)
	}
	wg.Wait()

	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			if pix[y*w+x] != 0 {
				m.SetRGBA(x, y, color.RGBA{0, 0, 0, 255})
			}
		}
	}
}

func drawline(pix []uint32, w, h, n int, sp, dsp float64, prj func(lo, la float64) (x, y float64), wg *sync.WaitGroup, ln *uint32) {
	for {
		y := int(atomic.AddUint32(ln, 1)) - 1
		if y > h {
			break
		}

		for x := 0; x <= w; x++ {
			if x%n == 0 || y%n == 0 {
				for i := -sp; i <= sp; i += dsp {
					for j := -sp; j <= sp; j += dsp {
						hx, hy := prj(xy2ll(float64(x)+i, float64(y)+j))
						hx = lrmap(hx, -1, 1, 0, float64(w))
						hy = lrmap(hy, -1, 1, 0, float64(h))

						k := int(hy+0.5)*w + int(hx+0.5)
						if 0 <= k && k < len(pix) {
							atomic.StoreUint32(&pix[k], 1)
						}
					}
				}
			}
		}

	}
	wg.Done()
}

// (x, y) are [0, w]:[0, h]
// (lo, la) are [-pi,pi]:[-pi/2,pi/2]
func xy2ll(x, y float64) (lo, la float64) {
	lo = lrmap(x, 0, float64(opt.width), -math.Pi, math.Pi)
	la = lrmap(y, 0, float64(opt.height), -math.Pi/2, math.Pi/2)
	return
}

// (x, y) are normalized to [-1, 1]
func ll2ha(lo, la float64) (x, y float64) {
	z := math.Sqrt(1 + math.Cos(la)*math.Cos(lo/2))
	x = math.Cos(la) * math.Sin(lo/2) / z
	y = math.Sin(la) / z
	return
}

func ha2ll(x, y float64) (lo, la float64) {
	z := math.Sqrt(1 - x*x/2 - y*y/2)
	lo = 2 * math.Atan(math.Sqrt2*x*z/(2*z*z-1))
	la = math.Asin(math.Sqrt2 * y * z)
	return
}

// (x, y) are from [-pi,pi]:[-pi/2,pi/2]
// we remap to [-1:1] at the end
func ll2ai(lo, la float64) (x, y float64) {
	a := math.Acos(math.Cos(la) * math.Cos(lo/2))
	s := sinc(a)
	x = 2 * math.Cos(la) * math.Sin(lo/2) / s
	y = math.Sin(la) / s
	x = lrmap(x, -math.Pi, math.Pi, -1, 1)
	y = lrmap(y, -math.Pi/2, math.Pi/2, -1, 1)
	return
}

// (x, y) are from [-pi,pi]:[-inf,inf]
// we clamp y to [-pi:pi] and remap to [-1:1]
func ll2mrc(lo, la float64) (x, y float64) {
	lo0 := 0.0
	x = lo - lo0
	y = math.Atanh(math.Sin(la))
	x = lrmap(x, -math.Pi, math.Pi, -1, 1)
	y = clamp(y, -math.Pi, math.Pi)
	y = lrmap(y, -math.Pi, math.Pi, -1, 1)
	return
}

func ll2miller(lo, la float64) (x, y float64) {
	x = lo
	y = 5.0 / 4 * math.Asinh(math.Tan(4*la/5))
	x = lrmap(x, -math.Pi, math.Pi, -1, 1)
	y = lrmap(y, -1.8655684825455494, 1.8655684825455494, -1, 1)
	return
}

func sinc(x float64) float64 {
	if x == 0 {
		return 0
	}
	return math.Sin(x) / x
}

func clamp(x, a, b float64) float64 {
	return math.Min(math.Max(x, a), b)
}

func lerp(t, a, b float64) float64        { return a + t*(b-a) }
func unlerp(t, a, b float64) float64      { return (t - a) / (b - a) }
func lrmap(x, a, b, c, d float64) float64 { return lerp(unlerp(x, a, b), c, d) }
