// https://esolangs.org/wiki/4

package main

import (
	"flag"
	"fmt"
	"io"
	"os"
	"text/scanner"
	"unicode"
)

var (
	status = 0
)

func main() {
	flag.Parse()
	if flag.NArg() < 1 {
		run("-", os.Stdin)
	} else {
		for _, name := range flag.Args() {
			fd, err := os.Open(name)
			if ek(err) {
				continue
			}
			run(name, fd)
			fd.Close()
		}
	}
	os.Exit(status)
}

func run(name string, r io.Reader) {
	src, err := io.ReadAll(r)
	if ek(err) {
		return
	}

	s4 := NewS4(&Stdio{os.Stdin, os.Stdout})
	ek(s4.Interpret(name, src))
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "s4:", err)
		status = 1
		return true
	}
	return false
}

func isnum(ch int) bool {
	return '0' <= ch && ch <= '9'
}

type Stdio struct {
	io.Reader
	io.Writer
}

type inst struct {
	pos scanner.Position
	op  int
	arg [3]int
}

type S4 struct {
	pos   scanner.Position
	src   []byte
	state int
	stdio *Stdio
	inst  []inst
	Cells [100]int
}

func NewS4(stdio *Stdio) *S4 {
	return &S4{
		stdio: stdio,
	}
}

func (p *S4) errf(format string, args ...interface{}) {
	text := fmt.Sprintf(format, args...)
	panic(fmt.Errorf("%v: %v", p.pos, text))
}

func (p *S4) Interpret(name string, src []byte) (err error) {
	defer func() {
		if e := recover(); e != nil {
			err = e.(error)
		}
	}()

	p.pos = scanner.Position{
		Filename: name,
		Line:     1,
		Column:   1,
	}
	p.src = src

	c1 := p.getch()
	c2 := p.getch()
	if c1 != '3' || c2 != '.' {
		p.errf("syntax error: missing beginning marker")
	}

	for {
		pos := p.pos
		op := p.getch()
		if !isnum(op) {
			p.errf("syntax error: unexpected EOF, missing end marker")
		}
		op -= '0'

		p.inst = append(p.inst, inst{pos, op, p.arg(op)})
		if op == 4 {
			break
		}
	}
	p.exec()

	return
}

func (p *S4) getch0() int {
	s := &p.pos
	if s.Offset >= len(p.src) {
		return -1
	}
	ch := p.src[s.Offset]

	if ch == '\n' {
		s.Line++
		s.Column = 1
	} else {
		s.Column++
	}
	s.Offset++
	return int(ch)
}

func (p *S4) getch() int {
	for {
		ch := p.getch0()
		if unicode.IsSpace(rune(ch)) {
			continue
		}
		return ch
	}
}

func (p *S4) getarg() int {
	hi := p.getch()
	lo := p.getch()
	if !isnum(lo) || !isnum(hi) {
		p.errf("syntax error: expected variable argument")
	}
	return (lo - '0') + 10*(hi-'0')
}

func (p *S4) arg(op int) (arg [3]int) {
	switch op {
	case 0, 1, 2, 3:
		arg[0] = p.getarg()
		arg[1] = p.getarg()
		arg[2] = p.getarg()
	case 6:
		arg[0] = p.getarg()
		arg[1] = p.getarg()
	case 5, 7, 8:
		arg[0] = p.getarg()
	case 4, 9:
	default:
		p.errf("syntax error: invalid op %d", op)

	}
	return
}

func (p *S4) exec() {
	c := p.Cells[:]
	t := p.inst[:]
	for i := 0; i < len(t); i++ {
		w := &t[i]
		x, y, z := w.arg[0], w.arg[1], w.arg[2]

		p.pos = w.pos

		switch w.op {
		case 0:
			c[x] = c[y] + c[z]
		case 1:
			c[x] = c[y] - c[z]
		case 2:
			c[x] = c[y] * c[z]
		case 3:
			c[x] = c[y] / c[z]
		case 4:
			return
		case 5:
			fmt.Fprintf(p.stdio, "%c", c[x])
		case 6:
			c[x] = y
		case 7:
			fmt.Fscanf(p.stdio, "%c", &c[x])
		case 8:
			if c[x] == 0 {
				for n := 1; n > 0; {
					if i++; i >= len(t) {
						p.errf("syntax error: unmatched loop")
					}

					if t[i].op == 8 {
						n++
					} else if t[i].op == 9 {
						n--
					}
				}
			}
		case 9:
			for n := 1; n > 0; {
				if i--; i < 0 {
					p.errf("syntax error: unmatched loop")
				}

				if t[i].op == 9 {
					n++
				} else if t[i].op == 8 {
					n--
				}
			}
		}
	}
}
