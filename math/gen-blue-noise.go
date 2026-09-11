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
	"github.com/qeedquan/go-media/math/ga"
	"github.com/qeedquan/go-media/math/ga/vec2"
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

	var p []ga.Vec2d
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
		x := ga.LinearRemap(p.X, 0, 1, 0, float64(w))
		y := ga.LinearRemap(p.Y, 0, 1, 0, float64(h))
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
func blue2rej(n int, d float64) []ga.Vec2d {
	var p []ga.Vec2d
	for len(p) < n {
		q := randv2()
		valid := true
		for i := 0; i < len(p); i++ {
			if vec2.Distance(p[i], q) < d {
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
func chkblue2(p []ga.Vec2d, d float64) bool {
	valid := true
	for i := 0; i < len(p); i++ {
		for j := i + 1; j < len(p); j++ {
			if t := vec2.Distance(p[i], p[j]); t < d {
				fmt.Fprintln(os.Stderr, p[i], p[j], t)
				valid = false
			}
		}
	}
	return valid
}

// poisson disc sampling using bridson's algorithm
func blue2pdb(n int, d float64) []ga.Vec2d {
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
	grid    []ga.Vec2d
	proc    []ga.Vec2d
	pts     []ga.Vec2d
}

func (c *pdb2) init(w, h, d float64) {
	c.tries = 30
	c.mindist = d
	c.csz = d / math.Sqrt(2)
	c.gw = int(ga.Ceil(w / c.csz))
	c.gh = int(ga.Ceil(h / c.csz))
	c.grid = make([]ga.Vec2d, c.gw*c.gh)
	for i := range c.grid {
		c.grid[i] = ga.Vec2d{math.MaxFloat32, math.MaxFloat32}
	}
}

func (c *pdb2) add(p ga.Vec2d) {
	c.proc = append(c.proc, p)
	c.set(p)
	c.pts = append(c.pts, p)
}

func (c *pdb2) set(p ga.Vec2d) {
	x := int(p.X / c.csz)
	y := int(p.Y / c.csz)
	c.grid[y*c.gw+x] = p
}

func (c *pdb2) pop() ga.Vec2d {
	p := c.proc[len(c.proc)-1]
	c.proc = c.proc[:len(c.proc)-1]
	return p
}

func (c *pdb2) inarea(p ga.Vec2d) bool {
	return 0 <= p.X && p.X <= 1 && 0 <= p.Y && p.Y <= 1
}

func (c *pdb2) pointaround(p ga.Vec2d) ga.Vec2d {
	r := c.mindist * math.Sqrt(rand.Float64()*3+1)
	a := rand.Float64() * 2 * math.Pi
	return ga.Vec2d{
		p.X + math.Cos(a)*r,
		p.Y + math.Sin(a)*r,
	}
}

func (c *pdb2) pointclose(p ga.Vec2d) bool {
	ix := int(math.Floor(p.X / c.csz))
	iy := int(math.Floor(p.Y / c.csz))
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
			d := vec2.Distance(p, q)
			if exists && d*d < mindistsq {
				return true
			}
		}
	}
	return false
}

func randv2() ga.Vec2d {
	return ga.Vec2d{
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
