// https://en.wikipedia.org/wiki/German_tank_problem

package main

import (
	"fmt"
	"math"
	"math/rand"
)

/*

We want to estimate the maximum in a discrete uniform distribution and we have some samples without replacement against the distribution.

The items are ordered 1 to N, and we like to estimate N using the samples we gotten.

Frequentist and Bayesian approaches give different estimates, but they are near each other.

*/

func main() {
	sim(1e3)
	sim(50)
}

func sim(n int) {
	p := rand.Perm(n)
	m := 0
	for i := range p {
		if p[i] > m {
			m = p[i]
		}

		f_max := frequentist(m+1, i+1)
		b_median, b_mean := bayesian(m+1, i+1)
		fmt.Printf("%d | true %d | max %d | frequentist_max %.3f | bayes median %.3f mean %.3f\n", i+1, n, m, f_max, b_median, b_mean)
	}
	fmt.Println()
}

// m - max number we saw so far
// k - how many samples we drew
func frequentist(mi, ki int) float64 {
	m, k := float64(mi), float64(ki)
	return m + m/k - 1
}

// estimate median and mean
func bayesian(mi, ki int) (median float64, mean float64) {
	m, k := float64(mi), float64(ki)
	median = m + m*math.Log(2)/(k-1)
	mean = (m - 1) * (k - 1) / (k - 2)
	return
}
