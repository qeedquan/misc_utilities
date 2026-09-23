// https://www4.uwsp.edu/math/nwodarz/Math209Files/209-0809F-L10-Section06_03-AlgorithmsForGeneratingPermutationsAndCombinations-Notes.pdf
package main

import (
	"fmt"
)

func main() {
	// An RLC circuit can have the following combinations:
	// [RL, RC, LC, RLC], we can generate the combinations
	// by having a blank as another element inside of the nCr framework
	// as it represents a state we can pick

	// outputs depend on the order of the array, but they are the same
	// if we sort them at the end
	combination(4, 3, mkstrprint([]string{"R", "L", " ", "C"}))
	combination(4, 3, mkstrprint([]string{"L", "R", "C", " "}))
}

func mkstrprint(p []string) func(int, int, []int) {
	return func(n, r int, c []int) {
		for i := 1; i <= r; i++ {
			fmt.Printf("%s ", p[c[i]-1])
		}
		fmt.Println()
	}
}

func combination(n, r int, f func(int, int, []int)) {
	s := make([]int, r+1)
	for i := 1; i <= r; i++ {
		s[i] = i
	}
	f(n, r, s)

	k := ncr(n, r)
	for i := 2; i <= k; i++ {
		m := r
		M := n
		for s[m] == M {
			m--
			M--
		}

		s[m]++
		for j := m + 1; j <= r; j++ {
			s[j] = s[j-1] + 1
		}
		f(n, r, s)
	}
}

func ncr(n, r int) int {
	return fact(n) / (fact(r) * fact(n-r))
}

func fact(n int) int {
	if n <= 1 {
		return 1
	}
	r := 1
	for i := 2; i <= n; i++ {
		r *= i
	}
	return r
}
