// +build linux

package main

import (
	"flag"
	"fmt"
	"os"
	"strconv"
	"syscall"
)

var (
	mode = modeFlag(0666)
)

func main() {
	flag.Var(&mode, "m", "set mode")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	for _, name := range flag.Args() {
		err := syscall.Mknod(name, syscall.S_IFIFO|uint32(mode), 0)
		if err != nil {
			fmt.Fprintln(os.Stderr, "mkfifo:", err)
		}
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] name ...")
	flag.PrintDefaults()
	os.Exit(1)
}

type modeFlag uint32

func (m *modeFlag) Set(s string) error {
	n, err := strconv.ParseUint(s, 0, 32)
	if err != nil {
		return err
	}
	*m = modeFlag(n)
	return nil
}

func (m modeFlag) String() string {
	return fmt.Sprintf("%#o", m)
}
