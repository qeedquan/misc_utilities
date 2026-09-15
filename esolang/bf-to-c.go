// translates bf to c

package main

import (
	"flag"
	"fmt"
	"os"
	"strings"
)

func main() {
	flag.Parse()
	if flag.NArg() < 1 {
		die("usage: <file>")
	}

	run(flag.Arg(0))
}

func run(fn string) {
	const accepted = "><.+-,[]"

	code, err := os.ReadFile(fn)
	if err != nil {
		die("%v", err)
	}

	fmt.Println(`
#include <stdio.h>
#include <stdlib.h>

unsigned char *ip;
unsigned char mem[30000];

int main(void) {
	ip = mem;
`)

	tabs := 1
	for i := 0; i < len(code); i++ {
		c := code[i]
		if strings.ContainsRune(accepted, rune(c)) {
			if c == ']' {
				tabs--
			}

			if tabs <= 0 {
				die("unmatched [")
			}

			fmt.Printf(strings.Repeat("\t", tabs))
		}

		switch c {
		case '>':
			fmt.Println("ip++;")

		case '<':
			fmt.Println("ip--;")

		case '.':
			fmt.Println("putchar(*ip);")

		case '+':
			fmt.Println("(*ip)++;")

		case '-':
			fmt.Println("(*ip)--;")

		case ',':
			fmt.Println("*ip = getchar();")

		case '[':
			fmt.Println("while (*ip) {")
			tabs++

		case ']':
			fmt.Println("}")
		}
	}

	fmt.Println(`
	return 0;
}
`)

}

func die(format string, args ...interface{}) {
	fmt.Printf(format+"\n", args...)
	os.Exit(1)
}
