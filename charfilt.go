package main

import (
	"bufio"
	"flag"
	"fmt"
	"os"
	"sort"
	"strings"
	"unicode"
)

func main() {
	filter := flag.String("f", "ascii", "filter to use")
	flag.Usage = usage
	flag.Parse()

	is := funcs[*filter]
	r := bufio.NewReader(os.Stdin)
	w := bufio.NewWriter(os.Stdout)
	defer w.Flush()
	for {
		c, _, err := r.ReadRune()
		if err != nil {
			break
		}

		if is != nil && is(c) {
			continue
		}

		w.WriteRune(c)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options]")
	flag.PrintDefaults()

	strlist := []string{}
	for key := range funcs {
		strlist = append(strlist, key)
	}
	sort.Strings(strlist)
	fmt.Fprintf(os.Stderr, "available filters: %s\n", strings.Join(strlist, " "))

	os.Exit(2)
}

func isascii(r rune) bool {
	return r >= 0x80
}

var funcs = map[string]func(r rune) bool{
	"ascii":   isascii,
	"control": unicode.IsControl,
	"digit":   unicode.IsDigit,
	"graphic": unicode.IsGraphic,
	"letter":  unicode.IsLetter,
	"lower":   unicode.IsLower,
	"mark":    unicode.IsMark,
	"number":  unicode.IsNumber,
	"upper":   unicode.IsUpper,
	"space":   unicode.IsSpace,
	"symbol":  unicode.IsSymbol,
	"print":   unicode.IsPrint,
	"punct":   unicode.IsPunct,
	"title":   unicode.IsTitle,
}
