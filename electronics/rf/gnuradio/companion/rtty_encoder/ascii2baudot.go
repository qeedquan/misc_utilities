/*

Generate baudot codes for GNU Radio.
The generated output is a binary stream that can be fed into a BFSK encoder to generate a waveform that can be decoded by minimodem.

Notes:
For each character that is outputted, the shift code is prepended unless disabled (this is what minimodem does)

The repeat rate is what determines how long 1 bit is, the equation is as follows:

sampling_rate/baud_rate

For 48 khz sampling rate and 45.45 bps per second (standard rtty), we have
48000/45.45 = ~1056 samples for 1 bit of data.

The fractional delay needed by start/stop bits is implemented by scaling the sample rate of 1 bit by that value, so for the standard 1.5 bits stopbit:
1056 * 1.5 = 1584

Fldigi assumes stopbit to be 1.5 bits, so fractional delay is needed.
Minimodem supports integer stopbits.

*/

package main

import (
	"bufio"
	"encoding/binary"
	"encoding/csv"
	"flag"
	"fmt"
	"io"
	"log"
	"math/bits"
	"os"
	"strconv"
	"strings"
	"unicode"
)

type Charmap struct {
	name     string
	chars    map[rune][2]int
	codepage *Codepage
}

type Codepage struct {
	bits    int
	shifts  []byte
	symbols [][]rune
}

type Flags struct {
	endian     binary.ByteOrder
	charmap    *Charmap
	syncbytes  []byte
	mark       int
	space      int
	leaderbits float64
	startbits  float64
	stopbits   float64
	repeat     float64
	invert     bool
	reverse    bool
	shift      bool
}

var flags Flags

func main() {
	log.SetFlags(0)
	log.SetPrefix("ascii2baudot: ")
	parseflags()

	var r io.Reader = os.Stdin
	if flag.NArg() >= 1 {
		r = strings.NewReader(strings.Join(flag.Args(), " "))
	}

	err := convert(r, os.Stdout, &flags)
	if err != nil {
		log.Fatal(err)
	}
}

func parseflags() {
	var format, endian, syncbytes string
	flag.StringVar(&endian, "endian", "le", "endianess")
	flag.StringVar(&format, "format", "ita2", "output format")
	flag.StringVar(&syncbytes, "syncbytes", "", "comma separated list of synchronization bytes")
	flag.IntVar(&flags.mark, "mark", 1, "mark value")
	flag.IntVar(&flags.space, "space", 0, "space value")
	flag.Float64Var(&flags.leaderbits, "leaderbits", 2, "number of leader bits")
	flag.Float64Var(&flags.startbits, "startbits", 1, "number of start bits")
	flag.Float64Var(&flags.stopbits, "stopbits", 1.5, "number of stop bits")
	flag.Float64Var(&flags.repeat, "repeat", 48e3/45.45, "repeat rate")
	flag.BoolVar(&flags.invert, "invert", false, "invert mark and space output")
	flag.BoolVar(&flags.reverse, "reverse", false, "reverse bit output")
	flag.BoolVar(&flags.shift, "shift", true, "output shift codes before character")

	flag.Usage = usage
	flag.Parse()

	flags.syncbytes = csv2bytes(syncbytes)

	flags.endian = binary.LittleEndian
	if endian != "le" {
		flags.endian = binary.BigEndian
	}

	var err error
	flags.charmap, err = newcharmap(format)
	if err != nil {
		log.Fatalf("unsupported character map format: %q", format)
	}

	flags.space = clamp(flags.space, 0, 1)
	flags.mark = clamp(flags.mark, 0, 1)
}

func csv2bytes(s string) []byte {
	r := csv.NewReader(strings.NewReader(s))
	p, err := r.ReadAll()
	if err != nil {
		return nil
	}

	var b []byte
	for i := range p {
		for j := range p[i] {
			v, err := strconv.ParseInt(p[i][j], 0, 8)
			if err == nil {
				b = append(b, byte(v))
			}
		}
	}
	return b
}

func clamp(x, a, b int) int {
	if x < a {
		x = a
	} else if x > b {
		x = b
	}
	return x
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] [text]")
	flag.PrintDefaults()
	fmt.Fprintln(os.Stderr)
	fmt.Fprintln(os.Stderr, "supported codes: ascii ita1 ita2 ustty")
	os.Exit(2)
}

func newcharmap(s string) (ch *Charmap, err error) {
	var cp *Codepage
	switch s {
	case "ascii":
		cp = ascii
	case "ita1":
		cp = ita1
	case "ita2":
		cp = ita2
	case "ustty":
		cp = ustty
	default:
		err = fmt.Errorf("invalid codepage: %q", s)
		return
	}

	ch = &Charmap{
		name:     s,
		chars:    make(map[rune][2]int),
		codepage: cp,
	}
	for i, l := range cp.symbols {
		for j, r := range l {
			ch.chars[r] = [2]int{i, j}
		}
	}

	return
}

