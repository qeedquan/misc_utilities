/*
In the early 1600s, Galileo was asked to explain the fact that, although the
number of triples of integers from 1 to 6 with sum 9 is the same as the number
of such triples with sum 10, when three dice are rolled, a 9 seemed to come
up less often than a 10—supposedly in the experience of gamblers.
(a) Write a program to simulate the roll of three dice a large number of
times and keep track of the proportion of times that the sum is 9 and
the proportion of times it is 10.
(b) Can you conclude from your simulations that the gamblers is right?

Answer: gamblers seem to be wrong, in 1e6 to 1e8 simulation shows
that on average, it happens ~11.5% for 9 and ~12.5% for ten

if we brute force all sum pairs that gets us 6, we see
that the sum to pairs is ~11.5% and to ten is ~12.5%, so
this matches our experiment
*/

package main

import (
	"fmt"
	"math"
	"math/rand"
)

const (
	N = 1e7
)

func main() {
	nines := 0
	tens := 0
	for i := 1; i <= 6; i++ {
		for j := 1; j <= 6; j++ {
			for k := 1; k <= 6; k++ {
				s := i + j + k
				if s == 9 {
					nines++
				} else if s == 10 {
					tens++
				}
			}
		}
	}
	total := float64(6 * 6 * 6)
	fmt.Println(total, nines, tens, float64(nines)/total, float64(tens)/total)

	nines = 0
	tens = 0
	for n := 0; n < N; n++ {
		sum := 0
		for i := 0; i < 3; i++ {
			sum += int(math.Floor(rand.Float64()*6) + 1)
		}
		if sum == 9 {
			nines++
		} else if sum == 10 {
			tens++
		}
	}
	fmt.Println(nines, tens)
}
