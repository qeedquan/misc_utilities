// ported from openbsd pig

package main

import (
	"bufio"
	"flag"
	"fmt"
	"log"
	"os"
	"strings"
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("pig: ")
	flag.Parse()

	var b [4096 * 2]byte
	var n int

	r := bufio.NewReader(os.Stdin)
	for {
		c, err := r.ReadByte()
		if err != nil {
			break
		}
		if isalpha(c) {
			if n >= len(b)/2 {
				log.Fatal("ate too much!")
			}
			b[n], n = c, n+1
			continue
		}
		if n != 0 {
			pigout(b[:], n)
			n = 0
		}
		fmt.Printf("%c", c)
	}
}

func pigout(b []byte, n int) {
	allupper := isupper(b[0])
	firstupper := allupper
	for i := 1; i < n && allupper; i++ {
		allupper = allupper && isupper(b[i])
	}

	if strings.IndexByte("aeiouAEIOU", b[0]) >= 0 {
		way := "way"
		if allupper {
			way = "WAY"
		}
		fmt.Printf("%.*s%s", n, b[:n], way)
		return
	}

	if !allupper {
		b[0] = tolower(b[0])
	}

	s := 0
	o := n
	for strings.IndexByte("aeiouyAEIOUY", b[s]) < 0 && s < o {
		c := b[n]

		b[n] = b[s]
		n, s = n+1, s+1

		if (c == 'q' || c == 'Q') && s < o && (b[s] == 'u' || b[s] == 'U') {
			b[n] = b[s]
			n, s = n+1, s+1
		}
	}

	if firstupper {
		b[s] = toupper(b[s])
	}

	ay := "ay"
	if allupper {
		ay = "AY"
	}
	fmt.Printf("%.*s%s", o, b[s:], ay)
}

func isalpha(c byte) bool {
	return 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z'
}

func isupper(c byte) bool {
	return 'A' <= c && c <= 'Z'
}

func islower(c byte) bool {
	return 'a' <= c && c <= 'z'
}

func tolower(c byte) byte {
	if isupper(c) {
		return c - 'A' + 'a'
	}
	return c
}

func toupper(c byte) byte {
	if islower(c) {
		return c - 'a' + 'A'
	}
	return c
}
