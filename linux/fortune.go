// ported from plan9 fortune

package main

import (
	"bufio"
	"bytes"
	"flag"
	"fmt"
	"math/rand"
	"os"
	"time"
)

var (
	file = flag.String("f", "/usr/share/games/fortunes/fortunes", "use fortune file")
	sep  = flag.String("s", "%", "use separator")
)

func main() {
	flag.Usage = usage
	flag.Parse()

	f, err := os.Open(*file)
	if err != nil {
		fmt.Println("Misfortune")
		os.Exit(1)
	}
	defer f.Close()

	rand.Seed(time.Now().UnixNano())

	var choice string
	s := bufio.NewScanner(f)
	s.Split(scanFortunes)
	for i := int64(1); ; i++ {
		if !s.Scan() {
			break
		}
		line := s.Text()

		if rand.Int63()%i == 0 {
			choice = line
		}
	}

	fmt.Println(choice)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options]")
	flag.PrintDefaults()
	os.Exit(1)
}

func scanFortunes(data []byte, atEOF bool) (advance int, token []byte, err error) {
	if atEOF && len(data) == 0 {
		return
	}

	if i := bytes.Index(data, []byte(*sep)); i >= 0 {
		n := i
		if n > 0 {
			n--
		}
		return i + 2, data[0:n], nil
	}

	if atEOF {
		return len(data), data, nil
	}

	return
}
