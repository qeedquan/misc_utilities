package main

import (
	"flag"
	"fmt"
	"os"
	"path/filepath"
	"strings"
)

var (
	aflag = flag.Bool("a", false, "print all matching pathnames of each argument")
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}
	for _, cmd := range flag.Args() {
		which(cmd, "path", " ")
		which(cmd, "PATH", ":")
	}
}

func which(cmd, env, delim string) {
	path := os.Getenv(env)
	if path == "" {
		return
	}
	toks := strings.Split(path, delim)
	for _, tok := range toks {
		name := filepath.Join(tok, cmd)
		f, err := os.Open(name)
		if err != nil {
			continue
		}
		f.Close()

		fmt.Println(name)
		if !*aflag {
			break
		}
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] cmd ...")
	flag.PrintDefaults()
	os.Exit(2)
}
