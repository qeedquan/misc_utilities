// ported from 9front
package main

import (
	"bufio"
	"flag"
	"fmt"
	"log"
	"os"
)

var (
	syntax = flag.String("s", "gnu", "assembly syntax [gnu, plan9]")
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("data2s: ")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 1 {
		usage()
	}

	r := bufio.NewReader(os.Stdin)
	w := bufio.NewWriter(os.Stdout)
	defer w.Flush()

	switch *syntax {
	case "gnu":
		gnu(r, w)
	case "plan9":
		plan9(r, w)
	default:
		log.Fatal("unknown syntax")
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [-s] name")
	flag.PrintDefaults()
	os.Exit(2)
}

func gnu(r *bufio.Reader, w *bufio.Writer) {
	fmt.Fprintf(w, ".data\n")
	fmt.Fprintf(w, ".globl %scode\n", flag.Arg(0))
	fmt.Fprintf(w, ".globl %slen\n", flag.Arg(0))
	fmt.Fprintf(w, "%scode:\n", flag.Arg(0))
	i := uint64(0)
	for ; ; i++ {
		c, err := r.ReadByte()
		if err != nil {
			break
		}
		if i&7 == 0 {
			fmt.Fprintf(w, ".byte")
		} else {
			fmt.Fprintf(w, ",")
		}
		fmt.Fprintf(w, " %#02x", c)
		if i&7 == 7 {
			fmt.Fprintf(w, "\n")
		}
	}
	fmt.Fprintf(w, "\n\n.p2align 2\n%slen:\n", flag.Arg(0))
	fmt.Fprintf(w, ".long %d\n", i)
}

func plan9(r *bufio.Reader, w *bufio.Writer) {
	i := uint64(0)
	for ; ; i++ {
		c, err := r.ReadByte()
		if err != nil {
			break
		}
		if i&7 == 0 {
			fmt.Fprintf(w, "DATA %scode+%d(SB)/8 $\"", flag.Arg(0), i)
		}
		if c != 0 {
			fmt.Fprintf(w, "\\%o", c)
		} else {
			fmt.Fprintf(w, "\\z")
		}
		if i&7 == 7 {
			fmt.Fprintf(w, "\"\n")
		}
	}
	j := i
	if i&7 != 0 {
		for i&7 != 0 {
			fmt.Fprintf(w, "\\z")
			i++
		}
		fmt.Fprintf(w, "\"\n")
	}
	fmt.Fprintf(w, "GLOBL %scode+0(SB), $%d\n", flag.Arg(0), i)
	fmt.Fprintf(w, "GLOBL %slen+0(SB), $4\n", flag.Arg(0))
	fmt.Fprintf(w, "DATA %slen+0(SB)/4, $%d\n", flag.Arg(0), j)
}
