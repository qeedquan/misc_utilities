/*

https://en.wikipedia.org/wiki/Factorial_number_system
https://en.wikipedia.org/wiki/Lehmer_code
https://2ality.com/2013/03/permutations.html

Lehmer code is a way to map integers as a unique permutation of n elements.
To generate a permutation using lehmer code:

1. Iterate from [0, factorial(n-1)]
2. Convert the integer into a lehmer code.
Example:
Convert integer (base-10) 3120 to the factorial number system base.
3*3! + 1*2! + 2*1! + 0*0!
3. Use the lehmer code to index into the array of elements to get a permutation of it.

*/

package main

import "fmt"

func main() {
	gen(9)
}

func gen(n int) {
	p := factorial(n)
	for i := 0; i < p; i++ {
		c := int2code(i, n)
		x := code2perm(c)
		fmt.Println(c, x)
	}
}

func int2code(n, p int) []int {
	if p <= 1 {
		return []int{0}
	}
	m := factorial(p - 1)
	d := n / m

	r := []int{d}
	r = append(r, int2code(n%m, p-1)...)
	return r
}

func code2perm(c []int) []int {
	e := make([]int, len(c))
	for i := range e {
		e[i] = i
	}

	r := make([]int, len(c))
	for i := range r {
		r[i] = e[c[i]]
		e = remove(e, c[i])
	}
	return r
}

func remove(a []int, s int) []int {
	return append(a[:s], a[s+1:]...)
}

func factorial(n int) int {
	if n < 1 {
		return 0
	}

	if n < 2 {
		return 1
	}

	r := 1
	for i := 2; i <= n; i++ {
		r *= i
	}
	return r
}
