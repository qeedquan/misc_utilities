// +build linux
// ported from suckless

package main

import (
	"flag"
	"fmt"
	"os"
	"os/exec"
	"syscall"
)

var (
	mflag = flag.Bool("m", false, "namespace")
	uflag = flag.Bool("u", false, "uts")
	iflag = flag.Bool("i", false, "ipc")
	nflag = flag.Bool("n", false, "net")
	pflag = flag.Bool("p", false, "pid")
	Uflag = flag.Bool("U", false, "new user")
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	var flags int
	if *mflag {
		flags |= syscall.CLONE_NEWNS
	}
	if *uflag {
		flags |= syscall.CLONE_NEWUTS
	}
	if *iflag {
		flags |= syscall.CLONE_NEWIPC
	}
	if *nflag {
		flags |= syscall.CLONE_NEWNET
	}
	if *pflag {
		flags |= syscall.CLONE_NEWPID
	}
	if *Uflag {
		flags |= syscall.CLONE_NEWUSER
	}

	ck(syscall.Unshare(flags))

	args := flag.Args()
	cmd := exec.Command(args[0], args[1:]...)
	cmd.Stdin = os.Stdin
	cmd.Stderr = os.Stderr
	cmd.Stdout = os.Stdout
	ck(cmd.Run())
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] command")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "unshare:", err)
		os.Exit(1)
	}
}
