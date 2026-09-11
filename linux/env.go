package main

import (
	"flag"
	"fmt"
	"os"
	"os/exec"
	"strings"
)

var (
	status int
)

func main() {
	flag.Usage = usage
	flag.Parse()

	args := flag.Args()
	for ; len(args) > 0; args = args[1:] {
		s := args[0]
		i := strings.IndexRune(s, '=')

		var err error
		if i > 0 {
			err = os.Setenv(s[:i], s[i+1:])
		} else if i == 0 {
			err = os.Setenv(s, "")
		} else {
			break
		}
		ck(err)
	}

	if len(args) == 0 {
		for _, env := range os.Environ() {
			fmt.Println(env)
		}
	} else {
		cmd := exec.Command(args[0], args[1:]...)
		cmd.Stdin = os.Stdin
		cmd.Stdout = os.Stdout
		cmd.Stderr = os.Stderr
		err := cmd.Run()
		ck(err)
	}

	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] [name=value] ... [command [arg ...]]")
	flag.PrintDefaults()
	os.Exit(1)
}

func ck(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		status = 1
	}
}
