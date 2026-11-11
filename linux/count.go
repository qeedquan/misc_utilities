// ported from toybox

package main

import (
	"flag"
	"fmt"
	"io"
	"os"
)

func main() {
	flag.Parse()

	var buf [8192]byte
	var total uint64
	for {
		n, err := os.Stdin.Read(buf[:])
		if n > 0 {
			total += uint64(n)
			os.Stdout.Write(buf[:n])
			fmt.Fprintf(os.Stderr, "%d bytes\r", total)
		}
		if err == io.EOF {
			break
		}
		ck(err)
	}
	fmt.Printf("\n")
}

func ck(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "count:", err)
	}
}
