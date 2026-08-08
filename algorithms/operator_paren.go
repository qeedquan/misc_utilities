// algorithm described at https://en.wikipedia.org/wiki/Operator-precedence_parser
package main

import (
	"bytes"
	"flag"
	"fmt"
	"os"
	"strings"
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}
	expr := strings.Join(flag.Args(), "")
	expr = strings.Replace(expr, " ", "", -1)
	fmt.Printf("%s -> %s\n", expr, paren(expr))
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: expr")
	os.Exit(0)
}

func paren(s string) string {
	b := new(bytes.Buffer)
	fmt.Fprint(b, "((((")

	var c rune
	for i, r := range s {
		switch r {
		case '(':
			fmt.Fprint(b, "((((")
		case ')':
			fmt.Fprint(b, "))))")
		case '^':
			fmt.Fprint(b, ")^(")
		case '*':
			fmt.Fprint(b, "))*((")
		case '/':
			fmt.Fprint(b, "))/((")
		case '+', '-':
			if i == 0 || strings.IndexRune("(^*+-", c) >= 0 {
				fmt.Fprint(b, string(r))
			} else {
				fmt.Fprint(b, ")))"+string(r)+"(((")
			}
		default:
			fmt.Fprint(b, string(r))
		}
		c = r
	}
	fmt.Fprint(b, "))))")
	return b.String()
}
