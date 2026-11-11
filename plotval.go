package main

import (
	"bytes"
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"os/exec"
)

func main() {
	sigch := make(chan os.Signal)
	flag.Usage = usage
	flag.Parse()

	var files []string
	var name string
	if flag.NArg() < 1 {
		buf := readstdin(sigch)
		name = writetmpfile(buf)
		defer os.Remove(name)
		files = append(files, name)
	} else {
		files = flag.Args()
	}

	var cmds []*exec.Cmd
	for _, name := range files {
		args := fmt.Sprintf("set term wxt title '%s';", name)
		args += fmt.Sprintf("N = system(\"awk 'NR==1{print NF}' %s\");", name)
		args += fmt.Sprintf("plot for [i=1:N] \"%s\" u 0:i w l title \"Column \".i;", name)
		cmd := exec.Command("gnuplot", "-p", "-e", args)
		cmd.Stdin = os.Stdin
		cmd.Stderr = os.Stderr
		cmd.Stdout = os.Stdout
		cmd.Start()
		cmds = append(cmds, cmd)
	}
	for _, cmd := range cmds {
		cmd.Wait()
	}
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: plotval [options] <file> ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func readstdin(sigch chan os.Signal) []byte {
	var buf []byte
	var err error

	readch := make(chan bool)
	go func() {
		buf, err = io.ReadAll(os.Stdin)
		if err != nil {
			log.Fatal(err)
		}

		readch <- true
	}()

	select {
	case <-sigch:
		os.Exit(1)

	case <-readch:
	}

	return buf
}

func writetmpfile(buf []byte) string {
	f, err := os.CreateTemp(".", "plotval")
	ck(err)

	name := f.Name()

	w := bytes.NewBuffer(buf)
	io.Copy(f, w)
	f.Close()

	return name
}
