// ported from https://github.com/wolfram77/js-4004-assembler
package main

import (
	"flag"
	"fmt"
	"os"
	"path/filepath"
	"regexp"
	"strconv"
	"strings"
)

var (
	status = 0
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	for _, name := range flag.Args() {
		ek(assemble(name))
	}

	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: file ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func ek(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "as:", err)
		status = 1
	}
}

func assemble(name string) error {
	buf, err := os.ReadFile(name)
	if err != nil {
		return err
	}

	re := regexp.MustCompile("[0-9A-z]+")
	lines := strings.Split(string(buf), "\n")

	var code []byte
loop:
	for i, line := range lines {
		toks := re.FindAllString(line, -1)
		if len(toks) == 0 {
			continue
		}

		op := strings.ToUpper(toks[0])
		args := []int64{}
		for _, arg := range toks[1:] {
			n, err := strconv.ParseInt(arg, 0, 64)
			if err != nil {
				continue loop
			}
			args = append(args, n)
		}
		if err := validate(op, args); err != nil {
			return fmt.Errorf("%s: %d: %v", name, i+1, err)
		}
		code = gen(code, op, args)
	}

	ext := filepath.Ext(name)
	out := name[:len(name)-len(ext)] + ".bin"
	if out == name {
		out += ".bin"
	}
	return os.WriteFile(out, code, 0644)
}

func validate(op string, args []int64) error {
	x, found := opcodes[op]
	if !found {
		return fmt.Errorf("no such opcode %q", op)
	}
	if len(x.par) != len(args) {
		return fmt.Errorf("parameter count mismatch (%d != %d)", len(x.par), len(args))
	}
	for i := range args {
		p := params[x.par[i]]
		if args[i] >= 1<<p.shift {
			return fmt.Errorf("parameter %q overflow (%d bits)", p.str, p.shift)
		}
	}
	return nil
}

func gen(code []byte, op string, args []int64) []byte {
	x := opcodes[op]
	o := append([]byte{}, x.enc...)
	for i := range args {
		insert(o, args[i], params[x.par[i]].bits)
	}
	return append(code, o...)
}

func insert(arr []byte, val int64, pos uint) {
	i := pos >> 3
	o := pos & 7
	for val != 0 {
		arr[i] |= byte((val << o) & 0xff)
		val >>= 8 - o
		o = 0
		i--
	}
}

type param struct {
	bits  uint
	shift uint
	str   string
}

var params = [256]param{
	'c': {0, 4, "condition"},
	'r': {0, 4, "register"},
	'R': {1, 3, "register pair"},
	'd': {0, 4, "immediate data"},
	'D': {8, 8, "rom data"},
	'a': {8, 8, "short rom address"},
	'A': {8, 12, "long rom address"},
}

type opcode struct {
	enc []byte
	par string
}

var opcodes = map[string]opcode{
	"NOP": {[]byte{0}, ""},
	"JCN": {[]byte{0x10, 0x00}, "ca"},
	"FIM": {[]byte{0x20, 0x00}, "RD"},
	"FIN": {[]byte{0x30}, "R"},
	"JIN": {[]byte{0x31}, "R"},
	"JUN": {[]byte{0x40, 0x00}, "A"},
	"JMS": {[]byte{0x50, 0x00}, "A"},
	"INC": {[]byte{0x60}, "r"},
	"ISZ": {[]byte{0x70, 0x00}, "ra"},
	"ADD": {[]byte{0x80}, "r"},
	"SUB": {[]byte{0x90}, "r"},
	"LD":  {[]byte{0xA0}, "r"},
	"XCH": {[]byte{0xB0}, "r"},
	"BBL": {[]byte{0xC0}, "d"},
	"LDM": {[]byte{0xD0}, "d"},
	"CLB": {[]byte{0xF0}, ""},
	"CLC": {[]byte{0xF1}, ""},
	"IAC": {[]byte{0xF2}, ""},
	"CMC": {[]byte{0xF3}, ""},
	"RAL": {[]byte{0xF5}, ""},
	"RAR": {[]byte{0xF6}, ""},
	"TCC": {[]byte{0xF7}, ""},
	"DAC": {[]byte{0xF8}, ""},
	"TCS": {[]byte{0xF9}, ""},
	"STC": {[]byte{0xFA}, ""},
	"DAA": {[]byte{0xFB}, ""},
	"KBP": {[]byte{0xFC}, ""},
	"DCL": {[]byte{0xFD}, ""},
	// io and ram
	"SRC": {[]byte{0x21}, "R"},
	"WRM": {[]byte{0xE0}, ""},
	"WMP": {[]byte{0xE1}, ""},
	"WRR": {[]byte{0xE2}, ""},
	"WPM": {[]byte{0xE3}, ""},
	"WR0": {[]byte{0xE4}, ""},
	"WR1": {[]byte{0xE5}, ""},
	"WR2": {[]byte{0xE6}, ""},
	"WR3": {[]byte{0xE7}, ""},
	"SBM": {[]byte{0xE8}, ""},
	"RDM": {[]byte{0xE9}, ""},
	"RDR": {[]byte{0xEA}, ""},
	"ADM": {[]byte{0xEB}, ""},
	"RD0": {[]byte{0xEC}, ""},
	"RD1": {[]byte{0xED}, ""},
	"RD2": {[]byte{0xEE}, ""},
	"RD3": {[]byte{0xEF}, ""},
}
