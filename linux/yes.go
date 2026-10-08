package main

import (
	"os"
	"strings"
)

func main() {
	if len(os.Args) < 2 {
		yes("y")
	} else {
		yes(strings.Join(os.Args[1:], " "))
	}
}

func yes(s string) {
	s += "\n"
	for {
		os.Stdout.WriteString(s)
	}
}
