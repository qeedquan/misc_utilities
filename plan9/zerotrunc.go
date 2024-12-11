// ported from 9front
package main

import (
	"bytes"
	"os"
)

func main() {
	var buf [8192]byte
	for {
		n, err := os.Stdin.Read(buf[:])
		if err != nil {
			break
		}

		p := buf[:n]
		i := bytes.IndexByte(p, 0)
		if i > 0 {
			os.Stdout.Write(p[:i])
			break
		}
		os.Stdout.Write(p)
	}
}
