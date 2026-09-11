// ported from suckless last
package main

import (
	"bufio"
	"bytes"
	"encoding/binary"
	"flag"
	"fmt"
	"log"
	"os"
	"time"
)

const (
	EMPTY         = 0
	RUN_LVL       = 1
	BOOT_TIME     = 2
	NEW_TIME      = 3
	OLD_TIME      = 4
	INIT_PROCESS  = 5
	LOGIN_PROCESS = 6
	USER_PROCESS  = 7
	DEAD_PROCESS  = 8
	ACCOUNTING    = 9
)

const (
	UT_LINESIZE = 32
	UT_NAMESIZE = 32
	UT_HOSTSIZE = 256
)

type ExitStatus struct {
	Termination int16
	Exit        int16
}

type UTMP struct {
	Type int16
	_    [2]byte
	Pid  int32
	Line [UT_LINESIZE]byte
	ID   [4]byte

	User [UT_NAMESIZE]byte
	Host [UT_HOSTSIZE]byte

	Exit ExitStatus

	Session int32
	Sec     int32
	Usec    int32

	AddrV6 [4]int32

	_ [20]byte
}

var (
	bflag = flag.Bool("b", false, "read from btmp file")
	luser string
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("last: ")
	flag.Usage = usage
	flag.Parse()

	file := "/var/log/wtmp"
	if *bflag {
		file = "/var/log/btmp"
	}
	if flag.NArg() >= 1 {
		luser = flag.Arg(0)
	}

	err := last(file)
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] <user>")
	flag.PrintDefaults()
	os.Exit(2)
}

func last(name string) error {
	f, err := os.Open(name)
	if err != nil {
		return err
	}
	defer f.Close()

	b := bufio.NewReader(f)

	for {
		var u UTMP
		err := binary.Read(b, binary.LittleEndian, &u)
		if err != nil {
			break
		}

		user := bytes.TrimRight(u.User[:], "\x00")
		line := bytes.TrimRight(u.Line[:], "\x00")
		host := bytes.TrimRight(u.Host[:], "\x00")

		if u.Type != USER_PROCESS || luser == string(user) {
			continue
		}

		fmt.Printf("%-8.8s %-8.8s %-16.16s %s\n", user, line, host, time.Unix(int64(u.Sec), int64(u.Usec)*1000))
	}

	return nil
}
