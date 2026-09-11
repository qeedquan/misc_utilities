// +build unix

// ported from suckless
package main

import (
	"flag"
	"fmt"
	"os"
	"os/exec"
	"time"
)

var (
	interval = flag.Duration("n", 2*time.Second, "interval to call command")
)

func main() {
	flag.Usage = usage
	flag.Parse()

	if flag.NArg() < 1 {
		usage()
	}

	args := flag.Args()
	for {
		fmt.Printf("\x1b[2J\x1b[H")
		cmd := exec.Command(args[0], args[1:]...)
		cmd.Stdin = os.Stdin
		cmd.Stderr = os.Stderr
		cmd.Stdout = os.Stdout
		cmd.Run()
		time.Sleep(*interval)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: watch [-n interval] command")
	flag.PrintDefaults()
	os.Exit(2)
}
