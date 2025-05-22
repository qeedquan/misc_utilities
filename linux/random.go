// ported from unix v10
package main

import (
	"bufio"
	"flag"
	"fmt"
	"math/rand"
	"os"
	"strconv"
	"time"
)

var (
	eflag = flag.Bool("e", false, "exit with random status")
)

func main() {
	flag.Usage = usage
	flag.Parse()
	rand.Seed(time.Now().UnixNano())
	if *eflag {
		os.Exit(rand.Int())
	}

	fract := 2.0
	if flag.NArg() >= 1 {
		fract, _ = strconv.ParseFloat(flag.Arg(0), 64)
	}

	sc := bufio.NewScanner(os.Stdin)
	for sc.Scan() {
		ln := sc.Text()
		if rand.Float64()*fract < 1 {
			fmt.Println(ln)
		}
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] fraction")
	flag.PrintDefaults()
	os.Exit(2)
}
