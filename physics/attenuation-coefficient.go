/*

https://en.wikipedia.org/wiki/Attenuation_coefficient
https://en.wikipedia.org/wiki/Beer%E2%80%93Lambert_law
https://www.scratchapixel.com/lessons/advanced-rendering/volume-rendering-for-artists

*/

package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	"image/png"
	"log"
	"math"
	"math/rand"
	"os"
	"time"
)

var (
	width  = flag.Int("w", 512, "window width")
	height = flag.Int("h", 512, "window height")
	sigma  = flag.Float64("s", 0.2, "attentuation coefficient")
	volume = flag.Float64("v", 20, "volume size")
	random = flag.Bool("r", false, "randomize color")
)

func main() {
	rand.Seed(time.Now().UnixNano())
	log.SetFlags(0)
	log.SetPrefix("attenutation-coefficient: ")
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	f, err := os.Create(flag.Arg(0))
	ck(err)

	m := image.NewRGBA(image.Rect(0, 0, *width, *height))
	ramp(m, *sigma, *volume, *random)
	ck(png.Encode(f, m))
	ck(f.Close())
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] output.png")
	flag.PrintDefaults()
	os.Exit(2)
}

/*

The attenuation coefficient characterizes how easily a volume of material
can be penetrated by light/sound/particles/etc. Large coefficient means that
the light gets attenutated much faster (grows weaker as it travels through a medium)

The medium in this case is a square where the light goes in on the left and gets
attenuated as it goes to the right

The equation is modeled as an exponential decay as a function of the attenuation
coefficient sigma and the path length
T = I/I0 = exp(-sigma*path_length)

*/
func ramp(m *image.RGBA, sigma, volume float64, random bool) {
	r := m.Bounds()
	w := r.Dx()
	h := r.Dy()
	col0 := color.RGBA{255, 255, 255, 255}
	if random {
		col0 = randrgb()
	}
	for x := 0; x < w; x++ {
		l := float64(x) / float64(w-1) * volume
		T := math.Exp(-sigma * l)
		col := attenuate(col0, T)
		for y := 0; y < h; y++ {
			m.SetRGBA(x, y, col)
		}
	}
}

func randrgb() color.RGBA {
	return color.RGBA{
		uint8(rand.Intn(256)),
		uint8(rand.Intn(256)),
		uint8(rand.Intn(256)),
		255,
	}
}

func attenuate(c color.RGBA, s float64) color.RGBA {
	return color.RGBA{
		uint8(float64(c.R) * s),
		uint8(float64(c.G) * s),
		uint8(float64(c.B) * s),
		255,
	}
}
