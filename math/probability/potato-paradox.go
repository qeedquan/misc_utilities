/*

https://en.wikipedia.org/wiki/Potato_paradox

Fred brings home 100 kg of potatoes, which (being purely mathematical potatoes) consist of 99% water (being purely mathematical water).
He then leaves them outside overnight so that they consist of 98% water. What is their new weight?

The surprising answer is 50 kg.

*/

package main

import "fmt"

func main() {
	for i := 100.0; i >= 0; i-- {
		fmt.Println(i, percent(i))
	}
}

func percent(p float64) float64 {
	return 1 / (p/100 + 1)
}
