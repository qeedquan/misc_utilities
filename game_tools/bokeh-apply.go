/*

https://www.scratchapixel.com/lessons/digital-imaging/simple-image-manipulations/bookeh-effect
https://en.wikipedia.org/wiki/Bokeh

*/

package main

import (
	"flag"
	"fmt"
	"log"
	"math"
	"os"

	"github.com/qeedquan/go-media/image/imageutil"
)

var (
	lightthreshold = flag.Float64("t", 0.7, "light threshold")
	lightfactor    = flag.Float64("l", 3, "lighten factor")
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("bokeh-apply: ")
	flag.Parse()
	if flag.NArg() != 3 {
		usage()
	}

	in, err := imageutil.LoadFloatFile(flag.Arg(0))
	mask, err := imageutil.LoadFloatFile(flag.Arg(1))
	ck(err)
	ck(err)

	lighten(in, *lightthreshold, *lightfactor)
	lighten(mask, *lightthreshold, *lightfactor)

	out := imageutil.NewFloat(in.Bounds())
	bokeh(in, mask, out)

	err = imageutil.WriteRGBAFile(flag.Arg(2), out.ToRGBA())
	ck(err)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] infile maskfile outfile")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

/*

The idea is for every non-zero value in the mask, circularly mix the image with the mask at the non-zero location.
The circular mixing is achieved by moving the image around and multiplying it with the mask, then adds it to the output.
Since the image values are normalized, this circular mixing has the effect of averaging out the image and make it blurry.
We lighten the parts where we want the focus to be to make it larger than the normalized range [0, 1].
The darken and brighten parts follow the shape of the mask.

*/
func bokeh(in, mask, out *imageutil.Float) {
	tmp := imageutil.NewFloat(in.Bounds())
	r := in.Bounds()
	w := r.Dx()
	h := r.Dy()
	t := 0.0

	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			c := mask.FloatAt(x, y)
			if !eq(c, [4]float64{0, 0, 0, 1}) {
				circshift(tmp, in, x, y)
				mix(tmp, c)
				add(out, tmp)
				t += average(c)
			}
		}
	}
	mul(out, 1/t)
}

func lighten(m *imageutil.Float, th, ds float64) {
	for i := range m.Pix {
		for j := 0; j < 3; j++ {
			if m.Pix[i][j] > th {
				m.Pix[i][j] *= ds
			}
		}
	}
}

func eq(a, b [4]float64) bool {
	const eps = 1e-8
	return math.Abs(a[0]-b[0]) <= eps &&
		math.Abs(a[1]-b[1]) <= eps &&
		math.Abs(a[2]-b[2]) <= eps
}

func average(a [4]float64) float64 {
	return (a[0] + a[1] + a[2]) / 3
}

func mul(m *imageutil.Float, s float64) {
	for i := range m.Pix {
		m.Pix[i] = [4]float64{
			m.Pix[i][0] * s,
			m.Pix[i][1] * s,
			m.Pix[i][2] * s,
			m.Pix[i][3],
		}
	}
}

func mix(m *imageutil.Float, c [4]float64) {
	for i := range m.Pix {
		m.Pix[i][0] *= c[0]
		m.Pix[i][1] *= c[1]
		m.Pix[i][2] *= c[2]
		m.Pix[i][3] = 1
	}
}

func add(a, b *imageutil.Float) {
	for i := range a.Pix {
		a.Pix[i][0] += b.Pix[i][0]
		a.Pix[i][1] += b.Pix[i][1]
		a.Pix[i][2] += b.Pix[i][2]
		a.Pix[i][3] = 1
	}
}

func circshift(c, m *imageutil.Float, sx, sy int) {
	r := m.Bounds()
	w := r.Dx()
	h := r.Dy()
	for y := 0; y < h; y++ {
		j := (y + sy) % h
		for x := 0; x < w; x++ {
			i := (x + sx) % w
			c.SetFloat(i, j, m.FloatAt(x, y))
		}
	}
}
