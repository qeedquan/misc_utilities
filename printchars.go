package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"strconv"
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("printchar: ")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 2 {
		printChars(32, 127)
	} else {
		printChars(atoi(flag.Arg(0)), atoi(flag.Arg(1)))
	}
}

func printChars(start, end int64) {
	if end < start {
		start, end = end, start
	}
	for ; start <= end; start++ {
		fmt.Printf("%c", start)
	}
}

func atoi(str string) int64 {
	n, err := strconv.ParseInt(str, 0, 64)
	if err != nil {
		log.Fatal(err)
	}
	if n < 0 || n > 0x7fffffff {
		log.Fatalf("invalid range value %d", n)
	}
	return n
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: <start> <end>")
	flag.PrintDefaults()
	os.Exit(2)
}
