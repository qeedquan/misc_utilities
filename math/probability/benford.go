// https://introcs.cs.princeton.edu/java/21function/Benford.java.html
// https://en.wikipedia.org/wiki/Benford%27s_law
package main

import (
	"flag"
	"fmt"
	"os"
)

func leading(x int) int {
	for x >= 10 {
		x /= 10
	}
	return x
}

func main() {
	flag.Parse()

	var (
		c [10]int
		n int
	)
	for {
		var x int
		_, err := fmt.Fscanf(os.Stdin, "%d", &x)
		if err != nil {
			break
		}

		c[leading(x)]++
		n++
	}

	for i := 1; i < 10; i++ {
		x := float64(c[i])
		if n != 0 {
			x /= float64(n)
		}
		fmt.Printf("%d: %6.1f%%\n", i, 100*x)
	}
}
