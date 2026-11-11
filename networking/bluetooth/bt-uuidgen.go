package main

import (
	"crypto/rand"
	"flag"
	"fmt"
	"log"
)

// https://www.bluetooth.com/specifications/assigned-numbers/service-discovery/
// any value outside of the first 'X' prefix with different suffix is available for personal use
// must ask for assignment by the bluetooth spec people in order to be in the range of the base uuid
const (
	BASE_UUID = "XXXXXXXX-0000-1000-8000-00805F9B34FB"
)

var (
	usebase = flag.Bool("b", false, "generate uuid inside the base range")
	tmpl    = flag.String("t", "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX", "use template")
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("bt-uuidgen: ")
	flag.Parse()
	if *usebase {
		*tmpl = BASE_UUID
	}
	uuid, err := gen(*tmpl)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Println(uuid)
}

func gen(tmpl string) (uuid string, err error) {
	var c [32]byte
	n, _ := fmt.Sscanf(tmpl, "%c%c%c%c%c%c%c%c-%c%c%c%c-%c%c%c%c-%c%c%c%c-%c%c%c%c%c%c%c%c%c%c%c%c",
		&c[0], &c[1], &c[2], &c[3], &c[4], &c[5], &c[6], &c[7],
		&c[8], &c[9], &c[10], &c[11],
		&c[12], &c[13], &c[14], &c[15],
		&c[16], &c[17], &c[18], &c[19],
		&c[20], &c[21], &c[22], &c[23], &c[24], &c[25], &c[26], &c[27], &c[28], &c[29], &c[30], &c[31])
	if n != len(c) {
		err = fmt.Errorf("invalid uuid template")
		return
	}

	var b [16]byte
	_, err = rand.Read(b[:])
	if err != nil {
		return
	}

	p := []byte(fmt.Sprintf("%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		b[0], b[1], b[2], b[3],
		b[4], b[5],
		b[6], b[7],
		b[8], b[9],
		b[10], b[11], b[12], b[13], b[14], b[15]))
	j := 0
	for i := range p {
		if p[i] == '-' {
			continue
		}
		if ishex(c[j]) {
			p[i] = c[j]
		}
		j++
	}

	uuid = string(p)
	return
}

func ishex(b byte) bool {
	switch {
	case '0' <= b && b <= '9',
		'a' <= b && b <= 'f',
		'A' <= b && b <= 'F':
		return true
	}
	return false
}
