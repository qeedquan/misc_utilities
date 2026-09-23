// +build unix

// ported from suckless
package main

import (
	"flag"
	"fmt"
	"os"
	"os/exec"
	"syscall"
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}
	args := flag.Args()

	ck(syscall.Chroot(args[0]))
	ck(syscall.Chdir("/"))

	var cmd *exec.Cmd
	if len(args) == 1 {
		shell := os.Getenv("SHELL")
		if shell == "" {
			shell = "/bin/sh"
		}
		cmd = exec.Command(shell, "-i")
	} else {
		cmd = exec.Command(args[1], args[2:]...)
	}
	cmd.Stdin = os.Stdin
	cmd.Stderr = os.Stderr
	cmd.Stdout = os.Stdout
	ck(cmd.Run())
}

func ck(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "chroot:", err)
		os.Exit(1)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: dir [cmd [arg ...]]")
	flag.PrintDefaults()
	os.Exit(2)
}
