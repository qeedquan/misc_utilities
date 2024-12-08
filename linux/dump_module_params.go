package main

import (
	"bufio"
	"flag"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"strings"
)

func main() {
	log.SetPrefix("dump_module_params: ")
	log.SetFlags(0)

	flag.Usage = usage
	flag.Parse()

	mods, err := getmods()
	ck(err)

	mods = filter(mods, flag.Args())

	fmt.Printf("Number of modules: %d\n", len(mods))
	for _, m := range mods {
		dumpmod(m)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] <modname> ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func getmods() ([]string, error) {
	f, err := os.Open("/proc/modules")
	if err != nil {
		return nil, err
	}
	defer f.Close()

	var mods []string
	s := bufio.NewScanner(f)
	for s.Scan() {
		ln := s.Text()
		ln = strings.TrimSpace(ln)

		toks := strings.Split(ln, " ")
		if len(toks) < 1 {
			continue
		}
		mods = append(mods, toks[0])
	}
	return mods, nil
}

func dumpmod(mod string) {
	rootdir := fmt.Sprintf("/sys/module/%s/parameters", mod)
	fmt.Printf("%s\n", mod)
	filepath.Walk(rootdir, func(path string, info os.FileInfo, err error) error {
		if path == rootdir {
			return nil
		}

		val := ""
		data, err := os.ReadFile(path)
		if err == nil {
			val = string(data)
		}
		val = strings.TrimSpace(val)

		base := filepath.Base(path)
		fmt.Printf("\t%s = %q\n", base, val)
		return nil
	})
	fmt.Println()
}

func filter(str, flt []string) []string {
	if len(flt) == 0 {
		return str
	}

	var p []string
loop:
	for _, s := range str {
		for _, f := range flt {
			if s == f {
				p = append(p, s)
				continue loop
			}
		}
	}
	return p
}
