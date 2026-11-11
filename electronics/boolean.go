package main

import "fmt"

func main() {
	truth()
	demorgan()
}

func truth() {
	fmt.Printf("Truth Table\n")
	fmt.Printf("x y notx noty and nand or nor xor xnor\n")
	for i := uint(0); i < 4; i++ {
		x := i & 1
		y := (i >> 1) & 1
		fmt.Printf("%d %d %4d %4d %3d %4d %2d %3d %3d %3d\n",
			x, y, not(x), not(y), and(x, y), nand(x, y), or(x, y), nor(x, y), xor(x, y), xnor(x, y))
	}
	fmt.Println()
}

func demorgan() {
	fmt.Printf("De Morgan's\n")
	fmt.Printf("x y d1 d1 d2 d2\n")
	for i := uint(0); i < 4; i++ {
		x := i & 1
		y := (i >> 1) & 1
		fmt.Printf("%d %d %2d %2d %2d %2d\n",
			x, y,
			not(or(x, y)), and(not(x), not(y)),
			not(and(x, y)), or(not(x), not(y)),
		)
	}
	fmt.Println()
}

func or(x, y uint) uint {
	return (x | y) & 1
}

func and(x, y uint) uint {
	return (x & y) & 1
}

func not(x uint) uint {
	return ^x & 1
}

func nand(x, y uint) uint {
	return not(x&y) & 1
}

func nor(x, y uint) uint {
	return not(x|y) & 1
}

func xor(x, y uint) uint {
	return (x & not(y)) | (y&not(x))&1
}

func xnor(x, y uint) uint {
	return not(xor(x, y)) & 1
}
