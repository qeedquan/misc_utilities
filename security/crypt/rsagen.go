package main

import (
	"crypto/rand"
	"crypto/rsa"
	"flag"
	"fmt"
	"log"
)

var (
	nprimes = flag.Int("nprimes", 2, "number of primes")
	nbits   = flag.Int("bits", 2048, "RSA keypair size in bits")
)

func main() {
	log.SetPrefix("rsagen: ")
	log.SetFlags(0)

	flag.Parse()

	p, err := rsa.GenerateMultiPrimeKey(rand.Reader, *nprimes, *nbits)
	if err != nil {
		log.Fatalf("%v", err)
	}

	fmt.Printf("N=%v\n", p.N)
	fmt.Printf("E=%v\n", p.E)
	fmt.Printf("D=%v\n", p.D)
	for i, r := range p.Primes {
		fmt.Printf("P%d=%v\n", i+1, r)
	}
}
