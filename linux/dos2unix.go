package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"os"
	"path/filepath"
)

var (
	dflag = flag.Bool("d", false, "unix2dos")

	status = 0
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}
	for _, name := range flag.Args() {
		ek(convert(name))
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: dos2unix [-d] file ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func ek(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "dos2unix:", err)
		status = 1
	}
}

func convert(name string) error {
	f, err := os.Open(name)
	if err != nil {
		return err
	}
	defer f.Close()

	t, err := os.CreateTemp(filepath.Dir(name), filepath.Base(name))
	if err != nil {
		return err
	}

	r := bufio.NewReader(f)
	w := bufio.NewWriter(t)

	for {
		c, err := r.ReadByte()
		if err != nil {
			break
		}

		switch {
		case *dflag && c == '\n':
			w.Write([]byte("\r\n"))

		case *dflag && c == '\r':
			// ignore

		case !*dflag && c == '\r':
			w.WriteByte('\n')
			c, _ = r.ReadByte()
			if c != '\n' {
				w.WriteByte(c)
			}

		default:
			w.WriteByte(c)
		}
	}

	err = pi(err, w.Flush(), t.Close())
	if err != nil {
		os.Remove(t.Name())
		return err
	}

	return os.Rename(t.Name(), name)
}

func pi(errs ...error) error {
	for _, err := range errs {
		if err == nil || err == io.EOF {
			continue
		}
		return err
	}
	return nil
}
