// +build linux

// ported from toybox
package main

import (
	"fmt"
	"os"
	"syscall"
	"unsafe"
)

func main() {
	fmt.Fprintf(getty(), "\033c")
}

func getty() *os.File {
	for i := 0; i < 3; i++ {
		j := (i + 1) % 3
		if isatty(j) {
			switch j {
			case 0:
				return os.Stdin
			case 1:
				return os.Stdout
			case 2:
				return os.Stderr
			}
		}
	}

	f, err := os.Open("/dev/tty")
	if err != nil {
		fmt.Fprintln(os.Stderr, "reset:", err)
		os.Exit(1)
	}
	return f
}

func isatty(fd int) bool {
	var termios syscall.Termios
	_, _, err := syscall.Syscall6(syscall.SYS_IOCTL, uintptr(fd), syscall.TCGETS, uintptr(unsafe.Pointer(&termios)), 0, 0, 0)
	return err == 0
}
