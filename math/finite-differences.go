package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"os"
)

var (
	status = 0

	order = flag.Uint("n", 4, "calculate differences to nth order")
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		diff(bufio.NewReader(os.Stdin))
	} else {
		for _, name := range flag.Args() {
			f, err := os.Open(name)
			if ek(err) {
				continue
			}
			diff(bufio.NewReader(f))
			f.Close()
		}
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: finite-differences [options]")
	flag.PrintDefaults()
	os.Exit(2)
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "finite-differences:", err)
		status = 1
		return true
	}
	return false
}

func diff(r io.Reader) {
	p := make([][]float64, *order+1)
	for {
		var v float64
		_, err := fmt.Fscan(r, &v)
		if err != nil {
			break
		}
		p[0] = append(p[0], v)
	}

	for i := uint(1); i <= *order; i++ {
		for j := 0; j < len(p[i-1])-1; j++ {
			p[i] = append(p[i], p[i-1][j+1]-p[i-1][j])
		}
	}
	for i := range p {
		fmt.Printf("%10v ", i)
	}
	fmt.Printf("\n\n")

	for i := range p[0] {
		for j := range p {
			if i >= len(p[j]) {
				fmt.Printf("%10.2f", 0.0)
			} else {
				fmt.Printf("%10.2f", p[j][i])
			}
			fmt.Printf(" ")
		}
		fmt.Printf("\n")
	}
}
