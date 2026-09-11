// ported from openbsd unexpand

package main

import (
	"bufio"
	"bytes"
	"flag"
	"fmt"
	"os"
)

var (
	aflag = flag.Bool("a", false, "convert all blanks, instead of initial blanks")
)

func main() {
	flag.Usage = usage
	flag.Parse()

	if flag.NArg() < 1 {
		parse("")
	} else {
		for _, name := range flag.Args() {
			err := parse(name)
			if err != nil {
				fmt.Fprintln(os.Stderr, "unexpand:", err)
			}
		}
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [-a] [file ...]")
	flag.PrintDefaults()
	os.Exit(1)
}

func parse(name string) error {
	var f *os.File

	if name == "" {
		f = os.Stdin
	} else {
		var err error

		f, err = os.Open(name)
		if err != nil {
			return err
		}
		defer f.Close()
	}

	s := bufio.NewScanner(f)
	for s.Scan() {
		fmt.Println(tabify(s.Text()))
	}

	return nil
}

func tabify(line string) string {
	buf := new(bytes.Buffer)
	ocol := int(0)
	dcol := int(0)

loop:
	for i, r := range line {
		switch r {
		case ' ':
			dcol++

		case '\t':
			dcol += 8
			dcol &= ^07

		default:
			for (ocol+8) & ^07 <= dcol {
				if ocol+1 == dcol {
					break
				}
				buf.WriteRune('\t')
				ocol += 8
				ocol &= ^07
			}

			for ; ocol < dcol; ocol++ {
				buf.WriteRune(' ')
			}

			if !*aflag {
				for _, r := range line[i:] {
					buf.WriteRune(r)
				}
				break loop
			}

			buf.WriteRune(r)
			ocol++
			dcol++
		}
	}

	return buf.String()
}
