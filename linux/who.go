// +build linux

// ported from OpenBSD who
package main

import (
	"bufio"
	"bytes"
	"encoding/binary"
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"os/user"
	"path/filepath"
	"strings"
	"syscall"
	"time"
	"unsafe"
)

var (
	onlyCurrentTerm = flag.Bool("m", false, "show info about the current terminal only")
	showTerm        = flag.Bool("T", false, "show term state")
	showIdle        = flag.Bool("u", false, "show idle time")
	showLabels      = flag.Bool("H", false, "show column labels")
	showQuick       = flag.Bool("q", false, "show names only")
)

const (
	NAME_WIDTH = 8
	HOST_WIDTH = 45
)

var (
	hostWidth = HOST_WIDTH
	myTTY     string
	now       = time.Now()
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("who: ")
	flag.Usage = usage
	flag.Parse()

	myTTY, _ = ttyname(0)
	myTTY = filepath.Base(myTTY)

	if *showQuick {
		*onlyCurrentTerm = false
		*showTerm = false
		*showIdle = false
		*showLabels = false
	}

	if *showTerm {
		hostWidth -= 2
	}
	if *showIdle {
		hostWidth -= 6
	}

	if *showLabels {
		outputLabels()
	}

	var f *os.File
	if flag.NArg() == 1 {
		f = file(flag.Arg(0))
	} else {
		f = file(PATH_UTMP)
	}
	defer f.Close()

	r := bufio.NewReader(f)

	switch flag.NArg() {
	case 0, 1:
		switch {
		case *onlyCurrentTerm:
			whoami(r)
		case *showQuick:
			us := readUtmp(r)
			count := 0
			for _, u := range us {
				if u.User[0] != 0 && u.Line[0] != 0 {
					name := string(u.User[:])
					name = strings.TrimRight(name, "\x00")
					fmt.Printf("%-*.*s ", NAME_WIDTH, UT_NAMESIZE, name)

					if count++; count%8 == 0 {
						fmt.Println()
					}
				}
			}

			if count%8 != 0 {
				fmt.Println()
			}

			fmt.Printf("# users=%d\n", count)

		default:
			us := readUtmp(r)
			for _, u := range us {
				if flag.NArg() == 0 && u.User[0] != 0 && u.Line[0] != 0 {
					output(&u)
				} else if flag.NArg() == 1 {
					output(&u)
				}
			}
		}

	case 2:
		whoami(r)

	default:
		usage()
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options]")
	flag.PrintDefaults()
	os.Exit(1)
}

func file(name string) *os.File {
	f, err := os.Open(name)
	if err != nil {
		log.Fatal(err)
	}
	return f
}

func whoami(r io.Reader) {
	var u utmp

	if myTTY != "" {
		us := readUtmp(r)
		for _, u = range us {
			if u.User[0] != 0 && bytes.Compare(u.Line[:], []byte(myTTY)) == 0 {
				output(&u)
				return
			}
		}
		copy(u.Line[:], myTTY)
	} else {
		copy(u.Line[:], "tty??")
	}

	user, err := user.Current()
	if err == nil {
		copy(u.User[:], user.Name)
	}
	u.Sec = 0
	u.Usec = 0
	copy(u.Host[:], "\x00")
	output(&u)
}

func output(u *utmp) {
	var idle time.Duration

	line := strings.TrimRight(string(u.Line[:]), "\x00")
	name := strings.TrimRight(string(u.User[:]), "\x00")
	host := strings.TrimRight(string(u.Host[:]), "\x00")

	state := '?'
	if *showTerm || *showIdle {
		line = filepath.Join(PATH_DEV, line)
		fi, err := os.Stat(line)
		if err == nil {
			if fi.Mode()&020 != 0 {
				state = '+'
			} else {
				state = '-'
			}

			if st, ok := fi.Sys().(*syscall.Stat_t); ok {
				idle = now.Sub(time.Unix(st.Atim.Sec, st.Atim.Nsec))
			}
		}
	}

	fmt.Printf("%-*.*s ", NAME_WIDTH, UT_NAMESIZE, name)
	if *showTerm {
		fmt.Printf("%c ", state)
	}

	fmt.Printf("%-*.*s", UT_LINESIZE, UT_LINESIZE, line)

	var t time.Time
	if u.Sec == 0 && u.Usec == 0 {
		t = time.Now()
	} else {
		t = time.Unix(int64(u.Sec), int64(u.Usec)*1000)
	}
	fmt.Printf("%.12s", t.Format(time.ANSIC)[4:])

	if *showIdle {
		switch {
		case idle < 60*time.Second:
			fmt.Printf("  .   ")
		case idle < 24*60*60*time.Second:
			fmt.Printf("%02d:%02d", idle/(60*60*time.Second), (idle%(60*60*time.Second))/(60*time.Second))
		default:
			fmt.Printf(" old  ")
		}
	}

	if host != "" {
		fmt.Printf("  (%.*s)", hostWidth, host)
	}
	fmt.Println()
}

func outputLabels() {
	fmt.Printf("%-*.*s ", NAME_WIDTH, UT_NAMESIZE, "USER")

	if *showTerm {
		fmt.Printf("S ")
	}

	fmt.Printf("%-*.*s", UT_LINESIZE, UT_LINESIZE, "LINE")
	fmt.Printf("WHEN         ")

	if *showIdle {
		fmt.Printf("IDLE  ")
	}

	fmt.Printf("  %.*s", hostWidth, "FROM")
	fmt.Println()
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

const (
	PATH_UTMP = "/var/run/utmp"
	PATH_DEV  = "/dev/"
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

func readUtmp(r io.Reader) []utmp {
	var us []utmp
	var u utmp
	for {
		err := binary.Read(r, binary.LittleEndian, &u)
		if err != nil {
			break
		}
		us = append(us, u)
	}
	return us
}
