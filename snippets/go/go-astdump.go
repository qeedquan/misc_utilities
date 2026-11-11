package main

import (
	"flag"
	"fmt"
	"go/ast"
	"go/parser"
	"go/token"
	"io"
	"os"
)

var (
	status = 0
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		dump("<stdin>", os.Stdin)
	} else {
		for _, name := range flag.Args() {
			r, err := os.Open(name)
			if ek(err) {
				continue
			}
			dump(name, r)
			r.Close()
		}
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: go-astdump <file> ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func dump(name string, r io.Reader) {
	fset := token.NewFileSet()
	f, err := parser.ParseFile(fset, name, r, parser.ParseComments)
	if ek(err) {
		return
	}
	ast.Print(fset, f)
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintf(os.Stderr, "go-astdump: %v\n", err)
		status = 1
		return true
	}
	return false
}
