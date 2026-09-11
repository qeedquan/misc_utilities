package main

func main() {
	test_sum_xorand(1000)
	test_add_ab1_overflow_free(1000)
	test_mod(1000)
	test_average(1000)
	test_offset_to_direction()
}

func assert(x bool) {
	if !x {
		panic("assertion failed")
	}
}

// sum(a, b) = xor(a, b) + 2*and(a, b)
func test_sum_xorand(n int) {
	for a := -n; a <= n; a++ {
		for b := -n; b <= n; b++ {
			assert(a+b == a^b+((a&b)<<1))
		}
	}
}

// (a+b+1)>>1 = (a | b) - ((a ^ b) >> 1)
func test_add_ab1_overflow_free(n int) {
	for a := -n; a <= n; a++ {
		for b := -n; b <= n; b++ {
			assert((a+b+1)>>1 == (a|b)-((a^b)>>1))
		}
	}
}

// C/C++/Go/etc treats -a%b as -(a%b) where a and b can be positive or negative
// Python uses the math definition of mod for implement %
// So their behaviors are different when a < 0 or b < 0
func test_mod(n int) {
	for a := -n; a <= n; a++ {
		for b := -n; b <= n; b++ {
			if b != 0 {
				x := -a % b
				y := -(a % b)
				assert(x == y)
			}
		}
	}
}

/*

https://devblogs.microsoft.com/oldnewthing/20220207-00/?p=106223

A way to average number without overflowing

*/

// This needs to know which value is larger
func average_no_overflow_1(a, b uint) uint {
	low := min(a, b)
	high := max(a, b)
	return low + (high-low)/2
}

// This doesn't depend on knowing which value is larger
func average_no_overflow_2(a, b uint) uint {
	return (a >> 1) + (b >> 1) + (a & b & 1)
}

func test_average(n uint) {
	for a := range n {
		for b := range n {
			assert(average_no_overflow_1(a, b) == (a+b)/2)
			assert(average_no_overflow_2(a, b) == (a+b)/2)
		}
	}
}

/*

For 2D grid movements:

if the input is (1, 0) the output should be 0,
if the input is (0, 1) the output should be 1,
if the input is (-1, 0) the output should be 2,
if the input is (0, -1) the output should be 3.

*/

func test_offset_to_direction() {
	assert(offset_to_direction(1, 0) == 0)
	assert(offset_to_direction(0, 1) == 1)
	assert(offset_to_direction(-1, 0) == 2)
	assert(offset_to_direction(0, -1) == 3)
}

func offset_to_direction(x, y int) int {
	return (y & 0x1) | ((x | y) & 0x2)
}

// floor(k + x) = k + floor(x) for k ∈ Z.
// x = floor(x) + frac(x)
// floor(x + y) = floor(floor(x) + frac(x) + y) = floor(x) + floor(frac(x) + y)
