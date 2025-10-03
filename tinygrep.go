package main

import (
	"bufio"
	"bytes"
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"path/filepath"
	"regexp"
	"strings"
	"unicode"
)

var (
	cflag = flag.Bool("c", false, "print count matching lines")
	iflag = flag.Bool("i", false, "case sensitive")
	nflag = flag.Bool("n", false, "print line number")
	qflag = flag.Bool("q", false, "silent mode")
	Rflag = flag.Bool("R", false, "recursive")

	status = 0
	found  = false
	many   = false
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("tinygrep: ")

	flag.Usage = usage
	flag.Parse()
	args := flag.Args()
	if len(args) < 1 {
		usage()
	}

	expr := fmt.Sprintf(".*%s.*", args[0])
	if *iflag {
		expr = "(?i)" + expr
	}
	re, err := regexp.Compile(expr)
	ck(err)

	switch {
	case len(args) == 1:
		ck(grep(re, "-", os.Stdin))

	default:
		if *Rflag || len(args) > 2 {
			many = true
		}

		for _, name := range args[1:] {
			ek(grepfile(re, name))
		}
	}

	if !found && status == 0 {
		status |= 1
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] pattern file ...")
	flag.PrintDefaults()
	os.Exit(1)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func ek(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "tinygrep:", err)
		status = 2
	}
}

func fgetln(b *bufio.Reader) (string, error) {
	p := new(bytes.Buffer)
	for {
		r, _, err := b.ReadRune()
		if err != nil {
			return p.String(), err
		}

		p.WriteRune(r)
		if r == '\n' {
			break
		}
	}

	return p.String(), nil
}

func grepfile(re *regexp.Regexp, name string) error {
	f, err := os.Open(name)
	if err != nil {
		return err
	}
	defer f.Close()

	fi, err := f.Stat()
	if err != nil {
		return nil
	}

	if fi.IsDir() && *Rflag {
		err = filepath.Walk(name, func(name string, fi os.FileInfo, _ error) error {
			if fi.IsDir() {
				return nil
			}
			grepfile(re, name)
			return nil
		})
	} else {
		err = grep(re, name, f)
	}
	return err
}

func grep(re *regexp.Regexp, name string, r io.Reader) error {
	b := bufio.NewReader(r)
	c := uint64(0)
	for n := uint64(1); ; n++ {
		ln, err := fgetln(b)
		m := re.FindAllString(ln, -1)

		if len(m) > 0 {
			if *qflag {
				os.Exit(status)
			}
			if *cflag {
				c += uint64(len(m))
			}
			found = true
		}

		if !*cflag {
			for _, s := range m {
				if *nflag {
					s = fmt.Sprintf("%d:%s", n, s)
				}
				if many {
					s = fmt.Sprintf("%s:%s", name, s)
				}

				if strings.IndexFunc(s, isNonPrint) >= 0 {
					fmt.Println("Binary file", name, "matches")
					return nil
				} else {
					fmt.Println(s)
				}
			}
		}

		if err == io.EOF {
			break
		}

		if err != nil {
			return err
		}
	}

	if *cflag {
		s := fmt.Sprintf("%d", c)
		if many {
			s = fmt.Sprintf("%s:%s", name, s)
		}
		fmt.Println(s)
	}

	return nil
}

func isNonPrint(r rune) bool {
	return !unicode.IsSpace(r) && !unicode.IsPrint(r)
}