func convert(r io.Reader, w io.Writer, fl *Flags) error {
	br := bufio.NewReader(r)
	bw := bufio.NewWriter(w)

	ch := fl.charmap
	cp := ch.codepage

	repeat(bw, fl.leaderbits, marker(fl.invert, fl.space, fl.mark), fl)

	for _, v := range fl.syncbytes {
		writeframe(bw, v, cp.bits, fl)
	}

	for {
		v, _, err := br.ReadRune()
		if err == io.EOF {
			break
		}
		if err != nil {
			return err
		}

		err = writevalue(bw, unicode.ToUpper(v), fl)
		if err != nil {
			return err
		}
	}
	return bw.Flush()
}

func writevalue(w *bufio.Writer, v rune, fl *Flags) error {
	ch := fl.charmap
	cp := ch.codepage

	i, ok := ch.chars[v]
	if !ok {
		return fmt.Errorf("unsupported character %q", v)
	}

	if fl.shift && i[0] < len(cp.shifts) {
		writeframe(w, cp.shifts[i[0]], cp.bits, fl)
	}
	writeframe(w, byte(i[1]), cp.bits, fl)

	return nil
}

func writeframe(w *bufio.Writer, v byte, n int, fl *Flags) {
	if fl.reverse {
		v = bits.Reverse8(v)
	}

	repeat(w, fl.startbits, marker(fl.invert, fl.mark, fl.space), fl)

	for i := 0; i < n; i++ {
		x := fl.space
		if v&(1<<i) != 0 {
			x = fl.mark
		}
		repeat(w, 1, float32(x), fl)
	}

	repeat(w, fl.stopbits, marker(fl.invert, fl.space, fl.mark), fl)
}

func marker(b bool, x, y int) float32 {
	if b {
		return float32(x)
	}
	return float32(y)
}

func repeat(w *bufio.Writer, r float64, v float32, fl *Flags) {
	for i := 0; i < int(fl.repeat*r); i++ {
		binary.Write(w, fl.endian, v)
	}
}

// https://en.wikipedia.org/wiki/Baudot_code

func init() {
	ascii = &Codepage{
		bits: 7,
		symbols: [][]rune{
			make([]rune, 128),
		},
	}
	for i := range ascii.symbols[0] {
		ascii.symbols[0][i] = rune(i)
	}
}

var ascii *Codepage

var ita1 = &Codepage{
	bits:   5,
	shifts: []byte{0x10, 0x08},
	symbols: [][]rune{
		{
			0, 'A', 'E', '\r', 'Y', 'U', 'I', 'O', 0xe, 'J', 'G', 'H', 'B', 'C', 'F', 'D',
			' ', '\n', 'X', 'Z', 'S', 'T', 'W', 'V', 0x7f, 'K', 'M', 'L', 'R', 'Q', 'N', 'P',
		},

		{
			0, '1', '2', '\r', '3', '4', 0xc0, '5', ' ', '6', '7', '+', '8', '9', 0xc1, '0',
			0xf, '\n', ',', ':', '.', 0xc0, '?', '\'', 0x7f, '(', ')', '=', '-', '/', 0xc1, '%',
		},
	},
}

var ita2 = &Codepage{
	bits:   5,
	shifts: []byte{0x1f, 0x1b},
	symbols: [][]rune{
		{
			0, 'E', '\n', 'A', ' ', 'S', 'I', 'U', '\r', 'D', 'R', 'J', 'N', 'F', 'C', 'K',
			'T', 'Z', 'L', 'W', 'H', 'Y', 'P', 'Q', 'O', 'B', 'G', 0xe, 'M', 'X', 'V', 0xf,
		},

		{
			0, '3', '\n', '-', ' ', '\'', '8', '7', '\r', 5, '4', '\b', ',', '!', ':', '(',
			'5', '+', ')', '2', '£', '6', '0', '1', '9', '?', '&', 0xe, '.', '/', '=', 0xf,
		},
	},
}

var ustty = &Codepage{
	bits:   5,
	shifts: []byte{0x1f, 0x1b},
	symbols: [][]rune{
		{
			0, 'E', '\n', 'A', ' ', 'S', 'I', 'U', '\r', 'D', 'R', 'J', 'N', 'F', 'C', 'K',
			'T', 'Z', 'L', 'W', 'H', 'Y', 'P', 'Q', 'O', 'B', 'G', 0xe, 'M', 'X', 'V', 0xf,
		},

		{
			0, '3', '\n', '-', ' ', '\a', '8', '7', '\r', '$', '4', '\'', ',', '!', ':', '(',
			'5', '"', ')', '2', '#', '6', '0', '1', '9', '?', '&', 0xe, '.', '/', '=', 0xf,
		},
	},
}
