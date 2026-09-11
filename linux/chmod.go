// ported from plan9 chmod

package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"strconv"
)

var (
	status = 0
)

func main() {
	log.SetFlags(0)
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 2 {
		usage()
	}

	var mode, mask os.FileMode
	n, err := strconv.ParseInt(flag.Arg(0), 8, 64)
	if err == nil {
		mode = os.FileMode(n)
		mask = os.ModePerm
	} else {
		mask, mode, err = parseMode(flag.Arg(0))
		if err != nil {
			log.Fatal("chmod: ", err)
		}
	}

	args := flag.Args()
	for _, name := range args[1:] {
		fi, err := os.Stat(name)
		if ck(err) {
			continue
		}

		err = os.Chmod(name, (fi.Mode()&^mask)|(mode&mask))
		ck(err)
	}

	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: chmod [options] mode[,mode] ... file ...")
	fmt.Fprintln(os.Stderr, "   or: chmod [options] octal-mode file ...")
	flag.PrintDefaults()
	os.Exit(1)
}

func ck(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "chmod:", err)
		status = 1
		return true
	}
	return false
}

func parseMode(spec string) (mode, mask os.FileMode, err error) {
	xspec := spec
	xerr := fmt.Errorf("invalid mode: %q", xspec)

	mask = os.ModeAppend | os.ModeExclusive | os.ModeTemporary
loop:
	for ; len(spec) > 0; spec = spec[1:] {
		switch spec[0] {
		case 'u':
			mask |= 0700
		case 'g':
			mask |= 0070
		case 'o':
			mask |= 0007
		case 'a':
			mask |= 0777
		default:
			break loop
		}
	}

	if len(spec) == 0 {
		return 0, 0, xerr
	}

	if len(spec) == len(xspec) {
		mask |= 0777
	}

	op := spec[0]
	if op != '+' && op != '-' && op != '=' {
		return 0, 0, xerr
	}

	for spec = spec[1:]; len(spec) > 0; spec = spec[1:] {
		switch spec[0] {
		case 'r':
			mode |= 0444
		case 'w':
			mode |= 0222
		case 'x':
			mode |= 0111
		case 'a':
			mode |= os.ModeAppend
		case 'l':
			mode |= os.ModeExclusive
		case 't':
			mode |= os.ModeTemporary
		default:
			return 0, 0, xerr
		}
	}

	if op == '+' || op == '-' {
		mask &= mode
	}
	if op == '-' {
		mode = ^mode
	}

	return
}
