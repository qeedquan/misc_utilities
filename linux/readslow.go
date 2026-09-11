// ported from unix v10
package main

import (
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"time"
)

var (
	seekend = flag.Bool("e", false, "seek to end")
)

func main() {
	log.SetPrefix("readslow: ")
	log.SetFlags(0)
	flag.Parse()

	var (
		err error
		buf [256]byte
		amt int64
	)

	f := os.Stdin
	if flag.NArg() > 0 {
		f, err = os.Open(flag.Arg(0))
		ck(err)
	}

	if *seekend {
		amt, _ = f.Seek(0, io.SeekEnd)
	}
	for {
		n, err := f.Read(buf[:])
		if err != nil && err != io.EOF {
			os.Exit(1)
		}

		if n > 0 {
			os.Stdout.Write(buf[:n])
			amt += int64(n)
		} else if err == io.EOF {
			time.Sleep(10 * time.Second)
			fi, err := f.Stat()
			if err != nil {
				os.Exit(1)
			}
			if fi.Size() < amt {
				fmt.Printf("\n<<< file truncated -- restarting >>>\n")
				f.Seek(0, io.SeekStart)
				amt = 0
			}
		}
	}
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}
