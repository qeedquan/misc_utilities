// https://en.wikipedia.org/wiki/Gray_code
package main

import (
	"flag"
	"fmt"
)

func main() {
	var N int
	flag.IntVar(&N, "n", 32, "generate n gray codes")
	flag.Parse()
	gen(N)
}

func gen(n int) {
	for i := 0; i <= n; i++ {
		g := bin2gray(i)
		b := gray2bin(g)
		B := gray2bin32(g)
		fmt.Printf("%d %b %b %b\n", i, b, B, g)
	}
}

func bin2gray(n int) int {
	return n ^ (n >> 1)
}

func gray2bin(n int) int {
	m := n >> 1
	for m != 0 {
		n ^= m
		m >>= 1
	}
	return n
}

func gray2bin32(n int) int {
	n ^= (n >> 16)
	n ^= (n >> 8)
	n ^= (n >> 4)
	n ^= (n >> 2)
	n ^= (n >> 1)
	return n
}
