// ported from BSD printf

package main

import (
	"flag"
	"fmt"
	"io"
	"os"
	"strconv"
	"strings"
	"unicode/utf8"
)

func main() {
	flag.Usage = usage
	flag.Parse()

	args := flag.Args()
	if len(args) < 1 {
		usage()
	}

	p := Printer{}
	rval := p.Format(os.Stdout, warn, args[0], args[1:]...)
	os.Exit(rval)
}

func warn(format string, args ...interface{}) {
	fmt.Fprintf(os.Stderr, "printf: "+format+"\n", args...)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: format [arguments] ...")
	flag.PrintDefaults()
	os.Exit(1)
}

const eof = -1

type Printer struct {
	w              io.Writer
	errf           func(string, ...interface{})
	format         string
	args           []string
	rval           int
	haveFieldWidth bool
	havePrecision  bool
	fieldWidth     int
	precision      int
	count          int
}

func (p *Printer) Format(w io.Writer, errf func(string, ...interface{}), format string, args ...string) int {
	const skip1 = "#-+ 0"
	const skip2 = "0123456789"

	p.w = w
	p.errf = errf
	p.args = args
	p.rval = 0

	for {
		p.format = format

	loop:
		for {
			start := p.format

			switch r := p.next(); r {
			case eof:
				break loop

			case '%':
				p.count = 0

				r = p.next()
				if r == '%' {
					fmt.Fprintf(p.w, "%%")
					break
				}

				if r == 'b' {
					if p.printEscapeString(p.getString()) {
						return 1
					}
					break
				}

				// skip to field width
				for {
					if strings.IndexRune(skip1, r) < 0 {
						break
					}
					r = p.next()
				}
				if r == '*' {
					r = p.next()
					p.haveFieldWidth = true
					p.fieldWidth = p.getInt()
				} else {
					p.haveFieldWidth = false
				}

				// skip to field precision
				for {
					if strings.IndexRune(skip2, r) < 0 {
						break
					}
					r = p.next()
				}
				p.havePrecision = false
				if r == '.' {
					r = p.next()
					if r == '*' {
						r = p.next()
						p.havePrecision = true
						p.precision = p.getInt()
					}
					for {
						if strings.IndexRune(skip2, r) < 0 {
							break
						}
						r = p.next()
					}
				}

				if r == eof {
					p.errf("missing format character")
					return 1
				}
				start = start[:p.count+1]

				convch := r
				switch convch {
				case 'c':
					ch := p.getChar()
					p.printField(start, ch)

				case 's':
					s := p.getString()
					p.printField(start, s)

				case 'd', 'i':
					n := p.getInt64()
					p.printField(start, n)

				case 'o', 'u', 'x', 'X':
					n := p.getUint64()
					p.printField(start, n)

				case 'a', 'A', 'e', 'E', 'f', 'F', 'g', 'G':
					n := p.getFloat64()
					p.printField(start, n)

				default:
					p.errf("invalid directive")
					return 1
				}

			case '\\':
				p.printEscape()

			default:
				fmt.Fprintf(p.w, "%c", r)
			}
		}

		if len(p.args) == len(args) || len(p.args) == 0 {
			break
		}
	}

	return p.rval
}

func (p *Printer) peek() rune {
	if len(p.format) == 0 {
		return eof
	}
	r, _ := utf8.DecodeRuneInString(p.format)
	return r
}

func (p *Printer) next() rune {
	if len(p.format) == 0 {
		return eof
	}
	r, size := utf8.DecodeRuneInString(p.format)
	p.format = p.format[size:]
	p.count += size
	return r
}

func (p *Printer) printField(format string, arg interface{}) {
	switch {
	case p.haveFieldWidth:
		if p.havePrecision {
			fmt.Printf(format, p.fieldWidth, p.precision, arg)
		} else {
			fmt.Printf(format, p.fieldWidth, arg)
		}

	case p.havePrecision:
		fmt.Printf(format, p.precision, arg)

	default:
		fmt.Printf(format, arg)
	}
}

func (p *Printer) printEscapeString(str string) bool {
	q := Printer{w: p.w, errf: p.errf, format: str}
	for {
		r := q.peek()
		switch r {
		case eof:
			return false

		case '\\':
			q.next()
			r = q.peek()

			switch r {
			case '0':
				q.next()
				v := 0
				for i := 0; i < 3; i++ {
					r = q.peek()
					if !isOctal(r) {
						break
					}
					v <<= 3
					v += int(oct2bin(r))
				}
				fmt.Fprintf(q.w, "%c", v)
				continue

			case 'c':
				return true

			default:
				q.printEscape()
			}

		default:
			fmt.Fprintf(q.w, "%c", r)
		}

		q.next()
	}
}

