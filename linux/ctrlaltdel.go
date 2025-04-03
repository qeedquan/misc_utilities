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
	hflag = flag.Bool("h", false, "hard reset")
	sflag = flag.Bool("s", false, "soft reset")
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 0 || (*hflag && *sflag) {
		usage()
	}

	cmd := syscall.LINUX_REBOOT_CMD_CAD_OFF
	if *hflag {
		cmd = syscall.LINUX_REBOOT_CMD_CAD_ON
	}
	ck(syscall.Reboot(cmd))
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: ctrlaltdel -h | -s")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
}
