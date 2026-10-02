package main

import (
	"flag"
	"fmt"
	"os"
	"path/filepath"
	"syscall"
	"unsafe"
)

var (
	silent = flag.Bool("s", false, "silent mode")
)

func main() {
	flag.Usage = usage
	flag.Parse()

	status := 0
	tty, err := ttyname(0)
	if err != nil {
		if !*silent {
			fmt.Fprintln(os.Stderr, "tty:", err)
		}
		status = 1
	} else if !*silent {
		fmt.Println(tty)
	}

	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options]")
	flag.PrintDefaults()
	os.Exit(1)
}

func ttyname(fd int) (string, error) {
	if !isatty(fd) {
		return "", fmt.Errorf("not a tty")
	}

	var st syscall.Stat_t
	err := syscall.Fstat(fd, &st)
	if err != nil {
		return "", err
	}

	if st.Mode&syscall.S_IFCHR == 0 {
		return "", fmt.Errorf("not a character device")
	}

	f, err := os.Open("/dev/pts")
	if err != nil {
		return "", err
	}
	defer f.Close()

	for {
		fis, err := f.Readdir(16)
		if err != nil {
			break
		}

		for _, fi := range fis {
			var ds syscall.Stat_t
			name := filepath.Join("/dev/pts", fi.Name())
			err := syscall.Stat(name, &ds)
			if err != nil {
				continue
			}

			if ds.Mode&syscall.S_IFCHR != 0 && ds.Rdev == st.Rdev {
				return name, nil
			}
		}
	}

	return "", fmt.Errorf("unable to lookup tty name")
}

func isatty(fd int) bool {
	var termios syscall.Termios
	_, _, err := syscall.Syscall6(syscall.SYS_IOCTL, uintptr(fd), syscall.TCGETS, uintptr(unsafe.Pointer(&termios)), 0, 0, 0)
	return err == 0
}
