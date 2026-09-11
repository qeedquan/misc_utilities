// https://en.wikipedia.org/wiki/Sieve_of_Atkin
package main

import (
	"flag"
	"fmt"
	"log"
	"math"
	"os"
	"strconv"
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("atkin: ")
	flag.Usage = usage
	flag.Parse()

	var err error
	n := int(1e5)
	if flag.NArg() >= 1 {
		n, err = strconv.Atoi(flag.Arg(0))
		if err != nil {
			log.Fatal(err)
		}
	}
	prime := make([]bool, n+1)
	atkin(prime)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: atkin [options] limit")
	flag.PrintDefaults()
	os.Exit(2)
}

func atkin(prime []bool) {
	limit := len(prime) - 1
	switch limit {
	case 0, 1:
		return
	case 2:
		fmt.Println("2")
		return
	case 3:
		fmt.Println("2")
		fmt.Println("3")
		return
	}

	for i := 5; i <= limit; i++ {
		prime[i] = false
	}

	l := int(math.Sqrt(float64(limit)))
	for x := 1; x <= l; x++ {
		for y := 1; y <= l; y++ {
			n := 4*x*x + y*y
			m := n % 12
			if n <= limit && (m == 1 || m == 5) {
				prime[n] = !prime[n]
			}

			n = 3*x*x + y*y
			m = n % 12
			if n <= limit && m == 7 {
				prime[n] = !prime[n]
			}

			n = 3*x*x - y*y
			m = n % 12
			if x > y && n <= limit && m == 11 {
				prime[n] = !prime[n]
			}
		}
	}

	for n := 5; n <= l; n++ {
		if prime[n] {
			t := n * n
			for i, k := 2, t; k < limit; k, i = t*i, i+1 {
				prime[k] = false
			}
		}
	}

	prime[2] = true
	prime[3] = true
	for i := 2; i <= limit; i++ {
		if prime[i] {
			fmt.Println(i)
		}
	}
}
