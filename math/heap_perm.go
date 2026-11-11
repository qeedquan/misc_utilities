// https://en.wikipedia.org/wiki/Heap%27s_algorithm
// generate permuations of n objects, this minimizes movements
package main

import "fmt"

func main() {
	f := func(a []int) { fmt.Println(a) }
	a := []int{1, 2, 3, 4, 5, 6}
	genr(len(a), a, f)
	geni(a, f)
}

func genr(k int, a []int, f func([]int)) {
	if k <= 1 {
		f(a)
		return
	}

	genr(k-1, a, f)
	for i := 0; i < k-1; i++ {
		if k&1 == 0 {
			a[i], a[k-1] = a[k-1], a[i]
		} else {
			a[0], a[k-1] = a[k-1], a[0]
		}
		genr(k-1, a, f)
	}
}

func geni(a []int, f func([]int)) {
	f(a)

	c := make([]int, len(a))
	for i := 0; i < len(a); {
		if c[i] < i {
			if i&1 == 0 {
				a[0], a[i] = a[i], a[0]
			} else {
				a[c[i]], a[i] = a[i], a[c[i]]
			}

			f(a)
			c[i]++
			i = 0
		} else {
			c[i] = 0
			i++
		}
	}
}
