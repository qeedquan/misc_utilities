// a bf interpreter

package main

import (
	"flag"
	"fmt"
	"os"
)

const (
	MemSize = 500000
)

func main() {
	flag.Parse()
	if flag.NArg() < 1 {
		die("usage: <file>")
	}

	run(flag.Arg(0))
}

func run(fn string) {
	code, err := os.ReadFile(fn)
	if err != nil {
		die("%v", err)
	}

	ip := 0
	mem := make([]byte, MemSize)

	for i := 0; i < len(code); i++ {
		switch code[i] {
		case '>':
			if ip++; ip > len(mem) {
				nmem := make([]byte, len(mem)*2)
				copy(nmem, mem)
				mem = nmem
			}

		case '<':
			if ip--; ip < 0 {
				nmem := make([]byte, len(mem)*2)
				copy(nmem[len(mem):], mem)
				ip = len(mem) - 1
			}

		case '.':
			fmt.Printf("%c", mem[ip])

		case '+':
			mem[ip]++

		case '-':
			mem[ip]--

		case ',':
			var r rune
			fmt.Scanf("%v", &r)
			mem[ip] = byte(r)

		case '[':
			if mem[ip] == 0 {
				for p := 1; p > 0; {
					if i++; i >= len(code) {
						die("unmatched ]")
					}

					if code[i] == '[' {
						p++
					} else if code[i] == ']' {
						p--
					}
				}
			}

		case ']':
			if mem[ip] != 0 {
				for p := 1; p > 0; {
					if i--; i < 0 {
						die("unmatched [")
					}

					if code[i] == ']' {
						p++
					} else if code[i] == '[' {
						p--
					}
				}
			}
		}
	}
}

func die(format string, args ...interface{}) {
	fmt.Printf(format+"\n", args...)
	os.Exit(1)
}
