package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	"image/png"
	"log"
	"math"
	"os"
	"strconv"
)

var opt struct {
	gradient string
	width    int
	height   int
	pal      []color.RGBA
}

func main() {
	log.SetFlags(0)
	log.SetPrefix("gen-color-gradient: ")
	parseflags()

	m := image.NewRGBA(image.Rect(0, 0, opt.width, opt.height))
	gradient(m, opt.gradient, opt.pal)

	f, err := os.Create(flag.Arg(flag.NArg() - 1))
	ck(err)
	ck(png.Encode(f, m))
	ck(f.Close())
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func parseflags() {
	flag.IntVar(&opt.width, "width", 256, "image width")
	flag.IntVar(&opt.height, "height", 128, "image height")
	flag.StringVar(&opt.gradient, "gradient", "none", "gradient type")
	flag.Parse()
	if flag.NArg() < 2 {
		usage()
	}

	nargs := flag.Args()
	for _, arg := range nargs[:len(nargs)-1] {
		col, err := parsecolor(arg)
		ck(err)

		opt.pal = append(opt.pal, col)
	}
}

func parsecolor(str string) (col color.RGBA, err error) {
	var sv uint32
	n, _ := fmt.Sscanf(str, "#%x", &sv)
	if n == 1 {
		col = color.RGBA{uint8(sv), uint8(sv >> 8), uint8(sv >> 16), 255}
		return
	}

	iv, xerr := strconv.ParseInt(str, 0, 64)
	if xerr == nil {
		col = color.RGBA{uint8(iv), uint8(iv >> 8), uint8(iv >> 16), 255}
		return
	}

	n, _ = fmt.Sscanf(str, "rgb(%d,%d,%d)", &col.R, &col.G, &col.B)
	if n == 3 {
		col.A = 255
		return
	}

	err = fmt.Errorf("unknown color: %q", str)
	return
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] [colors] gradient.png")
	flag.PrintDefaults()
	fmt.Fprintln(os.Stderr)
	fmt.Fprintln(os.Stderr, "available gradients: horizontal vertical diagonal none")
	os.Exit(2)
}

func gradient(m *image.RGBA, typ string, pal []color.RGBA) {
	r := m.Bounds()
	w := r.Dx()
	h := r.Dy()
	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			px := remap(float64(x), 0, float64(w), 0, 1)
			py := remap(float64(y), 0, float64(h), 0, 1)

			var col color.RGBA
			switch typ {
			case "horizontal":
				col = hgrad(px, py, pal)
			case "vertical":
				col = vgrad(px, py, pal)
			case "diagonal":
				col = dgrad(px, py, pal)
			case "none":
				col = ngrad(px, py, pal)
			default:
				log.Fatalf("unsupported gradient: %q", typ)
			}
			m.SetRGBA(x, y, col)
		}
	}
}

func lgrad(x float64, pal []color.RGBA) color.RGBA {
	n := float64(len(pal) - 1)
	i := int(x * n)
	j := clampi(i+1, i+1, int(n))
	t := math.Mod(x, 1/n) * n
	return mix(pal[i], pal[j], t)
}

func hgrad(x, y float64, pal []color.RGBA) color.RGBA { return lgrad(x, pal) }
func vgrad(x, y float64, pal []color.RGBA) color.RGBA { return lgrad(y, pal) }
func dgrad(x, y float64, pal []color.RGBA) color.RGBA { return lgrad(math.Hypot(x, y)/math.Sqrt2, pal) }

func ngrad(x, y float64, pal []color.RGBA) color.RGBA {
	i := int(x * float64(len(pal)))
	return pal[i]
}

func lerp(t, a, b float64) float64        { return a + t*(b-a) }
func unlerp(t, a, b float64) float64      { return (t - a) / (b - a) }
func remap(x, a, b, c, d float64) float64 { return lerp(unlerp(x, a, b), c, d) }
func clampi(x, a, b int) int              { return min(max(x, a), b) }

func min(a, b int) int {
	if a < b {
		return a
	}
	return b
}

func max(a, b int) int {
	if a > b {
		return a
	}
	return b
}

func mix(a, b color.RGBA, t float64) color.RGBA {
	return color.RGBA{
		uint8(lerp(t, float64(a.R), float64(b.R))),
		uint8(lerp(t, float64(a.G), float64(b.G))),
		uint8(lerp(t, float64(a.B), float64(b.B))),
		255,
	}
}
