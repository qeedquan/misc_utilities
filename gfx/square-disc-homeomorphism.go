/*

https://en.wikipedia.org/wiki/Lp_space
https://blancosilva.wordpress.com/teaching/an-introduction-to-algebraic-topology/homeomorphisms/

In practice there are a few common LP norms that are common to measure distances between things:

L1

Benefits:
- Sparsity: The L1 norm encourages sparsity in solutions, making it useful in feature selection and situations where you want to minimize the number of active features (e.g., Lasso regression).
- Robustness: It is less sensitive to outliers compared to the L2 norm, as it does not square the values.
- Geometric Interpretation: The L1 norm corresponds to the Manhattan distance, which can be more intuitive in certain applications.

Drawbacks:
- Non-differentiability: The L1 norm is not differentiable at zero, which can complicate optimization processes.
- Solutions Can Be Ambiguous: In some cases, solutions may not be unique (multiple solutions can yield the same L1 norm).

L2
Benefits:
- Differentiability: The L2 norm is differentiable everywhere, which makes it easier to optimize using gradient-based methods.
- Sensitivity to Magnitude: It gives more weight to larger values due to squaring, which can be beneficial in certain applications where outliers are relevant.
- Geometric Interpretation: The L2 norm corresponds to the Euclidean distance, which is often the most natural measure of distance in many applications.
- Rotationally invariant: Rotating a vector and then measuring the length does not change the length.

Drawbacks:
- Sensitivity to Outliers: The squaring operation can make the L2 norm overly sensitive to outliers, potentially skewing results.
- Less Sparsity: It does not promote sparsity in the same way that the L1 norm does, which can lead to more complex models.

Loo [max(x1, x2, ..., xi)]
Benefits:
- Robustness to Small Changes: It is robust to small perturbations in the vector, focusing only on the largest deviation.
- Simplicity: The calculation is straightforward, as it only requires finding the maximum absolute value.
- Useful in Certain Contexts: It can be useful in optimization problems where the worst-case scenario is of interest.

Drawbacks:
- Non-differentiability: Like the L1 norm, the L∞ norm is not differentiable, which can complicate optimization.
- Less Informative: It provides less information about the overall distribution of values in the vector compared to the L1 and L2 norms.

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
	"math/rand"
	"os"
	"time"
)

var (
	width   = flag.Int("width", 1024, "image width")
	height  = flag.Int("height", 1024, "image height")
	sqsize  = flag.Float64("sqsize", 400, "square size")
	spstep  = flag.Float64("spstep", 1e-3, "sampling step")
	lpstart = flag.Float64("lpstart", 1, "lp norm start")
	lpend   = flag.Float64("lpend", 20, "lp norm end")
	lpstep  = flag.Float64("lpstep", 1, "lp step")
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	rand.Seed(time.Now().UnixNano())
	m := image.NewRGBA(image.Rect(0, 0, 1024, 1024))
	draw.Draw(m, m.Bounds(), image.NewUniform(color.RGBA{140, 130, 120, 255}), image.ZP, draw.Src)
	for i := *lpend; i >= *lpstart; i -= *lpstep {
		square2lpdisc(m, float64(*width/2), float64(*height/2), *sqsize, *spstep, i, randrgb())
	}

	f, err := os.Create(flag.Arg(0))
	ck(err)
	ck(png.Encode(f, m))
	ck(f.Close())
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] out.png")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
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

/*

Given a topological space X and Y we can have a map that is homeomorphic if
it satisfies 3 properties:

1) bijective
2) continuous
3) continuous inverse

A unit square [-1, 1] can be map into a disc centered at 0 with radius of 1 with the formula

p(x, y) = 0 if (x, y) is (0, 0)
p(x, y) = max(abs(x), abs(y)) / length(x, y)

where length(x, y) is the l2-norm for a disc, we can use other length(x, y) from (1..2) as a way to slowly "morph"
the square to a disc, in general a square is homeomorphic to a disk in the lp-norm where the inverse is

p'(x, y) = length(x, y)/max(abs(x), abs(y)) where (x, y) is in the lp-space

*/

func square2lpdisc(m *image.RGBA, cx, cy, sz, ds, p float64, col color.RGBA) {
	for y := -1.0; y <= 1; y += ds {
		for x := -1.0; x <= 1; x += ds {
			px, py := 0.0, 0.0
			if x != 0 && y != 0 {
				ax := math.Abs(x)
				ay := math.Abs(y)

				n := math.Max(ax, ay)
				d := math.Pow(math.Pow(ax, p)+math.Pow(ay, p), 1/p)

				px = n / d * x
				py = n / d * y
			}
			px = px*sz + cx
			py = py*sz + cy
			m.SetRGBA(int(px), int(py), col)
		}
	}
}
