// ported from tom duff ncsl
// http://iq0.com/duffgram/ncsl.c

/*
 * ncsl -- count non-comment source lines
 * Deletes comments, white space and braces then counts non-empty lines.
 * Flag -t causes { and } not to be ignored.
 * Flag -b causes C++ comments //...\n to be ignored
 */
package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"os"
)

var (
	tflag = flag.Bool("t", false, "don't ignore trivial lines")
	bflag = flag.Bool("b", false, "ignore bjarne-style comments")

	status = 0
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		fmt.Println(count(os.Stdin))
	} else {
		total := uint64(0)
		for _, name := range flag.Args() {
			f, err := os.Open(name)
			if ek(err) {
				continue
			}
			n := count(f)
			f.Close()

			if flag.NArg() == 1 {
				fmt.Println(n)
			} else {
				fmt.Printf("%s: %d\n", name, n)
				total += n
			}
		}
		if flag.NArg() > 1 {
			fmt.Printf("total %d\n", total)
		}
	}
	os.Exit(status)
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "ncsl:", err)
		status = 1
		return true
	}
	return false
}

func usage() {
	fmt.Fprintln(os.Stderr, "[options] [file] ...")
	flag.PrintDefaults()
	os.Exit(1)
}

func count(r io.Reader) uint64 {
	b := bufio.NewReader(r)
	nline := uint64(0)
	lastc := getnwnc(b)
	for {
		c := getnwnc(b)
		if c == eof {
			break
		}
		if c == '\n' && lastc != '\n' {
			nline++
		}
		lastc = c
	}
	return nline
}

func getnwnc(b *bufio.Reader) rune {
	var c rune
	for {
		c = getnc(b)
		if !(c == ' ' || c == '\t' || (!*tflag && (c == '{' || c == '}'))) {
			break
		}
	}
	return c
}

func getnc(b *bufio.Reader) rune {
	for {
		c := getc(b)
		if c != '/' {
			return c
		}
		c = getc(b)

		if *bflag && c == '/' {
			for {
				c = getc(b)
				if c == eof {
					return eof
				}
				if c == '\n' {
					break
				}
			}
			ungetc(b)
		} else {
			if c != '*' {
				ungetc(b)
				return '/'
			}

			for {
				for {
					c = getc(b)
					if c == eof {
						return eof
					}
					if c == '*' {
						break
					}
				}
				c = getc(b)
				if c == '/' {
					break
				}
				ungetc(b)
			}
		}
	}
}

func getc(b *bufio.Reader) rune {
	c, _, err := b.ReadRune()
	if err != nil {
		return eof
	}
	return c
}

func ungetc(b *bufio.Reader) {
	b.UnreadRune()
}

const eof = -1
