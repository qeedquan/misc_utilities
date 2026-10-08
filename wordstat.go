package main

import (
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"sort"
	"strings"
	"unicode"
	"unicode/utf8"
)

func main() {
	log.SetPrefix("wordstat: ")
	log.SetFlags(0)
	flag.Usage = usage
	flag.Parse()

	words := flag.Args()
	if flag.NArg() < 1 {
		buf, err := io.ReadAll(os.Stdin)
		if err != nil {
			log.Fatal(err)
		}

		lines := strings.Split(string(buf), "\n")
		for _, line := range lines {
			spaces := strings.Split(line, " ")
			words = append(words, spaces...)
		}
	}

	for _, word := range words {
		var ws WS
		ws.Analyze(word)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] <word> ...")
	flag.PrintDefaults()
	os.Exit(2)
}

type WS struct{}

func (ws *WS) Analyze(word string) {
	word = strings.TrimSpace(word)
	if word == "" {
		return
	}

	fmt.Printf("%q (Length: %d)\n", word, utf8.RuneCountInString(word))
	ws.basic(word)
	ws.letterdiff(word)
	ws.histogram(word)
	fmt.Printf("\n")
}

func (ws *WS) histogram(word string) {
	m := make(map[rune]int)
	for _, r := range word {
		m[r]++
	}

	var p [][2]int64
	for k, v := range m {
		p = append(p, [2]int64{int64(k), int64(v)})
	}
	sort.Slice(p, func(i, j int) bool {
		if p[i][1] == p[j][1] {
			return p[i][0] < p[j][0]
		}
		return p[i][1] > p[j][1]
	})

	fmt.Printf("[Histogram]\n")
	for _, h := range p {
		fmt.Printf("%c: %d\n", h[0], h[1])
	}
	fmt.Printf("\n")
}

func (ws *WS) basic(word string) {
	var sum, mul, xor, and, or, vowel uint

	mul = 1
	for _, r := range word {
		sum += uint(r)
		mul *= uint(r)
		xor ^= uint(r)
		and &= uint(r)
		or |= uint(r)
		if isvowel(r) {
			vowel++
		}
	}
	fmt.Printf("[Basic]\n")
	fmt.Printf("Sum:        %d %#x\n", sum, sum)
	fmt.Printf("Mul:        %d %#x\n", mul, mul)
	fmt.Printf("XOR:        %d %#x\n", xor, xor)
	fmt.Printf("AND:        %d %#x\n", and, and)
	fmt.Printf("OR:         %d %#x\n", or, or)
	fmt.Printf("Vowels:     %d %#x\n", vowel, vowel)
	fmt.Printf("Palindrome: %v\n", ispalindrome(word))
}

func (ws *WS) letterdiff(word string) {
	fmt.Printf("[Letter Differences, Sum, XOR, AND, OR]\n")
	lr := rune(-1)
	i := 0
	for _, r := range word {
		if lr >= 0 {
			fmt.Printf("%d: %d %d %d %d %d\n", i, rabs(r-lr), rabs(r+lr), rabs(r^lr), rabs(r&lr), rabs(r|lr))
		}
		lr = r
		i++
	}
}

func rabs(x rune) rune {
	if x < 0 {
		return -x
	}
	return x
}

func isvowel(r rune) bool {
	return strings.IndexRune("aeiou", unicode.ToLower(r)) >= 0
}

func ispalindrome(s string) bool {
	r := []rune(s)
	for i := 0; i < len(r)/2; i++ {
		if r[i] != r[len(r)-i-1] {
			return false
		}
	}
	return true
}
