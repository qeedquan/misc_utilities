package main

import (
	"flag"
	"fmt"
	"os"
	"strconv"
)

func main() {
	parent := false
	mode := modeFlag(0755)

	flag.Var(&mode, "m", "set file mode")
	flag.BoolVar(&parent, "p", parent, "make parent directories as needed")

	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	status := 0
	for _, name := range flag.Args() {
		var err error
		if parent {
			err = os.MkdirAll(name, os.FileMode(mode))
		} else {
			err = os.Mkdir(name, os.FileMode(mode))
		}

		if err != nil {
			fmt.Fprintln(os.Stderr, err)
			status = 1
		}
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] directory ...")
	flag.PrintDefaults()
	os.Exit(1)
}

type modeFlag os.FileMode

func (m modeFlag) String() string {
	return fmt.Sprintf("%#o", m)
}

func (m *modeFlag) Set(s string) error {
	n, err := strconv.ParseUint(s, 8, 64)
	if err != nil {
		return fmt.Errorf("invalid mode: %v", err)
	}
	if n > 0777 {
		return fmt.Errorf("invalid mode")
	}
	*m = modeFlag(n)
	return nil
}
