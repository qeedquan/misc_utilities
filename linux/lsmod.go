// +build linux

// based on linux lsmod
package main

import (
	"bufio"
	"flag"
	"fmt"
	"os"
	"strings"
)

func main() {
	flag.Parse()

	f, err := os.Open("/proc/modules")
	ck(err)
	defer f.Close()

	fmt.Printf("%-32s %8s   Used by\n", "Module", "Size")
	s := bufio.NewScanner(f)
	for {
		if !s.Scan() {
			break
		}
		line := s.Text()
		line = strings.Replace(line, "\t", " ", -1)
		line = strings.Replace(line, "\n", " ", -1)
		line = strings.TrimSpace(line)

		toks := strings.Split(line, " ")
		if len(toks) < 2 {
			continue
		}

		fmt.Printf("%-32s", toks[0])
		fmt.Printf(" %8s", toks[1])
		if len(toks) >= 3 {
			fmt.Printf(" %4s", toks[2])

			if len(toks) >= 4 {
				use := toks[3]
				if strings.Index(use, ",") >= 0 {
					use = strings.TrimRight(use, ",")
				}
				if use == "-" || strings.HasPrefix(use, "[") {
					use = ""
				}
				fmt.Printf(" %s", use)
			}
		}
		fmt.Printf("\n")
	}
}

func ck(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "lsmod:", err)
		os.Exit(1)
	}
}
