// some permutation properties
package main

import (
	"fmt"
	"math/rand"
	"sort"
	"time"
)

func main() {
	rand.Seed(time.Now().UnixNano())
	itertest()
	permsortedtest()
	permtest()
}

func permtest() {
	a := []string{"a", "b", "c", "d", "e"}
	i := 1
	perm(a, func(p []string) {
		fmt.Println(i, p)
		i++
	})
}

func permsortedtest() {
	a := []string{"abc", "def", "ghi", "jkl"}
	i := 1
	permsorted(a, func(p []string) {
		fmt.Println(i, p)
		i++
	})
}

func itertest() {
	p := []int{4, 1, 0, 2, 3}
	for i := 0; i < 32; i++ {
		for j := 0; j < 32; j++ {
			fmt.Printf("iteridx1(%d) %v\n", j, iteridx1(p, j))
			fmt.Printf("iteridx2(%d) %v\n", j, iteridx2(p, j))
		}
		p = rand.Perm(i)
	}
}

// find in the array which index maps to the current index
// that we use to iterate, and then keep doing that until
// the number of times we want is achieved
// returns the indices generated when we are done
func iteridx1(p []int, n int) []int {
	q := append([]int{}, p...)

	for i := range p {
		v := i
	loop:
		for j := 0; j < n; j++ {
			for k := range p {
				if p[k] == v {
					v = k
					q[i] = v
					continue loop
				}
			}
		}
	}
	return q
}

// trades memory for speed, gives same result as
// iteridx1, but faster due to the fact we don't
// have to search through the array to find the index
// but what we do is apply the permutation inverse operator
// on the array n times
func iteridx2(p []int, n int) []int {
	if n <= 0 {
		return append([]int{}, p...)
	}

	a := make([]int, len(p))
	b := make([]int, len(p))
	for i := range b {
		b[i] = i
	}
	for i := 0; i < n; i++ {
		for j := range a {
			a[p[j]] = b[j]
		}
		a, b = b, a
	}
	return b
}

func perm(p []string, f func([]string)) {
	permrec(p, 0, f)
}

func permrec(p []string, l int, f func([]string)) {
	f(p)
	for i := l; i < len(p); i++ {
		for j := i + 1; j < len(p); j++ {
			p[i], p[j] = p[j], p[i]
			permrec(p, i+1, f)
			p[i], p[j] = p[j], p[i]
		}
	}
}

// https://www.geeksforgeeks.org/lexicographic-permutations-of-string/
// permute in lexicographic order, algorithm works as follows
// sorted in ascending order as a starting point
// then start at end and move back, finding the first element that is smaller
// then its next one, use that as an anchor, then find the smallest element
// to the right of that, then swap it, sort that substring and continue on
// until no elements are less than its peers to the right
// we have swapped all elements to be descending at this point
func permsorted(p []string, f func([]string)) {
	sort.Strings(p)
	for {
		f(p)

		i := len(p) - 2
		for ; i >= 0; i-- {
			if p[i] < p[i+1] {
				break
			}
		}
		if i < 0 {
			break
		}

		j := findceil(p, p[i], i+1, len(p)-1)
		p[i], p[j] = p[j], p[i]

		sort.Strings(p[i+1:])
	}
}

func findceil(p []string, f string, l, h int) int {
	n := l
	for i := l + 1; i <= h; i++ {
		if p[i] > f && p[i] < p[n] {
			n = i
		}
	}
	return n
}
