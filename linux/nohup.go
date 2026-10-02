// ported from OpenBSD nohup
package main

import (
	"flag"
	"fmt"
	"os"
	"os/exec"
	"os/signal"
	"os/user"
	"path/filepath"
	"syscall"
	"unsafe"
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	if isatty(1) {
		dofile()
	}

	if isatty(2) {
		os.Stderr = os.Stdout
	}

	signal.Ignore(syscall.SIGHUP)

	args := flag.Args()
	cmd := exec.Command(args[0], args[1:]...)
	cmd.Stdin = os.Stdin
	cmd.Stderr = os.Stderr
	cmd.Stdout = os.Stdout

	err := cmd.Run()
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		if _, ok := err.(*exec.ExitError); ok {
			os.Exit(126)
		}
		os.Exit(127)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: nohup utility [arg ...]")
	flag.PrintDefaults()
	os.Exit(127)
}

func isatty(fd int) bool {
	var termios syscall.Termios
	_, _, err := syscall.Syscall6(syscall.SYS_IOCTL, uintptr(fd), syscall.TCGETS, uintptr(unsafe.Pointer(&termios)), 0, 0, 0)
	return err == 0
}

func dofile() {
	name := "nohup.out"
	f, err := os.OpenFile(name, os.O_RDWR|os.O_CREATE|os.O_APPEND, 0644)
	if err != nil {
		user, err := user.Current()
		if err == nil {
			name = filepath.Join(user.HomeDir, name)
			f, err = os.OpenFile(name, os.O_RDWR|os.O_CREATE|os.O_APPEND, 0644)
		}
	}

	if err != nil {
		fmt.Fprintln(os.Stderr, "can't open a nohup.out file")
		os.Exit(127)
	}

	os.Stdout = f
	fmt.Fprintln(os.Stderr, "sending output to", name)
}
