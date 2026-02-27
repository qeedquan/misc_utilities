// +build linux

// ported from suckless
package main

import (
	"flag"
	"fmt"
	"os"
	"syscall"
	"time"
)

var (
	aflag = flag.Bool("a", false, "change only the access time")
	cflag = flag.Bool("c", false, "do not create any files")
	mflag = flag.Bool("m", false, "change only the modification time")
	ref   = flag.String("r", "", "use reference file timestamp")
	times [2]syscall.Timespec

	status int
)

func main() {
	now := time.Now()
	times[0] = syscall.Timespec{now.Unix(), int64(now.Nanosecond())}

	flag.Var(timeFlag('d'), "d", "parse date and use it instead of current time")
	flag.Var(timeFlag('t'), "t", "parse time and use it instead of current time")
	flag.Var(timeFlag('T'), "T", "parse as number of seconds since epoch")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	if !*aflag && !*mflag {
		*aflag, *mflag = true, true
	}

	if *ref != "" {
		st, err := stat(*ref)
		ck(err)
		times[0] = st.Atim
		times[1] = st.Mtim
	} else {
		times[1] = times[0]
	}

	for _, name := range flag.Args() {
		touch(name)
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: touch [-acm] [-d time | -r ref_file | -t time | -T time] file ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if ek(err) {
		os.Exit(status)
	}
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "touch:", err)
		status |= 1
		return true
	}
	return false
}

func stat(name string) (*syscall.Stat_t, error) {
	fi, err := os.Stat(name)
	if err != nil {
		return nil, err
	}

	st, ok := fi.Sys().(*syscall.Stat_t)
	if !ok {
		return nil, fmt.Errorf("invalid stat type")
	}

	return st, nil
}

func touch(name string) {
	st, err := stat(name)
	if err != nil {
		if !os.IsNotExist(err) {
			ek(err)
			return
		}

		if *cflag {
			return
		}
	} else {
		if !*aflag {
			times[0] = st.Atim
		}
		if !*mflag {
			times[1] = st.Mtim
		}
		err = syscall.UtimesNano(name, []syscall.Timespec{times[0], times[1]})
		ek(err)
		return
	}

	f, err := os.OpenFile(name, os.O_CREATE|os.O_EXCL, 0644)
	if ek(err) {
		return
	}
	f.Close()

	touch(name)
}

type timeFlag rune

func (t timeFlag) String() string { return "" }
func (t timeFlag) Set(s string) error {
	switch t {
	case 'd':
		fallthrough
	case 't':
		t, err := time.Parse(time.RFC3339Nano, s)
		ck(err)
		times[0] = syscall.Timespec{t.Unix(), int64(t.Nanosecond())}
	case 'T':
		d, err := time.ParseDuration(s)
		ck(err)
		times[0].Sec = int64(d)
	}
	return nil
}
