package main

func main() {
	test_sum_xorand(1000)
	test_add_ab1_overflow_free(1000)
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

// floor(k + x) = k + floor(x) for k ∈ Z.
// x = floor(x) + frac(x)
// floor(x + y) = floor(floor(x) + frac(x) + y) = floor(x) + floor(frac(x) + y)
