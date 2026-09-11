// ported from unix v10

package main

import "os"

func main() {
	if len(os.Args) != 3 {
		os.Exit(1)
	}

	x, err := os.Stat(os.Args[1])
	if err != nil {
		os.Exit(1)
	}

	y, err := os.Stat(os.Args[2])
	if err != nil {
		os.Exit(0)
	}

	p := x.ModTime()
	q := y.ModTime()
	if p.Before(q) {
		os.Exit(1)
	}
}
