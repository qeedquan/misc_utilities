// ported from original malbolge source
// http://www.lscheffer.com/malbolge_interp.html

package main

import (
	"flag"
	"fmt"
	"io"
	"os"
	"strings"
	"unicode"
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	ip := Interp{Stdin: os.Stdin, Stdout: os.Stdout}
	for _, name := range flag.Args() {
		err := run(name, &ip)
		if err != nil {
			fmt.Fprintf(os.Stderr, "malbolge: %v\n", err)
		}
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: file ...")
	flag.PrintDefaults()
	os.Exit(1)
}

func run(name string, ip *Interp) error {
	buf, err := os.ReadFile(name)
	if err != nil {
		return err
	}

	err = ip.Load(buf)
	if err != nil {
		return err
	}

	err = ip.Exec()
	if err != nil {
		return err
	}

	return nil
}

const (
	xlat1 = "+b(29e*j1VMEKLyC})8&m#~W>qxdRp0wkrUo[D7,XTcA\"lI" +
		".v%{gJh4G\\-=O@5`_3i<?Z';FNQuY]szf$!BS/|t:Pn6^Ha"

	xlat2 = "5z]&gqtyfr$(we4{WP)H-Zn,[%\\3dL+Q;>U!pJS72FhOA1C" +
		"B6v^=I_0/8|jsb9m<.TVac`uY*MK'X~xDl}REokN:#?G\"i@"
)

type Interp struct {
	Stdin  io.Reader
	Stdout io.Writer
	mem    [59049]uint16
}

func (p *Interp) Load(b []byte) error {
	for i := range p.mem {
		p.mem[i] = 0
	}

	var i int
	for _, x := range b {
		if unicode.IsSpace(rune(x)) {
			continue
		}

		if x < 127 && x > 32 {
			if strings.IndexByte("ji*p</vo", xlat1[(int(x)-33+i)%94]) < 0 {
				return fmt.Errorf("invalid character %c in input", x)
			}
		}

		if i >= len(p.mem) {
			return fmt.Errorf("input too long")
		}

		p.mem[i], i = uint16(x), i+1
	}

	for ; i < len(p.mem); i++ {
		if i >= 3 {
			p.mem[i] = p.op(p.mem[i-1], p.mem[i-2])
		}
	}

	return nil
}

func (p *Interp) Exec() (err error) {
	var a, c, d uint16
	var x int

	defer func() {
		if e, _ := recover().(error); e != nil {
			err = e
		}
	}()

loop:
	for {
		if p.mem[c] < 33 || p.mem[c] > 126 {
			continue
		}
		switch xlat1[(p.mem[c]-33+c)%94] {
		case 'j':
			d = p.mem[d]
		case 'i':
			c = p.mem[d]
		case '*':
			p.mem[d] = p.mem[d]/3 + p.mem[d]%3*19683
			a = p.mem[d]
		case 'p':
			p.mem[d] = p.op(a, p.mem[d])
			a = p.mem[d]
		case '<':
			fmt.Fprintf(p.Stdout, "%c", a&0xff)
		case '/':
			var b [1]uint8

			_, err := p.Stdin.Read(b[:])
			x = int(b[0])

			if err == io.EOF {
				a = uint16(len(p.mem)) - 1
			} else {
				a = uint16(x)
			}
		case 'v':
			break loop
		}

		p.mem[c] = uint16(xlat2[p.mem[c]-33])
		if c++; int(c) >= len(p.mem) {
			c = 0
		}
		if d++; int(d) >= len(p.mem) {
			d = 0
		}
	}

	return nil
}

func (p *Interp) op(x, y uint16) uint16 {
	var p9 = [5]uint16{1, 9, 81, 729, 6561}
	var o = [9][9]uint16{
		{4, 3, 3, 1, 0, 0, 1, 0, 0},
		{4, 3, 5, 1, 0, 2, 1, 0, 2},
		{5, 5, 4, 2, 2, 1, 2, 2, 1},
		{4, 3, 3, 1, 0, 0, 7, 6, 6},
		{4, 3, 5, 1, 0, 2, 7, 6, 8},
		{5, 5, 4, 2, 2, 1, 8, 8, 7},
		{7, 6, 6, 7, 6, 6, 4, 3, 3},
		{7, 6, 8, 7, 6, 8, 4, 3, 5},
		{8, 8, 7, 8, 8, 7, 5, 5, 4},
	}

	i := uint16(0)
	for j := uint16(0); j < 5; j++ {
		i += o[y/p9[j]%9][x/p9[j]%9] * p9[j]
	}
	return i
}
