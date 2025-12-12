// ported from suckless
package main

import (
	"bufio"
	"flag"
	"fmt"
	"os"
)

var (
	lflag = flag.Bool("l", false, "print number and bytes (in octal) for each difference")
	sflag = flag.Bool("s", false, "run in silent mode")
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 2 || (*lflag && *sflag) {
		usage()
	}
	args := flag.Args()

	var r [2]*bufio.Reader
	for i := range args {
		if args[i] == "-" {
			r[i] = bufio.NewReader(os.Stdin)
		} else {
			f, err := os.Open(args[i])
			ck(err)
			defer f.Close()
			r[i] = bufio.NewReader(f)
		}
	}

	same := 1
	var b [2]byte
	var e [2]error
loop:
	for n, line := uint64(1), uint64(1); ; n++ {
		b[0], e[0] = r[0].ReadByte()
		b[1], e[1] = r[1].ReadByte()

		switch {
		case b[0] == b[1]:
			if e[0] != nil {
				break loop
			} else if b[0] == '\n' {
				line++
			}
			continue

		case e[0] != nil || e[1] != nil:
			if !*sflag {
				fmt.Fprintf(os.Stderr, "cmp: EOF on %s\n", args[pi(e[0], e[1])])
			}
			same = 0
			break loop

		case !*lflag:
			if !*sflag {
				fmt.Fprintf(os.Stderr, "%s %s differ: byte %d, line %d\n",
					args[0], args[1], n, line)
			}
			same = 0

		default:
			fmt.Printf("%d %o %o\n", n, b[0], b[1])
			same = 0
		}
	}
	os.Exit(1 - same)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: cmp [options] file1 file2")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		if !*sflag {
			fmt.Fprintln(os.Stderr, "cmp:", err)
		}
		os.Exit(1)
	}
}

func pi(errs ...error) int {
	for i := range errs {
		if errs[i] != nil {
			return i
		}
	}
	return 0
}
