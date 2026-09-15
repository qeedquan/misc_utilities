package main

import (
	"bufio"
	"bytes"
	"flag"
	"fmt"
	"os"
	"sort"
	"unicode"
)

var (
	wflag = flag.Bool("w", false, "count word frequencies instead of characters")

	freq   = make(map[string]uint64)
	status = 0
)

type word struct {
	chars string
	count uint64
}

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		count(os.Stdin)
	} else {
		for _, name := range flag.Args() {
			f, err := os.Open(name)
			if ek(err) {
				continue
			}
			count(f)
			f.Close()
		}
	}

	var words []word
	for chars, count := range freq {
		words = append(words, word{chars, count})
	}
	sort.Slice(words, func(i, j int) bool {
		return words[i].chars < words[j].chars
	})

	for _, w := range words {
		if isPrint(w.chars) {
			fmt.Printf("%s %d\n", w.chars, w.count)
		} else {
			fmt.Printf("%q %d\n", w.chars, w.count)
		}
	}

	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: freq [options] <files> ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "freq:", err)
		status = 1
		return true
	}
	return false
}

func count(f *os.File) {
	b := bufio.NewReader(f)
	w := new(bytes.Buffer)
	for {
		r, _, err := b.ReadRune()
		if err != nil {
			break
		}

		if !*wflag {
			if isChar(r) {
				freq[string(r)]++
			}
			continue
		}

		if isChar(r) {
			w.WriteRune(r)
		} else if w.Len() > 0 {
			freq[w.String()]++
			w.Reset()
		}
	}
}

func isPrint(s string) bool {
	for _, r := range s {
		if !unicode.IsPrint(r) {
			return false
		}
	}
	return true
}

func isChar(r rune) bool {
	return unicode.IsNumber(r) || unicode.IsLetter(r)
}
