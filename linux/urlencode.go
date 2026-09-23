// ported from 9front
package main

import (
	"bufio"
	"flag"
	"fmt"
	"os"
	"strings"
)

var (
	dflag = flag.Bool("d", false, "decode mode")
)

func main() {
	flag.Usage = usage
	flag.Parse()

	var fd *os.File
	switch flag.NArg() {
	case 0:
		fd = os.Stdin
	case 1:
		var err error
		fd, err = os.Open(flag.Arg(0))
		ck(err)
		defer fd.Close()

		fi, err := fd.Stat()
		ck(err)
		if fi.IsDir() {
			ck(fmt.Errorf("%v: is a directory", flag.Arg(0)))
		}
	default:
		usage()
	}

	r := bufio.NewReader(fd)
	w := bufio.NewWriter(os.Stdout)
	defer w.Flush()
	if *dflag {
		decode(r, w)
	} else {
		encode(r, w)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: urlencode [-d] [file]")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "urlencode:", err)
		os.Exit(1)
	}
}

const (
	hex = "0123456789abcdef"
	Hex = "0123456789ABCDEF"
)

func hexDigit(c byte) int {
	if i := strings.IndexByte(hex, c); i >= 0 {
		return i
	}

	if i := strings.IndexByte(Hex, c); i >= 0 {
		return i
	}

	return -1
}

func decode(r *bufio.Reader, w *bufio.Writer) {
loop:
	for {
		c, err := r.ReadByte()
		if err != nil {
			break
		}

		switch c {
		case '%':
			c1, err := r.ReadByte()
			if err != nil {
				break loop
			}
			x1 := hexDigit(c1)
			if x1 < 0 {
				r.UnreadByte()
				w.WriteByte(c)
				continue loop
			}

			c2, err := r.ReadByte()
			if err != nil {
				break loop
			}
			x2 := hexDigit(c2)
			if x2 < 0 {
				r.UnreadByte()
				w.WriteByte(c)
				w.WriteByte(c2)
				continue loop
			}
			c = byte(x1)<<4 | byte(x2)

		case '+':
			c = ' '
		}

		w.WriteByte(c)
	}
}

func encode(r *bufio.Reader, w *bufio.Writer) {
	for {
		c, err := r.ReadByte()
		if err != nil {
			break
		}

		switch {
		case strings.IndexByte("/$-_@.!*'(),", c) >= 0,
			'a' <= c && c <= 'z',
			'A' <= c && c <= 'Z',
			'0' <= c && c <= '9':
			w.WriteByte(c)

		case c == ' ':
			w.WriteByte('+')

		default:
			w.WriteByte('%')
			w.WriteByte(Hex[c>>4])
			w.WriteByte(Hex[c&0xf])
		}
	}
}
