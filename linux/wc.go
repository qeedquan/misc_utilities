// ported from plan9 wc

package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"os"
	"unicode"
	"unicode/utf8"
)

var (
	pline = flag.Bool("l", false, "print the newline counts")
	pword = flag.Bool("w", false, "print the word counts")
	prune = flag.Bool("r", false, "print the rune counts")
	pbadr = flag.Bool("b", false, "print the bad rune counts")
	pchar = flag.Bool("c", false, "print the character counts")
)

var (
	nline, nword, nrune, nbadr, nchar      uint64
	tnline, tnword, tnrune, tnbadr, tnchar uint64
)

const (
	space = iota
	word
)

func main() {
	flag.Usage = usage
	flag.Parse()

	flags := false
	flag.Visit(func(*flag.Flag) {
		flags = true
	})
	if !flags {
		*pline = true
		*pword = true
		*pchar = true
	}

	if flag.NArg() == 0 {
		r := bufio.NewReader(os.Stdin)
		wc(r)
		report(nline, nword, nrune, nbadr, nchar, "")
	} else {
		for _, name := range flag.Args() {
			f, err := os.Open(name)
			if err != nil {
				fmt.Fprintln(os.Stderr, "wc:", err)
				continue
			}
			r := bufio.NewReader(f)
			wc(r)
			report(nline, nword, nrune, nbadr, nchar, name)
			f.Close()
		}

		if flag.NArg() > 1 {
			report(tnline, tnword, tnrune, tnbadr, tnchar, "total")
		}
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] file ...")
	flag.PrintDefaults()
	os.Exit(1)
}

func wc(r *bufio.Reader) {
	nchar = 0
	nline = 0
	nword = 0
	nrune = 0
	nbadr = 0
	where := space

	for {
		r, size, err := r.ReadRune()
		if err == io.EOF {
			break
		}

		if err != nil {
			fmt.Fprintln(os.Stderr, "wc:", err)
			break
		}

		nchar += uint64(size)
		nrune++
		if r == utf8.RuneError {
			nbadr++
			continue
		}

		if r == '\n' {
			nline++
		}

		if where == word {
			if unicode.IsSpace(r) {
				where = space
			}
		} else {
			if !unicode.IsSpace(r) {
				where = word
				nword++
			}
		}
	}

	tnline += nline
	tnword += nword
	tnrune += nrune
	tnbadr += nbadr
	tnchar += nchar
}

func report(nline, nword, nrune, nbadr, nchar uint64, name string) {
	var line string

	if *pline {
		line += fmt.Sprintf(" %7d", nline)
	}

	if *pword {
		line += fmt.Sprintf(" %7d", nword)
	}

	if *prune {
		line += fmt.Sprintf(" %7d", nrune)
	}

	if *pbadr {
		line += fmt.Sprintf(" %7d", nbadr)
	}

	if *pchar {
		line += fmt.Sprintf(" %7d", nchar)
	}

	if name != "" {
		line += fmt.Sprintf(" %s", name)
	}

	fmt.Printf("%s\n", line)
}
