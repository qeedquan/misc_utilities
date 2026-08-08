// +build linux

package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"os/exec"
	"syscall"
)

var (
	prio = flag.Int("n", 10, "priority level")
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("nice: ")

	flag.Usage = usage
	flag.Parse()

	xprio, err := syscall.Getpriority(syscall.PRIO_PROCESS, 0)
	ck(err)

	if flag.NArg() < 1 {
		fmt.Println(xprio)
		os.Exit(0)
	}

	err = syscall.Setpriority(syscall.PRIO_PROCESS, 0, *prio+xprio)
	ck(err)

	args := flag.Args()
	cmd := exec.Command(args[0], args[1:]...)
	cmd.Stdin = os.Stdin
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	ck(cmd.Run())
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] command args...")
	os.Exit(1)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}
