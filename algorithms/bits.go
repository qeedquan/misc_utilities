package main

import "fmt"

func main() {
	t_mask(-1000, 1000)
	t_bounds(-256, 0)
	t_bounds(0, 256)
}

/*

Finds the first lsb set bit and generate a mask for the lower bits
101000 -> 111
(x&-x)-1

x|-x = -(x&-x)

Clears all the lower bits (x is a power of 2)
x&-x = x&~(x-1)

*/

func t_mask(n, m int) {
	fmt.Printf("Testing mask x|-x\n")
	for i := n; i <= m; i++ {
		fmt.Printf("%d %0b %0b | %0b %0b\n", i, i, (i&-i)-1, i|-i, -(i & -i))
	}
	fmt.Printf("\n")

	fmt.Printf("Testing mask x&-x\n")
	for i := n; i <= m; i++ {
		fmt.Printf("%d | %0b %0b\n", i, i&-i, i&^(i-1))
	}
	fmt.Println()
}

/*

max(a, b) <= a|b <= 2*max(a, b) [a, b >= 0]
max(a, b) >= a|b >= 2*max(a, b) [a, b < 0]

*/

func t_bounds(n, m int) {
	fmt.Printf("Testing bounds\n")
	for i := n; i <= m; i++ {
		for j := n; j <= m; j++ {
			fmt.Printf("%d %d | %d %d %d\n", i, j, max(i, j), i|j, max(i, j)<<1)
		}
	}
	fmt.Printf("\n")
}

func max(a, b int) int {
	if a > b {
		return a
	}
	return b
}
