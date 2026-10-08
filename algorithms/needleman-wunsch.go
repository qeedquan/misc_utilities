/*

https://en.wikipedia.org/wiki/Needleman%E2%80%93Wunsch_algorithm
https://www.geeksforgeeks.org/sequence-alignment-problem/
https://web.stanford.edu/class/cs262/presentations/lecture2.pdf
https://bioboot.github.io/bimm143_W20/class-material/nw/

Use dynamic programming to align protein/nucleotide sequences (known as the optimal matching/global alignment problem)

Different implementations uses different conventions for the penalties:
(Match, Mismatch, Gap) penalties can be positive, negative, or zero.
Some implementations specify negative sign convention for penalties

This implementation assumes the following for penalties:
Match=0, Mismatch=+value, Gap=+value

So the score returned will be positive rather than a negative value vs other implementations
However, their absolute value should match

*/

package main

import "fmt"

func main() {
	penalty("AGGGCT", "AGGCA", 3, 2)
	penalty("GATTACA", "GTCGACGCA", 1, 2)
	penalty("ABC", "ABCODWJW", 1, 2)
	penalty("XYZ", "ABC", 34, 22)
}

func penalty(x, y string, pxy, pgap int) {
	m := len(x)
	n := len(y)
	l := m + n

	// table for storing optimal substructure answers
	F := make([][]int, l+1)
	for i := range F {
		F[i] = make([]int, l+1)
	}

	// initializing the table
	for i := 0; i <= l; i++ {
		F[i][0] = i * pgap
		F[0][i] = i * pgap
	}

	// calculating the minimum penalty
	for i := 1; i <= m; i++ {
		for j := 1; j <= n; j++ {
			if x[i-1] == y[j-1] {
				F[i][j] = F[i-1][j-1]
			} else {
				F[i][j] = min(F[i-1][j-1]+pxy, F[i-1][j]+pgap, F[i][j-1]+pgap)
			}
		}
	}

	// Reconstructing the solution
	i, j, xi, yi := m, n, l, l
	X := make([]byte, l+1)
	Y := make([]byte, l+1)
	for i > 0 && j > 0 {
		switch {
		case x[i-1] == y[j-1]:
			X[xi], xi = x[i-1], xi-1
			Y[yi], yi = y[j-1], yi-1
			i, j = i-1, j-1

		case F[i-1][j-1]+pxy == F[i][j]:
			X[xi], xi = x[i-1], xi-1
			Y[yi], yi = y[j-1], yi-1
			i, j = i-1, j-1

		case F[i-1][j]+pgap == F[i][j]:
			X[xi], xi = x[i-1], xi-1
			Y[yi], yi = '_', yi-1
			i = i - 1

		case F[i][j-1]+pgap == F[i][j]:
			X[xi], xi = '_', xi-1
			Y[yi], yi = y[j-1], yi-1
			j = j - 1
		}
	}
	for xi > 0 {
		if i > 0 {
			X[xi], xi, i = x[i-1], xi-1, i-1
		} else {
			X[xi], xi = '_', xi-1
		}
	}
	for yi > 0 {
		if j > 0 {
			Y[yi], yi, j = y[j-1], yi-1, j-1
		} else {
			Y[yi], yi = '_', yi-1
		}
	}

	// Since we have assumed the answer to be n+m long,
	// we need to remove the extra gaps in the starting
	// id represents the index from which the arrays
	// X, Y are useful
	id := 1
	for i = l; i >= 1; i-- {
		if Y[i] == '_' && X[i] == '_' {
			id = i + 1
			break
		}
	}

	// Printing the final answer
	fmt.Printf("Minimum Penalty in aligning the genes = ")
	fmt.Printf("%d\n", F[m][n])
	fmt.Printf("The aligned genes are:\n")
	for i := id; i <= l; i++ {
		fmt.Printf("%c", X[i])
	}
	fmt.Printf("\n")
	for i := id; i <= l; i++ {
		fmt.Printf("%c", Y[i])
	}
	fmt.Println()
	fmt.Println()
}
