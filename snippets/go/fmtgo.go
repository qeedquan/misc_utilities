package main

import (
	"bytes"
	"flag"
	"fmt"
	"go/ast"
	"go/format"
	"go/parser"
	"go/printer"
	"go/token"
	"log"
	"os"
)

var (
	lflag = flag.Bool("l", false, "emit source pos in raw mode")
	rflag = flag.Bool("r", false, "raw mode")
	pflag = flag.Bool("p", false, "emit //line directives")
	sflag = flag.Bool("s", false, "use spaces instead of tabs for alignment")
	wflag = flag.Bool("w", false, "write to file instead of stdout")
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("fmtgo: ")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() == 0 {
		usage()
	}

	filename := flag.Arg(0)
	src, err := os.ReadFile(filename)
	ck(err)

	switch {
	case *rflag:
		fset := token.NewFileSet()
		file, err := parser.ParseFile(fset, filename, src, parser.ParseComments)
		ck(err)

		ast.SortImports(fset, file)

		var buf bytes.Buffer
		mode := printer.RawFormat
		if *lflag {
			mode |= printer.SourcePos
		}
		if *sflag {
			mode |= printer.UseSpaces
		}
		if *pflag {
			mode |= printer.SourcePos
		}
		cfg := printer.Config{
			Mode:     mode,
			Tabwidth: 4,
		}
		err = cfg.Fprint(&buf, fset, file)
		ck(err)

		src = buf.Bytes()

	default:
		src, err = format.Source(src)
		ck(err)
	}

	if *wflag {
		err = os.WriteFile(flag.Arg(0), src, 0644)
		ck(err)
	} else {
		os.Stdout.Write(src)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: file")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}
