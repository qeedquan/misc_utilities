package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"os"
	"os/signal"
)

var (
	append_ = flag.Bool("a", false, "append to given files, do not overwrite")
	sigint  = flag.Bool("i", false, "ignore interrupt signal")
)

var (
	status = 0
)

func main() {
	flag.Usage = usage
	flag.Parse()

	if *sigint {
		signal.Ignore(os.Interrupt)
	}

	mode := os.O_CREATE | os.O_WRONLY
	if *append_ {
		mode |= os.O_APPEND
	} else {
		mode |= os.O_TRUNC
	}

	var files []*os.File
	var writers []*bufio.Writer
	for _, name := range flag.Args() {
		f, err := os.OpenFile(name, mode, 0666)
		if ck(err) {
			continue
		}
		files = append(files, f)
		writers = append(writers, bufio.NewWriter(f))
	}
	writers = append(writers, bufio.NewWriter(os.Stdout))

	for {
		var buf [8192]byte

		n, err := os.Stdin.Read(buf[:])
		if n > 0 {
			for _, w := range writers {
				w.Write(buf[:n])
			}
		}

		if err == io.EOF {
			break
		}

		ck(err)
	}

	for _, w := range writers {
		err := w.Flush()
		ck(err)
	}

	for _, f := range files {
		err := f.Close()
		ck(err)
	}

	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "tee: usage: [options] file ...")
	flag.PrintDefaults()
	os.Exit(1)
}

func ck(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "tee:", err)
		status = 1
		return true
	}
	return false
}
