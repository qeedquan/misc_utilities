// ported from 9front
package main

import (
	"flag"
	"os"
	"strconv"
)

var (
	x [1024]byte
	s = "  ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
)

func fill() {
	for i := 0; i < len(x); i += len(s) {
		copy(x[i:], s)
		x[i] = byte(i >> 8)
		x[i+1] = byte(i)
	}
}

func main() {
	flag.Parse()

	i := 2560
	for _, arg := range flag.Args() {
		i, _ = strconv.Atoi(arg)
	}

	fill()
	for ; i > 0; i-- {
		os.Stdout.Write(x[:])
	}
}
