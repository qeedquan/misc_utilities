// https://en.wikipedia.org/wiki/Reservoir_sampling
package main

import (
	"fmt"
	"math"
	"math/rand"
	"time"
)

func main() {
	rand.Seed(time.Now().UnixNano())

	S := make([]int, 1000)
	for i := range S {
		S[i] = i
	}

	R := make([]int, 50)
	for i := 0; i < 1000; i++ {
		fmt.Println(reservoir(S, R))
	}
}

func reservoir(S, R []int) []int {
	copy(R, S)
	W := math.Exp(math.Log(rand.Float64()) / float64(len(R)))

	i := 0
	for i < len(S) {
		i += int(math.Floor(math.Log(rand.Float64())/math.Log(1-W))) + 1
		if i < len(S) {
			R[rand.Intn(len(R))] = S[i]
			W *= math.Exp(math.Log(rand.Float64()) / float64(len(R)))
		}
	}
	return R
}
