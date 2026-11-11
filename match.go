// ported from http://cellperformance-snippets.googlecode.com/files/match.c
package main

import (
	"bufio"
	"bytes"
	"flag"
	"fmt"
	"io"
	"os"
)

var (
	hex = flag.Bool("h", false, "output hex")
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 2 {
		usage()
	}

	source := open(flag.Arg(0))
	defer source.Close()
	uniq := open(flag.Arg(1))
	defer uniq.Close()

	for {
		index := uint64(0)
		found := false

		line, err := source.Line()
		if err != nil {
			break
		}

		for {
			xline, err := uniq.Line()
			if err != nil {
				break
			}

			if line == xline {
				if *hex {
					fmt.Printf("%#08x\n", index)
				} else {
					fmt.Printf("%d\n", index)
				}
				found = true
				break
			}

			index++
		}

		if !found {
			fmt.Println("NO MATCH")
		}

		uniq.Reset()
	}
}

func open(name string) *Reader {
	f, err := os.Open(name)
	ck(err)
	return &Reader{File: f, Reader: bufio.NewReader(f)}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: match [-h] <source_file> <uniq_file>")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "match:", err)
		os.Exit(1)
	}
}

type Reader struct {
	*os.File
	*bufio.Reader
	err error
}

func (r *Reader) Reset() {
	r.File.Seek(0, io.SeekStart)
	r.Reader.Reset(r.File)
}

func (r *Reader) Line() (string, error) {
	if r.err != nil {
		return "", r.err
	}

	var buf bytes.Buffer
	for {
		ch, _, err := r.ReadRune()
		if err != nil {
			str := buf.String()
			if str == "" {
				return str, err
			}
			r.err = err
			return str, nil
		}
		buf.WriteRune(ch)
		if ch == '\n' {
			break
		}
	}
	return buf.String(), nil
}
