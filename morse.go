// ported from unix v10
package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"os"
	"strings"
)

const (
	dit = "dit"
	daw = "daw"
)

var digit = []string{
	"-----",
	".----",
	"..---",
	"...--",
	"....-",
	".....",
	"-....",
	"--...",
	"---..",
	"----.",
}

var alph = []string{
	".-",
	"-...",
	"-.-.",
	"-..",
	".",
	"..-.",
	"--.",
	"....",
	"..",
	".---",
	"-.-",
	".-..",
	"--",
	"-.",
	"---",
	".--.",
	"--.-",
	".-.",
	"...",
	"-",
	"..-",
	"...-",
	".--",
	"-..-",
	"-.--",
	"--..",
}

func main() {
	flag.Parse()
	if flag.NArg() < 1 {
		morse(os.Stdin)
	} else {
		s := strings.Join(flag.Args(), "")
		s += "\n"
		r := strings.NewReader(s)
		morse(r)
	}
}

func morse(r io.Reader) {
	b := bufio.NewReader(r)
	for {
		c, err := b.ReadByte()
		if err != nil {
			break
		}

		if isupper(c) {
			c = tolower(c)
		}
		switch {
		case isalpha(c):
			print(alph[c-'a'])
		case isdigit(c):
			print(digit[c-'0'])
		case c == ',':
			print("--..--")
		case c == '.':
			print(".-.-.-")
		case isspace(c):
			fmt.Printf(" ...\n")
		}
	}
}

func isupper(c byte) bool {
	return 'A' <= c && c <= 'Z'
}

func isalpha(c byte) bool {
	return 'a' <= c && c <= 'z' ||
		'A' <= c && c <= 'Z'
}

func isdigit(c byte) bool {
	return '0' <= c && c <= '9'
}

func isspace(c byte) bool {
	return c == ' ' || c == '\t' || c == '\r' || c == '\n'
}

func tolower(c byte) byte {
	return c - 'A' + 'a'
}

func print(s string) {
	for i := 0; i < len(s); i++ {
		if s[i] == '.' {
			fmt.Printf(" %s", dit)
		} else if s[i] == '-' {
			fmt.Printf(" %s", daw)
		}
	}
	fmt.Printf(",\n")
}