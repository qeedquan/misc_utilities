// https://en.wikipedia.org/wiki/Colors_of_noise#Blue_noise
// https://www.jasondavies.com/poisson-disc/
// https://bl.ocks.org/mbostock/dbb02448b0f93e4c82c3
// https://observablehq.com/@techsparx/an-improvement-on-bridsons-algorithm-for-poisson-disc-samp/2
// https://github.com/martynafford/poisson-disc-distribution-bridson
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
	"math/rand"
	"os"
	"strconv"
	"time"

	"github.com/qeedquan/go-media/image/chroma"
	"github.com/qeedquan/go-media/math/f64"
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("gen-blue-noise: ")
	rand.Seed(time.Now().UnixNano())

	alg := flag.String("a", "pdb", "type of sampling")
	flag.Usage = usage
	flag.Parse()

	N := 4096
	d := 0.02
	if flag.NArg() >= 2 {
		N, _ = strconv.Atoi(flag.Arg(0))
		d, _ = strconv.ParseFloat(flag.Arg(1), 64)
	}

	var p []f64.Vec2
	switch *alg {
	case "pdb":
		p = blue2pdb(N, d)
	case "rej":
		p = blue2rej(N, d)
	default:
		usage()
	}

	if !chkblue2(p, d) {
		log.Fatal("invalid blue noise distribution")
	}

	w, h := 512, 512
	m := image.NewRGBA(image.Rect(0, 0, w, h))
	draw.Draw(m, m.Bounds(), image.NewUniform(color.Black), image.ZP, draw.Over)
	for _, p := range p {
		x := f64.LinearRemap(p.X, 0, 1, 0, float64(w))
		y := f64.LinearRemap(p.Y, 0, 1, 0, float64(h))
		m.Set(int(x), int(y), chroma.RandRGB())
	}

	png.Encode(os.Stdout, m)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: <number of points> <distance>")
	flag.PrintDefaults()
	fmt.Fprintln(os.Stderr, "available sampling methods: pdb rej")
	os.Exit(2)
}

// rejection sampling, really slow but simple
func blue2rej(n int, d float64) []f64.Vec2 {
	var p []f64.Vec2
	for len(p) < n {
		q := randv2()
		valid := true
		for i := 0; i < len(p); i++ {
			if p[i].Distance(q) < d {
				valid = false
				break
			}
		}

		if valid {
			p = append(p, q)
		}
	}
	return p
}

// returns if valid blue noise distribution
func chkblue2(p []f64.Vec2, d float64) bool {
	valid := true
	for i := 0; i < len(p); i++ {
		for j := i + 1; j < len(p); j++ {
			if t := p[i].Distance(p[j]); t < d {
				fmt.Fprintln(os.Stderr, p[i], p[j], t)
				valid = false
			}
		}
	}
	return valid
}

// poisson disc sampling using bridson's algorithm
func blue2pdb(n int, d float64) []f64.Vec2 {
	var pb pdb2
	pb.init(1, 1, d)
	pb.add(randv2())
	for len(pb.proc) > 0 {
		p := pb.pop()

		for i := 0; i < pb.tries; i++ {
			q := pb.pointaround(p)
			if pb.inarea(q) && !pb.pointclose(q) {
				pb.add(q)
			}
		}
	}

	return pb.pts[:min(n, len(pb.pts))]
}

type pdb2 struct {
	tries   int
	mindist float64
	csz     float64
	gw, gh  int
	grid    []f64.Vec2
	proc    []f64.Vec2
	pts     []f64.Vec2
}

func (c *pdb2) init(w, h, d float64) {
	c.tries = 30
	c.mindist = d
	c.csz = d / math.Sqrt(2)
	c.gw = f64.Iceil(w / c.csz)
	c.gh = f64.Iceil(h / c.csz)
	c.grid = make([]f64.Vec2, c.gw*c.gh)
	for i := range c.grid {
		c.grid[i] = f64.Vec2{math.MaxFloat32, math.MaxFloat32}
	}
}

func (c *pdb2) add(p f64.Vec2) {
	c.proc = append(c.proc, p)
	c.set(p)
	c.pts = append(c.pts, p)
}

func (c *pdb2) set(p f64.Vec2) {
	x := int(p.X / c.csz)
	y := int(p.Y / c.csz)
	c.grid[y*c.gw+x] = p
}

func (c *pdb2) pop() f64.Vec2 {
	p := c.proc[len(c.proc)-1]
	c.proc = c.proc[:len(c.proc)-1]
	return p
}

func (c *pdb2) inarea(p f64.Vec2) bool {
	return 0 <= p.X && p.X <= 1 && 0 <= p.Y && p.Y <= 1
}

func (c *pdb2) pointaround(p f64.Vec2) f64.Vec2 {
	r := c.mindist * math.Sqrt(rand.Float64()*3+1)
	a := rand.Float64() * 2 * math.Pi
	return f64.Vec2{
		p.X + math.Cos(a)*r,
		p.Y + math.Sin(a)*r,
	}
}

func (c *pdb2) pointclose(p f64.Vec2) bool {
	ix := f64.Ifloor(p.X / c.csz)
	iy := f64.Ifloor(p.Y / c.csz)
	if c.grid[iy*c.gw+ix].X != math.MaxFloat32 {
		return true
	}

	mindistsq := c.mindist * c.mindist
	minx := max(ix-2, 0)
	miny := max(iy-2, 0)
	maxx := min(ix+2, c.gw-1)
	maxy := min(iy+2, c.gh-1)

	for y := miny; y <= maxy; y++ {
		for x := minx; x <= maxx; x++ {
			q := c.grid[y*c.gw+x]
			exists := q.X != math.MaxFloat32
			if exists && p.DistanceSquared(q) < mindistsq {
				return true
			}
		}
	}
	return false
}

func randv2() f64.Vec2 {
	return f64.Vec2{
		rand.Float64(),
		rand.Float64(),
	}
}

func max(a, b int) int {
	if a > b {
		return a
	}
	return b
}

func min(a, b int) int {
	if a < b {
		return a
	}
	return b
}
