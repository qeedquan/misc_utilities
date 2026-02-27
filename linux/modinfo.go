// +build linux
// ported from toybox

package main

import (
	"bytes"
	"flag"
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"syscall"
)

var (
	basedir = flag.String("b", "/", "base directory")
	version = flag.String("k", uname(), "kernel version")
	xfield  = flag.String("F", "", "only print this field value")
	zero    = flag.Bool("0", false, "use nul character as a delimiter instead of newline")

	status = 0
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	for _, name := range flag.Args() {
		if strings.HasSuffix(name, ".ko") {
			modinfo(name)
		} else {
			dir := filepath.Join(*basedir, "lib", "modules", *version)
			found := false
			name := strings.Replace(name, "-", "_", -1)

			filepath.Walk(dir, func(path string, info os.FileInfo, err error) error {
				if found {
					return filepath.SkipDir
				}
				if err != nil {
					return nil
				}

				base := filepath.Base(path)
				base = strings.Replace(base, "-", "_", -1)
				if strings.HasPrefix(base, name) && base[len(name):] == ".ko" {
					modinfo(path)
					found = true
					return filepath.SkipDir
				}

				return nil
			})

			if !found {
				fmt.Fprintf(os.Stderr, "modinfo: ERROR: Module %s not found\n", name)
				status |= 1
			}
		}
	}

	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] modname ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "modinfo:", err)
		status |= 1
		return true
	}
	return false
}

func ck(err error) {
	if ek(err) {
		os.Exit(1)
	}
}

func modinfo(name string) {
	var tags = []string{
		"alias", "license", "description", "author", "firmware",
		"vermagic", "srcversion", "intree", "depends", "parm",
		"parmtype", "version",
	}

	buf, err := os.ReadFile(name)
	if ek(err) {
		return
	}

	output("filename", name)

	for i := range buf {
		if buf[i] != 0 {
			continue
		}

		if i+1 >= len(buf) {
			continue
		}

		for _, tag := range tags {
			if bytes.HasPrefix(buf[i+1:], []byte(tag+"=")) {
				value := ""
				for j := i + 2 + len(tag); j < len(buf) && buf[j] != 0; j++ {
					value += string(buf[j])
				}
				output(tag, value)
				break
			}
		}
	}
}

func output(field, value string) {
	if *xfield == "" {
		fmt.Printf("%s:%*c", field, 15-len(field), ' ')
	} else if field != *xfield {
		return
	}
	fmt.Printf("%s", value)
	if *zero {
		fmt.Printf("\x00")
	} else {
		fmt.Printf("\n")
	}
}

func uname() string {
	var uts syscall.Utsname
	ck(syscall.Uname(&uts))

	b := make([]byte, len(uts.Release))
	for i := range b {
		b[i] = byte(uts.Release[i])
		if b[i] == 0 {
			break
		}
	}
	return strings.TrimRight(string(b), "\x00")
}
