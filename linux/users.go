// +build linux

// ported from OpenBSD users

package main

import (
	"bufio"
	"encoding/binary"
	"flag"
	"fmt"
	"io"
	"os"
	"sort"
	"strings"
)

func main() {
	flag.Usage = usage
	flag.Parse()

	var err error
	switch flag.NArg() {
	case 0:
		err = dump(PATH_UTMP)
	case 1:
		err = dump(flag.Arg(0))
	default:
		usage()
	}

	if err != nil {
		fmt.Fprintln(os.Stderr, "utmp:", err)
		os.Exit(1)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] file")
	flag.PrintDefaults()
	os.Exit(1)
}

const (
	PATH_UTMP = "/var/run/utmp"
)

const (
	UT_LINESIZE = 32
	UT_NAMESIZE = 32
	UT_HOSTSIZE = 256
)

type exitStatus struct {
	Termination uint16
	Exit        uint16
}

type utmp struct {
	Type       uint16
	_          [2]uint8
	Pid        uint32
	Line       [UT_LINESIZE]uint8
	ID         [4]uint8
	User       [UT_NAMESIZE]uint8
	Host       [UT_HOSTSIZE]uint8
	ExitStatus exitStatus
	Session    uint32
	Sec        uint32
	Usec       uint32
	Addr6      [4]uint32
	_          [20]uint8
}

func dump(name string) error {
	f, err := os.Open(name)
	if err != nil {
		return err
	}
	defer f.Close()

	r := bufio.NewReader(f)

	var names []string
	for {
		var u utmp
		err := binary.Read(r, binary.LittleEndian, &u)
		if err == io.EOF || err == io.ErrUnexpectedEOF {
			break
		}

		if err != nil {
			return err
		}

		if u.User[0] == 0 {
			continue
		}

		name := string(u.User[:])
		name = strings.TrimRight(name, "\x00")
		names = append(names, name)
	}

	if len(names) > 0 {
		sort.Strings(names)

		fmt.Printf("%.*s", UT_NAMESIZE, names[0])
		xname := names[0]
		for _, name := range names[1:] {
			if xname == name {
				continue
			}
			fmt.Printf(" %.*s", UT_NAMESIZE, name)
			xname = name
		}
		fmt.Println()
	}

	return nil
}
