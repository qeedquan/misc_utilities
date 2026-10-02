/*

https://gregorygundersen.com/blog/2019/11/11/morris-algorithm/
https://arpitbhayani.me/blogs/morris-counter/
https://www.inf.ed.ac.uk/teaching/courses/exc/reading/morris.pdf
https://github.com/robpike/typo
https://blog.codingconfessions.com/p/how-unix-spell-ran-in-64kb-ram

Approximate Counting algorithms are techniques that allow us to count a large number of events using a very small amount of memory.
A Morris counter is an algorithm that gives an approximate count of the true counter value (rounded to a near power of 2).

Space Complexity: O(log log N) (normally to store the value N requires O(log N) bits)

Algorithm:
1. Initialize counter 'c' to 0
2. Upon counter increment:
   E(x) = exp(log(2) * x) - 1
   d = 1 / (E(c + 1) - E(c))
   r = generate uniform random value in [0, 1]
   if d < r
        c++
3. E(c) gives the approximate value of the counter at that point in time

*/

package main

import (
	"fmt"
	"math"
	"math/rand"
)

func main() {
	for i := 1; i <= 65535; i++ {
		sim(1, i)
	}
}

func sim(trials, value int) {
	maxabs := 0
	maxrel := 0.0
	for range trials {
		m := Morris{}
		for range value {
			m.Increment()
		}
		maxabs = max(maxabs, abs(m.Value()-value))
		maxrel = max(maxrel, float64(abs(m.Value()-value))/float64(value))
	}
	fmt.Printf("value: %d max abs: %d max rel: %.2f\n", value, maxabs, maxrel)
}

func abs(x int) int {
	if x < 0 {
		x = -x
	}
	return x
}

type Morris struct {
	value int
}

func (c *Morris) Increment() int {
	d := 1 / (c.expvalue(c.value+1) - c.expvalue(c.value))
	r := rand.Float64()
	if r < d {
		c.value++
	}
	return c.value
}

func (c *Morris) expvalue(value int) float64 {
	return math.Exp(math.Ln2*float64(value)) - 1
}

func (c *Morris) Value() int {
	return int(c.expvalue(c.value))
}
