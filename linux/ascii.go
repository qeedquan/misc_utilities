// ported from plan9 ascii

package main

import (
	"bufio"
	"flag"
	"fmt"
	"os"
)

var str = [256]string{
	"nul", "soh", "stx", "etx", "eot", "enq", "ack", "bel",
	"bs ", "ht ", "nl ", "vt ", "np ", "cr ", "so ", "si ",
	"dle", "dc1", "dc2", "dc3", "dc4", "nak", "syn", "etb",
	"can", "em ", "sub", "esc", "fs ", "gs ", "rs ", "us ",
	"sp ", " ! ", " \" ", " # ", " $ ", " % ", " & ", " ' ",
	" ( ", " ) ", " * ", " + ", " , ", " - ", " . ", " / ",
	" 0 ", " 1 ", " 2 ", " 3 ", " 4 ", " 5 ", " 6 ", " 7 ",
	" 8 ", " 9 ", " : ", " ; ", " < ", " = ", " > ", " ? ",
	" @ ", " A ", " B ", " C ", " D ", " E ", " F ", " G ",
	" H ", " I ", " J ", " K ", " L ", " M ", " N ", " O ",
	" P ", " Q ", " R ", " S ", " T ", " U ", " V ", " W ",
	" X ", " Y ", " Z ", " [ ", " \\ ", " ] ", " ^ ", " _ ",
	" ` ", " a ", " b ", " c ", " d ", " e ", " f ", " g ",
	" h ", " i ", " j ", " k ", " l ", " m ", " n ", " o ",
	" p ", " q ", " r ", " s ", " t ", " u ", " v ", " w ",
	" x ", " y ", " z ", " { ", " | ", " } ", " ~ ", "del",
	"x80", "x81", "x82", "x83", "x84", "x85", "x86", "x87",
	"x88", "x89", "x8a", "x8b", "x8c", "x8d", "x8e", "x8f",
	"x90", "x91", "x92", "x93", "x94", "x95", "x96", "x97",
	"x98", "x99", "x9a", "x9b", "x9c", "x9d", "x9e", "x9f",
	"xa0", " ¡ ", " ¢ ", " £ ", " ¤ ", " ¥ ", " ¦ ", " § ",
	" ¨ ", " © ", " ª ", " « ", " ¬ ", " ­ ", " ® ", " ¯ ",
	" ° ", " ± ", " ² ", " ³ ", " ´ ", " µ ", " ¶ ", " · ",
	" ¸ ", " ¹ ", " º ", " » ", " ¼ ", " ½ ", " ¾ ", " ¿ ",
	" À ", " Á ", " Â ", " Ã ", " Ä ", " Å ", " Æ ", " Ç ",
	" È ", " É ", " Ê ", " Ë ", " Ì ", " Í ", " Î ", " Ï ",
	" Ð ", " Ñ ", " Ò ", " Ó ", " Ô ", " Õ ", " Ö ", " × ",
	" Ø ", " Ù ", " Ú ", " Û ", " Ü ", " Ý ", " Þ ", " ß ",
	" à ", " á ", " â ", " ã ", " ä ", " å ", " æ ", " ç ",
	" è ", " é ", " ê ", " ë ", " ì ", " í ", " î ", " ï ",
	" ð ", " ñ ", " ò ", " ó ", " ô ", " õ ", " ö ", " ÷ ",
	" ø ", " ù ", " ú ", " û ", " ü ", " ý ", " þ ", " ÿ ",
}

var Ncol = []byte{
	0, 0, 7, 5, 4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
}

var (
	_8flag = flag.Bool("8", false, "8 bit clean")
	xflag  = flag.Bool("x", false, "hex output")
	oflag  = flag.Bool("o", false, "octal output")
	dflag  = flag.Bool("d", false, "decimal output")
	nflag  = flag.Bool("n", false, "force numeric output")
	cflag  = flag.Bool("c", false, "force character output")
	tflag  = flag.Bool("t", false, "convert from numbers to running text; do not interpret control characters or insert newlines")
	bflag  = flag.Int("b", 16, "base")

	nchars = 128
	base   = 16
	ncol   int
	text   = 1
	strip  = 0
	bin    = bufio.NewWriter(os.Stdout)
)

func main() {
	flag.Usage = usage
	flag.Parse()

	if *_8flag {
		nchars = 256
	}
	if *xflag {
		base = 16
	} else if *oflag {
		base = 8
	} else if *dflag {
		base = 10
	} else {
		base = *bflag
	}
	if *nflag {
		text = 0
	} else if *cflag {
		text = 2
	}
	if *tflag {
		strip = 1
		text = 2
	}

	ncol = int(Ncol[base])
	narg := flag.NArg()
	if narg == 0 {
		for i := 0; i < nchars; i++ {
			put(i)
			if i&7 == 7 {
				bin.WriteString("\n")
			}
		}
	} else {
		if text == 1 {
			text = isnum(flag.Arg(0))
		}
		for i := 0; i < narg; i++ {
			if text != 0 {
				puttext(flag.Arg(i))
			} else {
				putnum(flag.Arg(i))
			}
		}
	}
	bin.Flush()
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [-8] [-xod | -b8] [-ncst] [--] [text]")
	flag.PrintDefaults()
	os.Exit(1)
}

func put(i int) {
	bin.WriteByte('|')
	putn(i, ncol)
	bin.WriteString(" " + str[i])
}

const dig = "0123456789abcdefghijklmopqrstuvwxyz"

func putn(n, ndig int) {
	if ndig == 0 {
		return
	}
	putn(n/base, ndig-1)
	bin.WriteByte(dig[n%base])
}

func isnum(s string) int {
	for _, c := range s {
		if value(c, 1) == -1 {
			return 0
		}
	}
	return 1
}

func value(c rune, f int) int {
	for i := 0; i < base; i++ {
		s := dig[i]
		if rune(s) == c {
			return i
		}
	}
	if f != 0 {
		return -1
	}
	fmt.Fprintf(os.Stderr, "ascii: bad input char %v", c)
	os.Exit(1)
	return 0
}

func btoi(s string) int {
	n := 0
	for _, c := range s {
		n = n*base + value(c, 0)
	}
	return n
}

func puttext(s string) {
	n := btoi(s) & 0377
	if strip != 0 {
		bin.WriteByte(byte(n))
	} else {
		bin.WriteString(str[n] + "\n")
	}
}

func putnum(s string) {
	for _, c := range s {
		putn(int(c&0377), ncol)
		bin.WriteByte('\n')
	}
}
