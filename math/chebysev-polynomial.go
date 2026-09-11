// ported from https://algs4.cs.princeton.edu/code/edu/princeton/cs/algs4/Polynomial.java.html
// https://en.wikipedia.org/wiki/Chebyshev_polynomials
package main

import (
	"fmt"
	"os"
	"text/tabwriter"
)

func main() {
	tw := tabwriter.NewWriter(os.Stdout, 1, 4, 1, ' ', 0)
	defer tw.Flush()

	zero := NewPolynomial(0, 0)
	p1 := NewPolynomial(4, 3)
	p2 := NewPolynomial(3, 2)
	p3 := NewPolynomial(1, 0)
	p4 := NewPolynomial(2, 1)
	p := p1.Add(p2).Add(p3).Add(p4)

	q1 := NewPolynomial(3, 2)
	q2 := NewPolynomial(5, 0)
	q := q1.Add(q2)

	r := p.Add(q)
	s := p.Mul(q)
	t := p.Compose(q)
	u := p.Sub(p)

	fmt.Fprintf(tw, "zero(x)\t=\t%v\n", zero)
	fmt.Fprintf(tw, "p(x)\t=\t%v\n", p)
	fmt.Fprintf(tw, "q(x)\t=\t%v\n", q)

	fmt.Fprintf(tw, "p(x) + q(x)\t=\t%v\n", r)
	fmt.Fprintf(tw, "p(x) * q(x)\t=\t%v\n", s)
	fmt.Fprintf(tw, "p(q(x))\t=\t%v\n", t)
	fmt.Fprintf(tw, "p(x) - p(x)\t=\t%v\n", u)
	fmt.Fprintf(tw, "0 - p(x)\t=\t%v\n", zero.Sub(p))
	fmt.Fprintf(tw, "p(3)\t=\t%v\n", p.Eval(3))
	fmt.Fprintf(tw, "p'(x)\t=\t%v\n", p.Differentiate())
	fmt.Fprintf(tw, "p''(x)\t=\t%v\n", p.Differentiate().Differentiate())
	fmt.Fprintf(tw, "\n")

	for i := 0; i <= 32; i++ {
		fmt.Fprintf(tw, "chebyshev1(%d)\t=\t%v\n", i, chebyshev1(i))
	}
	fmt.Fprintf(tw, "\n")

	for i := 0; i <= 32; i++ {
		fmt.Fprintf(tw, "chebyshev2(%d)\t=\t%v\n", i, chebyshev2(i))
	}
}

func chebyshev1(n int) *Polynomial {
	if n < 0 {
		return NewPolynomial(0, 0)
	}

	a := NewPolynomial(1, 0)
	b := NewPolynomial(1, 1)
	if n == 0 {
		return a
	}
	if n == 1 {
		return b
	}

	k := NewPolynomial(2, 1)
	for i := 2; i <= n; i++ {
		c := k.Mul(b).Sub(a)
		a, b = b, c
	}
	return b
}

func chebyshev2(n int) *Polynomial {
	if n < 0 {
		return NewPolynomial(0, 0)
	}

	a := NewPolynomial(1, 0)
	b := NewPolynomial(2, 1)
	if n == 0 {
		return a
	}
	if n == 1 {
		return b
	}

	k := NewPolynomial(2, 1)
	for i := 2; i <= n; i++ {
		c := k.Mul(b).Sub(a)
		a, b = b, c
	}
	return b
}

type Polynomial struct {
	coef   []int
	degree int
}

func NewPolynomial(a, b int, args ...int) *Polynomial {
	if len(args)%2 != 0 {
		panic("invalid polynomial argument length")
	}

	n := b
	for i := 0; i < len(args); i += 2 {
		n = max(n, args[i+1])
	}

	p := &Polynomial{}
	p.coef = make([]int, n+1)
	p.coef[b] = a
	for i := 0; i < len(args); i += 2 {
		p.coef[args[i+1]] = args[i]
	}

	p.reduce()
	return p
}

func (p *Polynomial) reduce() {
	p.degree = -1
	for i := len(p.coef) - 1; i >= 0; i-- {
		if p.coef[i] != 0 {
			p.degree = i
			return
		}
	}
}

func (p *Polynomial) Degree() int {
	return p.degree
}

func (p *Polynomial) Add(q *Polynomial) *Polynomial {
	r := NewPolynomial(0, max(p.degree, q.degree))
	for i := 0; i <= p.degree; i++ {
		r.coef[i] += p.coef[i]
	}
	for i := 0; i <= q.degree; i++ {
		r.coef[i] += q.coef[i]
	}
	r.reduce()
	return r
}

func (p *Polynomial) Sub(q *Polynomial) *Polynomial {
	r := NewPolynomial(0, max(p.degree, q.degree))
	for i := 0; i <= p.degree; i++ {
		r.coef[i] += p.coef[i]
	}
	for i := 0; i <= q.degree; i++ {
		r.coef[i] -= q.coef[i]
	}
	r.reduce()
	return r
}

func (p *Polynomial) Mul(q *Polynomial) *Polynomial {
	r := NewPolynomial(0, p.degree+q.degree)
	for i := 0; i <= p.degree; i++ {
		for j := 0; j <= q.degree; j++ {
			r.coef[i+j] += p.coef[i] * q.coef[j]
		}
	}
	r.reduce()
	return r
}

func (p *Polynomial) Compose(q *Polynomial) *Polynomial {
	r := NewPolynomial(0, 0)
	for i := p.degree; i >= 0; i-- {
		t := NewPolynomial(p.coef[i], 0)
		r = t.Add(q.Mul(r))
	}
	return r
}

func (p *Polynomial) Differentiate() *Polynomial {
	if p.degree == 0 {
		return NewPolynomial(0, 0)
	}

	q := NewPolynomial(0, p.degree-1)
	q.degree = p.degree - 1
	for i := 0; i < p.degree; i++ {
		q.coef[i] = (i + 1) * p.coef[i+1]
	}
	return q
}

func (p *Polynomial) Eval(x int) int {
	v := 0
	for i := p.degree; i >= 0; i-- {
		v = p.coef[i] + (x * v)
	}
	return v
}

func (p *Polynomial) Compare(q *Polynomial) int {
	if p.degree < q.degree {
		return -1
	}
	if p.degree > q.degree {
		return 1
	}
	for i := p.degree; i >= 0; i-- {
		if p.coef[i] < q.coef[i] {
			return -1
		}
		if p.coef[i] > q.coef[i] {
			return 1
		}
	}
	return 0
}

func (p *Polynomial) String() string {
	if p.degree == -1 {
		return "0"
	}
	if p.degree == 0 {
		return fmt.Sprint(p.coef[0])
	}
	if p.degree == 1 {
		s := fmt.Sprintf("%vx", p.coef[1])
		if p.coef[0] != 0 {
			s += fmt.Sprintf(" + %v", p.coef[0])
		}
		return s
	}

	s := fmt.Sprintf("%vx^%v", p.coef[p.degree], p.degree)
	for i := p.degree - 1; i >= 0; i-- {
		if p.coef[i] == 0 {
			continue
		} else if p.coef[i] > 0 {
			s += fmt.Sprintf(" + %v", p.coef[i])
		} else if p.coef[i] < 0 {
			s += fmt.Sprintf(" - %v", -p.coef[i])
		}

		if i == 1 {
			s += "x"
		} else if i > 1 {
			s += fmt.Sprintf("x^%v", i)
		}
	}
	return s
}

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
