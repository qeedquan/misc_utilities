package main

import (
	"flag"
	"fmt"
	"io"
	"math/rand"
	"os"
	"time"
)

var (
	status = 0

	iterations = flag.Int("n", 3, "overwrite for N iterations")
	remove     = flag.Bool("u", false, "remove after overwriting")
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}
	for _, name := range flag.Args() {
		ek(shred(name))
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] file ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func ek(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "shred:", err)
		status = 1
	}
}

func shred(name string) error {
	var err error
	for i := 0; i < *iterations; i++ {
		xerr := mangle(name)
		if os.IsExist(xerr) {
			return xerr
		}
		if xerr != nil {
			err = xerr
		}
	}
	if err == nil && *remove {
		err = os.Remove(name)
	}
	return err
}

func mangle(name string) error {
	f, err := os.OpenFile(name, os.O_RDWR, 0)
	if err != nil {
		return err
	}
	fi, err := f.Stat()
	if err != nil {
		return err
	}

	s := rand.NewSource(time.Now().UnixNano())
	r := rand.New(s)
	_, err = io.Copy(f, &io.LimitedReader{r, fi.Size()})

	f.Sync()

	xerr := f.Close()
	if err == nil {
		err = xerr
	}
	return err
}
