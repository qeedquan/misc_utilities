// +build linux

// ported from suckless
package main

import (
	"flag"
	"fmt"
	"os"
	"syscall"
)

var (
	pflag = flag.Bool("p", false, "poweroff")
	rflag = flag.Bool("r", false, "reboot")
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 0 {
		usage()
	}

	syscall.Sync()

	if *pflag && *rflag {
		usage()
	}

	cmd := syscall.LINUX_REBOOT_CMD_HALT
	if *pflag {
		cmd = syscall.LINUX_REBOOT_CMD_POWER_OFF
	}
	if *rflag {
		cmd = syscall.LINUX_REBOOT_CMD_RESTART
	}

	ck(syscall.Reboot(cmd))
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: halt -p | -r")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
}
