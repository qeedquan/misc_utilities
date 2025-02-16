package main

import (
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"strconv"
	"strings"
)

var (
	offset = flag.Int("o", 0, "binary location offset")

	status = 0
)

func main() {
	log.SetPrefix("binprint: ")
	log.SetFlags(0)

	flag.Usage = usage
	flag.Parse()

	var args []string
	if flag.NArg() < 1 {
		buf, err := io.ReadAll(os.Stdin)
		ck(err)

		str := strings.TrimSpace(string(buf))
		str = strings.Replace(str, "\n", " ", -1)
		args = strings.Split(str, " ")
	} else {
		args = flag.Args()
	}

	for _, a := range args {
		if a == "" {
			continue
		}

		n, err := strconv.ParseUint(a, 0, 64)
		if err != nil {
			n, err = strconv.ParseUint(a, 16, 64)
		}

		if err != nil {
			fmt.Fprintf(os.Stderr, "invalid number %q\n", a)
			status = 1
			continue
		}

		bits := fmt.Sprintf("%b", n)

		fmt.Printf("%#x %d -> %b {\n", n, n, n)
		for i := len(bits) - 1; i >= 0; i-- {
			if bits[i] == '1' {
				fmt.Printf("\t%d -> %c\n", *offset+len(bits)-1-i, bits[i])
			}
		}
		fmt.Printf("}\n")
	}

	os.Exit(status)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: binprint [options] num ...")
	flag.PrintDefaults()
	os.Exit(2)
}
