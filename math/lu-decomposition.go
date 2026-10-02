// https://en.wikipedia.org/wiki/LU_decomposition
// https://en.wikipedia.org/wiki/Crout_matrix_decomposition
package main

import (
	"fmt"
	"math"
)

func main() {
	A := [][]float64{
		{12, 2, 3},
		{43, 5, 6},
		{7, 8, 9},
	}
	B := invert(A)
	C := mul(A, B)
	dump(A)
	dump(B)
	dump(C)
}

func zeroes(n int) [][]float64 {
	a := make([][]float64, n)
	for i := range a {
		a[i] = make([]float64, n)
	}
	return a
}

func xcopy(a [][]float64) [][]float64 {
	b := make([][]float64, len(a))
	for i := range b {
		b[i] = make([]float64, len(a[i]))
		copy(b[i], a[i])
	}
	return b
}

func identity(n int) [][]float64 {
	a := zeroes(n)
	for i := range a {
		a[i][i] = 1
	}
	return a
}

func dump(a [][]float64) {
	for i := range a {
		for j := range a[i] {
			fmt.Printf("%.3f ", a[i][j])
		}
		fmt.Printf("\n")
	}
	fmt.Printf("\n")
}

func mul(a, b [][]float64) [][]float64 {
	n := len(a)
	c := zeroes(n)
	for i := 0; i < n; i++ {
		for j := 0; j < n; j++ {
			for k := 0; k < n; k++ {
				c[i][j] += a[i][k] * b[k][j]
			}
		}
	}
	return c
}

func isfilled(a [][]float64, v float64) bool {
	for i := range a {
		for j := range a[i] {
			if a[i][j] != v {
				return false
			}
		}
	}
	return true
}

// factors a matrix into upper/lower triangular matrices
// by gaussian elimination, stores the pivot indices inside ipvt
func lufactor(a [][]float64, ipvt []int) bool {
	n := len(a)

	// check if singular matrix
	if isfilled(a, 0) {
		return false
	}

	// crout method
	for j := 0; j != n; j++ {
		// calculate upper triangular for this column
		for i := 0; i != j; i++ {
			q := a[i][j]
			for k := 0; k != i; k++ {
				q -= a[i][k] * a[k][j]
			}
			a[i][j] = q
		}

		// calculate lower triangular for this column
		L := 0.0
		R := -1
		for i := j; i != n; i++ {
			q := a[i][j]
			for k := 0; k != j; k++ {
				q -= a[i][k] * a[k][j]
			}
			a[i][j] = q
			x := math.Abs(q)
			if x >= L {
				L = x
				R = i
			}
		}

		// pivot
		if j != R {
			for k := 0; k != n; k++ {
				a[R][k], a[j][k] = a[j][k], a[R][k]
			}
		}

		// keep track of row interchanges and avoid zeroes
		ipvt[j] = R
		if a[j][j] == 0 {
			a[j][j] = 1e-18
		}

		if j != n-1 {
			m := 1.0 / a[j][j]
			for i := j + 1; i != n; i++ {
				a[i][j] *= m
			}
		}
	}

	return true
}

// solves the set of n-linear equations using LU factorization
// previously performed by lufactor
func lusolve(a [][]float64, ipvt []int, b []float64) {
	n := len(a)

	// find first non-zero element
	i := 0
	for ; i != n; i++ {
		r := ipvt[i]
		b[r], b[i] = b[i], b[r]
		if b[i] != 0 {
			break
		}
	}

	// forward substitution using lower triangular
	bi, i := i, i+1
	for ; i < n; i++ {
		r := ipvt[i]
		t := b[r]
		b[r] = b[i]
		for j := bi; j < i; j++ {
			t -= a[i][j] * b[j]
		}
		b[i] = t
	}

	// back substitution using upper triangular
	for i := n - 1; i >= 0; i-- {
		t := b[i]
		for j := i + 1; j != n; j++ {
			t -= a[i][j] * b[j]
		}
		b[i] = t / a[i][i]
	}
}

// returns a matrix that is an inverse of m, nil if it is a singular matrix
func invert(m [][]float64) [][]float64 {
	a := xcopy(m)
	n := len(a)
	ipvt := make([]int, n)
	if !lufactor(a, ipvt) {
		return nil
	}

	b := make([]float64, n)
	inva := zeroes(n)
	for i := 0; i != n; i++ {
		for j := 0; j != n; j++ {
			b[j] = 0
		}
		b[i] = 1
		lusolve(a, ipvt, b)
		for j := 0; j != n; j++ {
			inva[j][i] = b[j]
		}
	}
	return inva
}
