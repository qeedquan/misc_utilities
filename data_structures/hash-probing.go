package main

import "fmt"

func main() {
	for i := 0; i <= 15; i++ {
		collider(1<<i, triangular)
	}
}

/*

https://c9x.me/notes/2023-07-14.html
Quadratic probing can visit all slots if the table is a power of 2

*/

func collider(n int, f func(int) int) {
	h := make([]int, n)
	for i := range n {
		clear(h)

		h[i] = 1
		c := 1
		for {
			x := (i + f(c)) % n
			if h[x] != 0 {
				break
			}
			c, h[x] = c+1, 1
		}

		if c != n {
			fmt.Printf("found collision: size %d index %d\n", n, i)
		}
	}
}

func triangular(n int) int {
	return n * (n + 1) / 2
}
