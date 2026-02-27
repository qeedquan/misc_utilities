// ported from https://github.com/mxgmn/ConvChain
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

	"github.com/qeedquan/go-media/image/imageutil"
)

var flags struct {
	ReceptorSize int
	Temperature  float64
	OutputSize   int
	Iterations   int
}

func main() {
	log.SetFlags(0)
	log.SetPrefix("convchain: ")
	rand.Seed(time.Now().UnixNano())
	parseFlags()
	if flag.NArg() != 2 {
		usage()
	}

	sample, err := loadMonoImage(flag.Arg(0))
	ck(err)

	bitmap := ConvChain(sample, flags.ReceptorSize, flags.Temperature, flags.OutputSize, flags.Iterations)

	err = writeMonoImage(flag.Arg(1), bitmap)
	ck(err)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func parseFlags() {
	flag.Usage = usage
	flag.IntVar(&flags.ReceptorSize, "r", 2, "receptor size")
	flag.IntVar(&flags.OutputSize, "s", 32, "output size")
	flag.IntVar(&flags.Iterations, "i", 2, "number of iterations")
	flag.Float64Var(&flags.Temperature, "t", 1, "temperature")
	flag.Parse()
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: convchain [options] input output")
	flag.PrintDefaults()
	os.Exit(2)
}

func loadMonoImage(name string) ([][]int, error) {
	m, err := imageutil.LoadGrayFile(name)
	if err != nil {
		return nil, err
	}

	r := m.Bounds()
	p := make([][]int, r.Dy())
	for i := range p {
		p[i] = make([]int, r.Dx())
	}

	for y := range p {
		for x := range p {
			c := m.GrayAt(x, y)
			if c.Y > 0 {
				p[y][x] = 1
			}
		}
	}

	return p, nil
}

func writeMonoImage(name string, bitmap [][]int) error {
	f, err := os.Create(name)
	if err != nil {
		return err
	}

	w := len(bitmap[0])
	h := len(bitmap)
	m := image.NewGray(image.Rect(0, 0, w, h))

	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			if bitmap[y][x] != 0 {
				m.SetGray(x, y, color.Gray{200})
			}
		}
	}

	err = png.Encode(f, m)
	xerr := f.Close()
	if err == nil {
		err = xerr
	}
	return err
}

func ConvChain(sample [][]int, N int, temperature float64, size, iterations int) [][]int {
	field := make([][]int, size)
	for i := range field {
		field[i] = make([]int, size)
	}
	weights := make([]float64, 1<<uint(N*N))

	for y := range sample {
		for x := range sample[y] {
			var p [8]*Pattern

			p[0] = NewPattern(N, PatternField(sample, x, y))
			p[1] = p[0].Rotated()
			p[2] = p[1].Rotated()
			p[3] = p[2].Rotated()
			p[4] = p[0].Reflected()
			p[5] = p[1].Reflected()
			p[6] = p[2].Reflected()
			p[7] = p[3].Reflected()

			for i := range p {
				weights[p[i].Index()] += 1
			}
		}
	}

	for i := range weights {
		if weights[i] <= 0 {
			weights[i] = 0.1
		}
	}

	for i := range field {
		for j := range field {
			field[i][j] = rand.Intn(2)
		}
	}

	energyExp := func(i, j int) float64 {
		value := 1.0
		for y := j - N + 1; y <= j+N-1; y++ {
			for x := i - N + 1; x <= i+N-1; x++ {
				pattern := NewPattern(N, PatternField(field, x, y))
				value *= weights[pattern.Index()]
			}
		}
		return value
	}

	metropolis := func(i, j int) {
		p := energyExp(i, j)
		field[j][i] = 1 - field[j][i]
		q := energyExp(i, j)

		if math.Pow(q/p, 1.0/temperature) < rand.Float64() {
			field[j][i] = 1 - field[j][i]
		}
	}

	for i := 0; i < iterations*size*size; i++ {
		metropolis(rand.Intn(size), rand.Intn(size))
	}

	return field
}

type Pattern struct {
	data [][]int
}

func NewPattern(size int, init func(i, j int) int) *Pattern {
	p := &Pattern{}
	p.data = make([][]int, size)
	for i := range p.data {
		p.data[i] = make([]int, size)
	}

	for j := range p.data {
		for i := range p.data {
			p.data[j][i] = init(i, j)
		}
	}
	return p
}

func (p *Pattern) Rotated() *Pattern {
	return NewPattern(len(p.data), func(x, y int) int {
		return p.data[x][len(p.data)-1-y]
	})
}

func (p *Pattern) Reflected() *Pattern {
	return NewPattern(len(p.data), func(x, y int) int {
		return p.data[y][len(p.data)-1-x]
	})
}

func (p *Pattern) Index() int {
	result := 0
	index := uint(0)
	for i := range p.data {
		for j := range p.data {
			result += p.data[i][j] << index
			index++
		}
	}
	return result
}

func PatternField(field [][]int, x, y int) func(i, j int) int {
	return func(i, j int) int {
		xx := (x + i + len(field[0])) % len(field[0])
		yy := (y + j + len(field)) % len(field)
		return field[yy][xx]
	}
}
