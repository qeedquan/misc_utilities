// ported from 9front date

package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"strconv"
	"time"
)

var (
	nflg = flag.Bool("n", false, "report the date as the number since the epoch")
	uflg = flag.Bool("u", false, "report GMT rather than local time")
	iflg = flag.Bool("i", false, "report in ISO date format")
	tflg = flag.Bool("t", false, "report full ISO date format (needs -i)")
	fflg = flag.String("f", "", "report using a custom time format")
)

func main() {
	log.SetFlags(0)
	flag.Usage = usage
	flag.Parse()

	var now int64
	if flag.NArg() == 1 {
		var err error

		now, err = strconv.ParseInt(flag.Arg(0), 0, 64)
		if err != nil {
			log.Fatal(err)
		}
	} else {
		now = time.Now().Unix()
	}

	if *nflg {
		fmt.Println(now)
		return
	}

	tm := time.Now()
	if *uflg {
		tm = tm.Local()
	}

	switch {
	case *fflg != "":
		fmt.Println(tm.Format(*fflg))

	case *iflg:
		if *tflg {
			fmt.Println(tm.Format(time.RFC3339))
		} else {
			fmt.Println(tm.Format("2006-01-02"))
		}

	default:
		fmt.Println(tm.Format(time.UnixDate))
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [-itun] [-f format] [seconds]")
	flag.PrintDefaults()
	os.Exit(1)
}
