package main

import (
	"compress/bzip2"
	"flag"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"strings"
)

var (
	force = flag.Bool("f", false, "force")

	status = 0
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		_, err := io.Copy(os.Stdout, bzip2.NewReader(os.Stdin))
		ek(err)
	} else {
		for _, name := range flag.Args() {
			if !ek(bunzip2(name)) {
				os.Remove(name)
			}
		}
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] file ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "bunzip2:", err)
		status |= 1
		return true
	}
	return false
}

func bunzip2(name string) error {
	ext := strings.ToLower(filepath.Ext(name))
	if !*force && ext != ".bz2" {
		return fmt.Errorf("unknown suffix -- ignored")
	}

	f, err := os.Open(name)
	if err != nil {
		return err
	}
	defer f.Close()

	output := name[:len(name)-len(ext)]
	if !*force {
		_, err := os.Stat(output)
		if err == nil {
			var c rune
			fmt.Printf("%s already exists; do you wish to overwrite (y or n)? ", name)
			fmt.Scan(&c)
			if c != 'y' {
				fmt.Println("not overwritten")
				return nil
			}
		}
	}

	w, err := os.Create(output)
	if err != nil {
		return err
	}

	r := bzip2.NewReader(f)
	_, err = io.Copy(w, r)
	xerr := w.Close()

	if err != nil {
		return err
	}
	return xerr
}
