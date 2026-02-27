package main

import (
	"flag"
	"fmt"
	"io"
	"os"
)

func main() {
	flag.Usage = usage
	flag.Parse()

	status := 0
	for _, name := range flag.Args() {
		z, err := check(name)
		if err != nil {
			fmt.Fprintln(os.Stderr, "zerocheck:", err)
			status = 1
			continue
		}

		if z {
			fmt.Println(name)
		} else {
			status = 1
		}
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: file ...")
	os.Exit(2)
}

func check(name string) (bool, error) {
	f, err := os.Open(name)
	if err != nil {
		return false, err
	}
	defer f.Close()

	var b [8192]byte
	for {
		n, err := f.Read(b[:])
		if err != nil && err != io.EOF {
			return false, err
		}

		if n > 0 {
			for i := range b[:n] {
				if b[i] != 0 {
					return false, nil
				}
			}
		}

		if err == io.EOF {
			break
		}
	}

	return true, nil
}
