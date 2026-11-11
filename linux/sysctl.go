// +build linux

// ported from toybox
package main

import (
	"bufio"
	"flag"
	"fmt"
	"os"
	"path/filepath"
	"strings"
)

var opt struct {
	show  bool
	quiet bool
	key   bool
	value bool
	file  string
	write bool
}

func main() {
	flag.BoolVar(&opt.show, "a", false, "show all values")
	flag.BoolVar(&opt.show, "A", false, "show all values")
	flag.BoolVar(&opt.quiet, "e", false, "don't warn about unknown keys")
	flag.BoolVar(&opt.value, "N", false, "don't print key values")
	flag.BoolVar(&opt.key, "n", false, "don't print key names")
	flag.StringVar(&opt.file, "p", "/etc/sysctl.conf", "read values from file")
	flag.BoolVar(&opt.key, "w", false, "only write values")
	flag.Parse()

	switch {
	case opt.show:
		filepath.Walk("/proc/sys", showKeys)

	case flagSet("p"):
		fd, err := os.Open(opt.file)
		ck(err)

		scan := bufio.NewScanner(fd)
		for scan.Scan() {
			processKey(scan.Text())
		}

	case flag.NArg() == 0:
		flag.Usage()
		os.Exit(2)

	default:
		for _, arg := range flag.Args() {
			processKey(arg)
		}
	}
}

func ck(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
}

func flagSet(name string) bool {
	set := false
	flag.Visit(func(f *flag.Flag) {
		if f.Name == name {
			set = true
		}
	})
	return set
}

func showKeys(name string, info os.FileInfo, err error) error {
	if info.IsDir() || err != nil {
		return nil
	}

	key := strings.TrimPrefix(name, "/proc/sys/")
	key = strings.Replace(key, "/", ".", -1)

	data, err := os.ReadFile(name)
	if keyError(key, err) {
		return nil
	}

	if !opt.key {
		fmt.Printf("%s", key)
	}
	if !opt.key && !opt.value {
		fmt.Printf(" = ")
	}
	if !opt.value {
		fmt.Printf("%s", strings.TrimSpace(string(data)))
	}
	if !opt.key || !opt.value {
		fmt.Printf("\n")
	}

	return nil
}

func processKey(line string) {
	line = strings.TrimSpace(line)
	if line == "" || strings.HasPrefix(line, "#") || strings.HasPrefix(line, ";") {
		return
	}

	toks := strings.Split(line, "=")
	if len(toks) != 2 {
		fmt.Fprintf(os.Stderr, "%q not key=value\n", line)
		return
	}

	key := strings.TrimSpace(toks[0])
	value := strings.TrimSpace(toks[1])

	if opt.write && value == "" {
		fmt.Fprintf(os.Stderr, "%q not key=value\n", key)
		return
	}

	name := fmt.Sprintf("/proc/sys/%s", key)
	name = strings.Replace(name, ".", "/", -1)
	f, err := os.OpenFile(name, os.O_WRONLY, 0644)
	if err != nil {
		if !opt.quiet {
			keyError(key, err)
		}
	} else {
		f.Write([]byte(value))
		f.Close()
	}
}

func keyError(key string, err error) bool {
	if err == nil {
		return false
	}

	if os.IsNotExist(err) {
		if !opt.quiet {
			fmt.Fprintf(os.Stderr, "unknown key %q\n", key)
		}
	} else {
		fmt.Fprintf(os.Stderr, "key %q: %v\n", key, err)
	}

	return true
}
