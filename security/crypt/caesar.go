package main

import (
	"bufio"
	"flag"
	"fmt"
	"os"
	"strings"
)

const (
	Alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	alpha = "abcdefghijklmnopqrstuvwxyz"
)

func main() {
	var shift int
	flag.IntVar(&shift, "s", 13, "shift")
	flag.Usage = usage
	flag.Parse()

	var r *bufio.Reader
	if flag.NArg() < 1 {
		r = bufio.NewReader(os.Stdin)
	} else {
		line := strings.Join(flag.Args(), " ")
		r = bufio.NewReader(strings.NewReader(line))
	}

	if shift %= 26; shift < 0 {
		shift += 26
	}
	for {
		ch, _, err := r.ReadRune()
		if err != nil {
			break
		}

		switch {
		case 'A' <= ch && ch <= 'Z':
			ch = rune(Alpha[(int(ch)-'A'+shift)%len(Alpha)])
		case 'a' <= ch && ch <= 'z':
			ch = rune(alpha[(int(ch)-'a'+shift)%len(alpha)])
		}
		fmt.Printf("%c", ch)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: caesar [-s] <string>")
	flag.PrintDefaults()
	os.Exit(2)
}
