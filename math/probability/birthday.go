// https://en.wikipedia.org/wiki/Birthday_problem
// simulates birthday problem
package main

import (
	"flag"
	"fmt"
	"math/rand"
	"os"
	"time"
)

var (
	people    = flag.Int("p", 23, "number of people")
	rounds    = flag.Int("r", 1e6, "number of rounds")
	birthdays = flag.Int("b", 365, "number of birthdays")
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if *people < 0 || *rounds < 0 || *birthdays < 0 {
		usage()
	}

	rand.Seed(time.Now().UnixNano())

	a := make([]int, *people)
	h := 0
loop:
	for i := 0; i < *rounds; i++ {
		for j := range a {
			a[j] = rand.Intn(*birthdays)
		}

		for j := range a {
			for k := range a {
				if a[j] == a[k] && j != k {
					h++
					continue loop
				}
			}
		}
	}
	fmt.Printf("people: %d rounds: %d birthdays: %d collision: %f\n",
		*people, *rounds, *birthdays, float64(h)/float64(*rounds))
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options]")
	flag.PrintDefaults()
	os.Exit(2)
}
