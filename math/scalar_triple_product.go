// http://realtimecollisiondetection.net/blog/?p=69
package main

import (
	"fmt"
	"math/rand"
	"time"

	"github.com/qeedquan/go-media/math/f64"
)

func main() {
	rand.Seed(time.Now().UnixNano())
	parity()
	basis()
}

/*

Scalar triple products occur frequently enough that they have been given their own special notation:

[a b c] = a*(bxc)

This notation makes it easy to remember that any even permutation (which, for three elements, is equivalent to a cyclic permutation) of the three vectors does not change the expression result, i.e.

[a b c] = [b c a] = [c a b]

Similarly, an odd permutation causes a sign change:

[a b c] = -[a c b]

*/
func parity() {
	fmt.Println("parity:")
	for i := 0; i < 10; i++ {
		p := randv3(1e3)
		q := copyv(p)
		r := copyv(p)
		s := copyv(p)
		q[0], q[1] = q[1], q[0]
		r[1], r[2] = r[2], r[1]
		s[0], s[2] = s[2], s[0]

		for j := 0; j < 3; j++ {
			// if we cyclically shift it, we get even parity
			fmt.Printf("even %s %s %s %.3f\n", strv(p[0]), strv(p[1]), strv(p[2]), stp(p))

			// since we swap one adjacent element above, this forces it to be an odd parity
			fmt.Printf("odd  %s %s %s %.3f\n", strv(s[0]), strv(s[1]), strv(s[2]), stp(s))
			fmt.Printf("odd  %s %s %s %.3f\n", strv(q[0]), strv(q[1]), strv(q[2]), stp(q))
			fmt.Printf("odd  %s %s %s %.3f\n", strv(r[0]), strv(r[1]), strv(r[2]), stp(r))
			shift(p, 1)
			shift(q, 1)
			shift(s, 1)
		}
		fmt.Println()
	}
}

/*

What’s your basis for that basis?
Earlier we concluded that this identity

d = ([d b c]a + [a d c]b + [a b d]c) / [a b c]

allows us to express d in in terms of the nonorthogonal basis { a, b, c } by computing the scalars r, s, and t as per above, giving d = ra + sb + tc. This is all good, but let’s say we have tons of vectors we want expressed in this basis. Can we come up with something cheaper to compute? I mean, if we had an orthonormal basis { a, b, c } instead, we could just simply express d as

d = (d • a)a + (d • b)b + (d • c)c

which is much cheaper (using only 3 dot products). Yes, turns out we can. Because “hidden” inside our identity is a dual (or reciprocal) basis that is orthogonal and that will make our repeated projections much cheaper!

Let’s do some successive rewrites of the identity to reveal this dual basis:

d = ([d b c]a + [a d c]b + [a b d]c) / [a b c]
 = k ([d b c]a + [a d c]b + [a b d]c)
 = k ((d • (b × c))a + (d • (c × a))b + (d • (a × b))c)
 = ((d • a’)a + (d • b’)b + (d • c’)c)

where a’ = k (b × c), b’ = k (c × a), c’ = k (a × b), and k = 1 / [a b c].

Clearly, { a’, b’, c’ } form an orthogonal basis. Not orthonormal, mind you, as these dual basis vectors each are multiplied by the reciprocal lengths of their original basis counterpart, but that’s exactly what we want, because the original basis vectors aren’t guaranteed to be unit length!

So, rather than computing r, s, and t as before, when we have multiple projections to make, we compute the dual basis { a’, b’, c’ } just once and instead compute r, s, and t as

r = d • a’
s = d • b’
t = d • c’

for every d that needs to be transformed.

It turns out that an orthogonal dual basis exists for any nonorthogonal basis (in 3D), so we can use this dual basis “trick” whenever we work with nonorthogonal spaces, which can be quite handy.

*/
func basis() {
	fmt.Println("basis:")

	for i := 0; i < 10; i++ {
		p := randv3(1e3)
		d := randv(1e3)
		w := findweight(p, d)

		MI := dual(p)

		var M f64.Mat3
		M.FromBasis(p[0], p[1], p[2])

		fmt.Println(M.Transform(w), MI.Transform(d))
		fmt.Println(d, w)
		var I f64.Mat3
		I.Mul(&M, &MI)
		fmt.Println(I)
	}
}

// the orthogonal dual basis acts as an inverse of the basis vectors
func dual(p []f64.Vec3) f64.Mat3 {
	a, b, c := p[0], p[1], p[2]
	k := 1 / ste(a, b, c)

	ap := b.Cross(c).Scale(k)
	bp := c.Cross(a).Scale(k)
	cp := a.Cross(b).Scale(k)

	var M f64.Mat3
	M.FromBasis(ap, bp, cp)
	M.Transpose()
	return M
}

// d = ([d b c]a + [a d c]b + [a b d]c) / [a b c]
func findweight(p []f64.Vec3, d f64.Vec3) f64.Vec3 {
	a, b, c := p[0], p[1], p[2]

	k := ste(a, b, c)
	r := ste(d, b, c)
	s := ste(a, d, c)
	t := ste(a, b, d)
	return f64.Vec3{r / k, s / k, t / k}
}

func copyv(p []f64.Vec3) []f64.Vec3 {
	q := make([]f64.Vec3, len(p))
	copy(q, p)
	return q
}

func shift(p []f64.Vec3, n int) {
	for i := 0; i < n; i++ {
		if n < 0 {
			v := p[0]
			copy(p[:], p[1:])
			p[len(p)-1] = v
		} else {
			v := p[len(p)-1]
			copy(p[1:], p)
			p[0] = v
		}
	}
}

func ste(a, b, c f64.Vec3) float64 {
	return stp([]f64.Vec3{a, b, c})
}

func stp(p []f64.Vec3) float64 {
	a := p[1].Cross(p[2])
	b := p[0].Dot(a)
	return b
}

func randv3(n float64) []f64.Vec3 {
	return []f64.Vec3{randv(n), randv(n), randv(n)}
}

func randv(n float64) f64.Vec3 {
	return f64.Vec3{
		rand.Float64() * n,
		rand.Float64() * n,
		rand.Float64() * n,
	}
}

func strv(p f64.Vec3) string {
	return fmt.Sprintf("Vec3(%.2f, %.2f, %.2f)", p.X, p.Y, p.Z)
}
