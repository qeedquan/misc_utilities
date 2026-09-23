package main

import (
	"bufio"
	"flag"
	"fmt"
	"os"
	"strings"
)

func main() {
	flag.Usage = usage
	flag.Parse()

	var r *bufio.Reader
	if flag.NArg() < 1 {
		r = bufio.NewReader(os.Stdin)
	} else {
		line := strings.Join(flag.Args(), " ")
		r = bufio.NewReader(strings.NewReader(line))
	}

	for {
		ch, _, err := r.ReadRune()
		if err != nil {
			break
		}

		switch {
		case 'A' <= ch && ch <= 'M',
			'a' <= ch && ch <= 'm':
			ch += 13
		case 'N' <= ch && ch <= 'Z',
			'n' <= ch && ch <= 'z':
			ch -= 13
		}
		fmt.Printf("%c", ch)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: rot13 <string>")
	flag.PrintDefaults()
	os.Exit(2)
}
