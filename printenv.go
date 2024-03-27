// ported from suckless
package main

import (
	"flag"
	"fmt"
	"os"
)

func main() {
	flag.Usage = usage
	flag.Parse()

	status := 0
	if flag.NArg() == 0 {
		for _, env := range os.Environ() {
			fmt.Println(env)
		}
	} else {
		for _, key := range flag.Args() {
			if env := os.Getenv(key); env != "" {
				fmt.Println(env)
			} else {
				status = 1
			}
		}
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: variable ...")
	flag.PrintDefaults()
	os.Exit(2)
}
