// ported from http://www.johnloomis.org/digitallab/rs232/miftext.html
package main

import (
	"bufio"
	"flag"
	"fmt"
	"os"
)

var (
	status = 0
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		dump(bufio.NewReader(os.Stdin))
	} else {
		for _, name := range flag.Args() {
			f, err := os.Open(name)
			if ek(err) {
				continue
			}
			dump(bufio.NewReader(f))
			f.Close()
		}
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: miftext [options] [file ...]")
	flag.PrintDefaults()
	os.Exit(2)
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "miftext:", err)
		status = 1
		return true
	}
	return false
}

func dump(r *bufio.Reader) {
	const depth = 256
	const width = 8

	w := os.Stdout
	fmt.Fprintf(w, "DEPTH = %d\n", depth)
	fmt.Fprintf(w, "WIDTH = %d\n", width)
	fmt.Fprintf(w, "ADDRESS_RADIX = HEX;\n")
	fmt.Fprintf(w, "DATA_RADIX = HEX;\n")
	fmt.Fprintf(w, "CONTENTS\n\nBEGIN\n")
	fmt.Fprintf(w, "[0..%x] : 0;\n", depth-1)

	addr := uint64(0)
	off := uint64(0)
	for {
		b, err := r.ReadByte()
		if err != nil {
			break
		}
		if b == '\n' {
			for off < 16 {
				fmt.Fprintf(w, "%04x : %02x;\n", addr, ' ')
				addr++
				off++
			}
			off = 0
		} else {
			c := b
			if c < 32 {
				c = 32
			}
			fmt.Fprintf(w, "%04x : %02x; %% %c %%\n", addr, b, c)
			addr++
			off++
		}
		if addr >= depth {
			break
		}
	}
	fmt.Fprintf(w, "END;\n")
}
