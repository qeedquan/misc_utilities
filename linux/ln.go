// +build linux

// ported from OpenBSD ln
package main

import (
	"flag"
	"fmt"
	"os"
	"path/filepath"
	"syscall"

	"golang.org/x/sys/unix"
)

var (
	fflag bool
	hflag bool
	Pflag bool
	sflag bool

	status int
)

func main() {
	flag.BoolVar(&fflag, "f", false, "unlink any already existing file, permitting the link to occur")
	flag.BoolVar(&hflag, "h", false, "if the target is a symlink to a directory, do not descend to it")
	flag.BoolVar(&hflag, "n", false, "alias for -h for compatibility")
	flag.BoolVar(&sflag, "s", false, "create a symbolic link")
	flag.Var(symFlag('L'), "L", "when creating a hard link and the source is a symbolic link, link to the fully resolved target of symbolic link")
	flag.Var(symFlag('P'), "P", "when creating a hardlink and the source is a symbolic link, link to the symbolic link itself")
	flag.Usage = usage
	flag.Parse()

	switch flag.NArg() {
	case 0:
		usage()
	case 1:
		ek(ln(flag.Arg(0), ".", true))
	case 2:
		ek(ln(flag.Arg(0), flag.Arg(1), false))
	default:
		sourceDir := flag.Arg(flag.NArg() - 1)
		fi, err := os.Stat(sourceDir)
		ck(err)

		if !fi.IsDir() {
			usage()
		}

		args := flag.Args()
		for _, name := range args[:len(args)-1] {
			ek(ln(name, sourceDir, true))
		}
	}

	os.Exit(status)
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		status |= 1
		return true
	}
	return false
}

func ck(err error) {
	if ek(err) {
		os.Exit(status)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [-fhLnPs] source [target]")
	fmt.Fprintln(os.Stderr, "       [-fLPs] source ... [directory]")
	flag.PrintDefaults()
	os.Exit(1)
}

func ln(target, source string, isdir bool) error {
	var fi os.FileInfo
	var err error

	statp := os.Stat
	if Pflag {
		statp = os.Lstat
	}
	if !sflag {
		fi, err = statp(target)
		if err != nil {
			return err
		}

		// only symbolic links to directory
		if fi.IsDir() {
			return fmt.Errorf("%s is a directory", target)
		}
	}

	// if the source is a directory, append the target's name
	statl := os.Stat
	if hflag {
		statl = os.Lstat
	}
	if isdir {
		fi, err = statl(source)
		if err == nil && fi.IsDir() {
			base := filepath.Base(target)
			if base == "" {
				return fmt.Errorf("%s", target)
			}
			source = filepath.Join(source, base)
		}
	}

	fi, err = os.Lstat(source)
	if err == nil && !sflag {
		tfi, err := statp(target)
		if err != nil {
			return fmt.Errorf("%s: dissapeared", target)
		}

		st, ok1 := fi.Sys().(*syscall.Stat_t)
		sb, ok2 := tfi.Sys().(*syscall.Stat_t)
		if ok1 && ok2 && st.Dev == sb.Dev && st.Ino == sb.Ino {
			if fflag {
				return nil
			} else {
				return fmt.Errorf("%s and %s are identical (nothing done).", target, source)
			}
		}
	}

	if fflag {
		err = syscall.Unlink(source)
	} else if sflag {
		err = syscall.Symlink(target, source)
	} else {
		follow := 0
		if !Pflag {
			follow = unix.AT_SYMLINK_FOLLOW
		}
		err = unix.Linkat(unix.AT_FDCWD, target, unix.AT_FDCWD, source, follow)
	}
	return err
}

type symFlag rune

func (symFlag) IsBoolFlag() bool { return true }
func (symFlag) String() string   { return "" }
func (s symFlag) Set(string) error {
	Pflag = true
	if s == 'L' {
		Pflag = false
	}
	return nil
}
