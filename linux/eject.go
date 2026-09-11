// +build linux

// ported from suckless
package main

import (
	"flag"
	"fmt"
	"os"
	"syscall"
	"unsafe"
)

const (
	OPEN_TRAY  = 0x5309
	CLOSE_TRAY = 0x5319
)

var (
	tflag = flag.Bool("t", false, "close tray")

	status = 0
)

func main() {
	flag.Usage = usage
	flag.Parse()
	for _, name := range flag.Args() {
		eject(name)
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [-t] devname ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func eject(name string) {
	fd, err := syscall.Open(name, syscall.O_RDONLY|syscall.O_NONBLOCK, 0644)
	if ek(err) {
		return
	}
	defer syscall.Close(fd)

	mode := OPEN_TRAY
	if *tflag {
		mode = CLOSE_TRAY
	}

	var out int32
	_, _, e := syscall.Syscall(syscall.SYS_IOCTL, uintptr(fd), uintptr(mode), uintptr(unsafe.Pointer(&out)))
	if e != 0 {
		ek(syscall.Errno(e))
	}
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "eject:", err)
		status |= 1
		return true
	}
	return false
}