func (p *Printer) printEscape() {
	switch r := p.next(); r {
	case '0', '1', '2', '3', '4', '5', '6', '7':
		v, i := 0, 0
		for {
			v <<= 3
			v += int(oct2bin(r))
			r = p.peek()
			if !isOctal(r) {
				break
			}

			if i++; i == 3 {
				break
			}

			p.next()
		}
		fmt.Fprintf(p.w, "%c", v)

	case 'x':
		atLeastOne := false
		v := 0
		for {
			r = p.peek()
			if !isHex(r) {
				break
			}

			v <<= 4
			v += int(hex2bin(r))

			atLeastOne = true

			p.next()
		}

		if !atLeastOne {
			p.errf("'\\x' used with no following digits")
		} else if v > 0xff {
			p.errf("escape sequence out of range for character")
		}
		fmt.Fprintf(p.w, "%c", v)

	case '\\':
		fmt.Fprint(p.w, "\\")

	case '\'':
		fmt.Fprint(p.w, "'")

	case '"':
		fmt.Fprint(p.w, "\"")

	case 'a':
		fmt.Fprint(p.w, "\a")

	case 'b':
		fmt.Fprint(p.w, "\b")

	case 'e':
		fmt.Fprint(p.w, 033)

	case 'f':
		fmt.Fprint(p.w, "\f")

	case 'n':
		fmt.Fprint(p.w, "\n")

	case 'r':
		fmt.Fprint(p.w, "\r")

	case 't':
		fmt.Fprint(p.w, "\t")

	case 'v':
		fmt.Fprint(p.w, "\v")

	case '\000':
		p.errf("null escape sequence")
		p.rval = 1

	default:
		fmt.Fprintf(p.w, "%c", r)
		p.errf("unknown escape sequence '\\%c'", r)
		p.rval = 1
	}
}

func (p *Printer) getChar() rune {
	if len(p.args) == 0 {
		return eof
	}

	r, _ := utf8.DecodeRuneInString(p.args[0])
	p.args = p.args[1:]
	return r
}

func (p *Printer) getString() string {
	if len(p.args) == 0 {
		return ""
	}

	str := p.args[0]
	p.args = p.args[1:]

	return str
}

func (p *Printer) getInt() int {
	if len(p.args) == 0 {
		return 0
	}

	str := p.args[0]
	p.args = p.args[1:]

	n, _ := strconv.Atoi(str)
	return n
}

func (p *Printer) getInt64() int64 {
	if len(p.args) == 0 {
		return 0
	}

	str := p.args[0]
	p.args = p.args[1:]

	n, err := strconv.ParseInt(str, 10, 64)
	if err != nil {
		p.errf("%s: %v", str, err)
	}
	return n
}

func (p *Printer) getUint64() uint64 {
	if len(p.args) == 0 {
		return 0
	}

	str := p.args[0]
	p.args = p.args[1:]

	n, err := strconv.ParseUint(str, 10, 64)
	if err != nil {
		p.errf("%s: %v", str, err)
	}
	return n
}

func (p *Printer) getFloat64() float64 {
	if len(p.args) == 0 {
		return 0
	}

	str := p.args[0]
	p.args = p.args[1:]

	if len(str) == 0 {
		return 0
	}

	if str[0] == '"' || str[0] == '\'' {
		if len(str) < 1 {
			return 0
		}
		return float64(str[1])
	}

	n, err := strconv.ParseFloat(str, 64)
	if err != nil {
		p.errf("%s: %v", str, err)
	}

	return n
}

func isOctal(r rune) bool {
	return '0' <= r && r <= '7'
}

func isHex(r rune) bool {
	return '0' <= r && r <= '9' ||
		'A' <= r && r <= 'F' ||
		'a' <= r && r <= 'f'
}

func oct2bin(r rune) rune {
	return r - '0'
}

func hex2bin(r rune) rune {
	switch {
	case 'A' <= r && r <= 'F':
		return r - 'A' + 10
	case 'a' <= r && r <= 'f':
		return r - 'a' + 10
	default:
		return r - '0'
	}
}
