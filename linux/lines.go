// ported from 9front

package main

import (
	"bufio"
	"bytes"
	"flag"
	"io"
	"log"
	"os"
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("lines: ")
	flag.Parse()

	if flag.NArg() == 0 {
		cat(os.Stdin, "<stdin>")
	} else {
		for _, name := range flag.Args() {
			f, err := os.Open(name)
			ck(err)
			cat(f, name)
			f.Close()
		}
	}
}

func cat(f *os.File, name string) {
	r := bufio.NewReader(f)
	for {
		line, err := fgetln(r)
		if err == io.EOF {
			break
		}
		xk(name, err)

		_, err = os.Stdout.Write(line)
		xk(name, err)
	}
}

func fgetln(r *bufio.Reader) ([]byte, error) {
	b := new(bytes.Buffer)
	for {
		c, _, err := r.ReadRune()
		if err != nil {
			return b.Bytes(), err
		}

		b.WriteRune(c)
		if c == '\n' {
			return b.Bytes(), nil
		}
	}
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func xk(name string, err error) {
	if err != nil {
		log.Fatalf("%s: %v", name, err)
	}
}
