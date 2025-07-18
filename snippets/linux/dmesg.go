// ported from suckless

package main

import (
	"flag"
	"fmt"
	"os"
	"syscall"
)

const (
	SYSLOG_ACTION_READ_ALL      = 3
	SYSLOG_ACTION_CLEAR         = 5
	SYSLOG_ACTION_CONSOLE_LEVEL = 8
	SYSLOG_ACTION_SIZE_BUFFER   = 10
)

var (
	clear = flag.Bool("C", false, "clear buffer")
	cflag = flag.Bool("c", false, "clear buffer after printing out contents")
	level = flag.Int("n", 0, "console level")
)

func main() {
	flag.Usage = usage
	flag.Parse()

	flag.Visit(func(f *flag.Flag) {
		var err error
		switch f.Name {
		case "C":
			_, err = syscall.Klogctl(SYSLOG_ACTION_CLEAR, nil)
		case "n":
			_, _, e := syscall.Syscall(syscall.SYS_SYSLOG, SYSLOG_ACTION_CONSOLE_LEVEL, uintptr(0), uintptr(*level))
			if e != 0 {
				err = syscall.Errno(e)
			}
		}
		xk(err)
	})

	n, err := syscall.Klogctl(SYSLOG_ACTION_SIZE_BUFFER, nil)
	ck(err)

	buf := make([]byte, n)
	n, err = syscall.Klogctl(SYSLOG_ACTION_READ_ALL, buf)
	ck(err)

	buf = buf[:n]
	if len(buf) > 0 && buf[len(buf)-1] != '\n' {
		buf = append(buf, '\n')
	}
	fmt.Printf("%s", buf)

	if *cflag {
		_, err = syscall.Klogctl(SYSLOG_ACTION_CLEAR, nil)
		ck(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options]")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "dmesg:", err)
		os.Exit(1)
	}
}

func xk(err error) {
	ck(err)
	os.Exit(0)
}
