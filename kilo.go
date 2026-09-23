// ported from https://github.com/antirez/kilo
package main

import (
	"bufio"
	"bytes"
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"strings"
	"time"
	"unicode"

	"github.com/qeedquan/go-media/posix"
	"golang.org/x/sys/unix"
)

const (
	KILO_VERSION = "0.0.1"
)

const (
	HL_NORMAL = iota
	HL_NONPRINT
	HL_COMMENT
	HL_MLCOMMENT
	HL_KEYWORD1
	HL_KEYWORD2
	HL_STRING
	HL_NUMBER
	HL_MATCH
)

const (
	HL_HIGHLIGHT_STRINGS = 1 << iota
	HL_HIGHLIGHT_NUMBERS
)

const (
	KEY_NULL  = 0   // NULL
	CTRL_C    = 3   // Ctrl-c
	CTRL_D    = 4   // Ctrl-d
	CTRL_F    = 6   // Ctrl-f
	CTRL_H    = 8   // Ctrl-h
	TAB       = 9   // Tab
	CTRL_L    = 12  // Ctrl-l
	ENTER     = 13  // Enter
	CTRL_Q    = 17  // Ctrl-q
	CTRL_S    = 19  // Ctrl-s
	CTRL_U    = 21  // Ctrl-u
	ESC       = 27  // Escape
	BACKSPACE = 127 // Backspace
)

const (
	// The following are just soft codes, not really reported by the
	// terminal directly
	ARROW_LEFT = 1000 + iota
	ARROW_RIGHT
	ARROW_UP
	ARROW_DOWN
	DEL_KEY
	HOME_KEY
	END_KEY
	PAGE_UP
	PAGE_DOWN
)

var (
	C_HL_extensions = []string{".c", ".cpp"}
	C_HL_keywords   = []string{
		// a few C/C++ keywords
		"switch", "if", "while", "for", "break", "case", "continue", "return", "else",
		"struct", "union", "typedef", "static", "enum", "class",

		// C types
		"int|", "long|", "double|", "float|", "char|", "unsigned|", "signed|",
		"void|", "const|", "size_t|", "ssize_t|", "restrict|", "loff_t|", "time_t|",
		"uintptr_t|", "uintptr|", "u8|", "u16|", "u32|", "u64|", "s8|", "s16|", "s32|",
		"s64|", "bool|", "short|", "unsigned|",
	}

	GO_HL_extensions = []string{".go"}
	GO_HL_keywords   = []string{
		"func", "var", "switch", "if", "for", "break", "case", "continue", "return", "else",
		"struct", "map", "type", "defer", "fallthrough", "package", "import",

		"byte|", "error|", "string|",
		"int|", "int8|", "int16|", "int32|", "int64|",
		"uint|", "uint8|", "uint16|", "uint32|", "uint64|",
		"float32|", "float64|",
	}

	// here we define an array of syntax highlights by extensions, keywords,
	// comments delimiters and flags
	HLDB = []Syntax{
		// C / C++
		{C_HL_extensions, C_HL_keywords, "//", "/*", "*/", HL_HIGHLIGHT_STRINGS | HL_HIGHLIGHT_NUMBERS},

		// Go
		{GO_HL_extensions, GO_HL_keywords, "//", "/*", "*/", HL_HIGHLIGHT_STRINGS | HL_HIGHLIGHT_NUMBERS},
	}

	E Editor
)

func main() {
	log.SetPrefix("")
	log.SetFlags(0)
	flag.IntVar(&E.tabstop, "tabstop", 4, "tabstop")
	flag.BoolVar(&E.safetybelt, "safetybelt", false, "apply safety belt")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 1 {
		usage()
	}

	E.Init()
	E.SelectSyntaxHighlight(flag.Arg(0))
	E.Open(flag.Arg(0))
	E.EnableRawMode(os.Stdin)
	E.SetStatusMessage("HELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F find")
	for {
		E.RefreshScreen()
		E.ProcessKeypress(os.Stdin)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: kilo [options] filename")
	flag.PrintDefaults()
	os.Exit(2)
}

func exit(err error) {
	E.DisableRawMode(os.Stdin)
	os.Stdout.WriteString("\x1b[2J\x1b[H")
	status := 0
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		status = 1
	}
	os.Exit(status)
}

func reallocBytes(b []byte, n int) []byte {
	if len(b) >= n {
		return b
	}
	return append(b, make([]byte, n-len(b))...)
}

func reallocRows(r []Row, n int) []Row {
	if len(r) >= n {
		return r
	}
	return append(r, make([]Row, n-len(r))...)
}

func memset(s []byte, c byte, n int) {
	for i := 0; i < n; i++ {
		s[i] = c
	}
}

type Editor struct {
	cx, cy         int    // cursor x and y position in characters
	rowoff         int    // offset of row displayed
	coloff         int    // offset of column displayed
	screenrows     int    // number of rows we can show
	screencols     int    // number of cols we can show
	numrows        int    // number of rows in use
	quit_times     int    // number of times tried to quit
	rawmode        bool   // is terminal raw mode enabled
	row            []Row  // rows
	dirty          bool   // file modified but not saved
	filename       string // currently open filename
	statusmsg      string
	statusmsg_time time.Time
	syntax         *Syntax
	tabstop        int
	safetybelt     bool
	orig_termios   unix.Termios
}

type Row struct {
	idx    int
	size   int
	rsize  int
	chars  []byte
	render []byte
	hl     []byte // syntax highlight type for each character in render
	hl_oc  bool   // row had open comment at end in last syntax highlight check
}

type Syntax struct {
	filematch                []string
	keywords                 []string
	singleline_comment_start string
	multiline_comment_start  string
	multiline_comment_end    string
	flags                    int
}

func (E *Editor) Init() {
	var err error
	E.cx = 0
	E.cy = 0
	E.rowoff = 0
	E.coloff = 0
	E.numrows = 0
	E.row = nil
	E.dirty = false
	E.filename = ""
	E.syntax = nil
	if E.tabstop <= 0 {
		E.tabstop = 4
	}
	E.screenrows, E.screencols, err = E.getWindowSize(os.Stdin, os.Stdout)
	if err != nil {
		log.Fatal("Unable to query the screen for size (columns / rows)")
	}
	// get room for status bar
	E.screenrows -= 2
}

// try to get the number of columns in the current terminal. If the ioctl()
// call fails the function will try to query the terminal itself
func (E *Editor) getWindowSize(ifd, ofd *os.File) (rows, cols int, err error) {
	ws, err := unix.IoctlGetWinsize(unix.Stdout, unix.TIOCGWINSZ)
	if err != nil || ws.Col == 0 {
		var orig_row, orig_col int

		// ioctl failed. try to query terminal itself
		orig_row, orig_col, err = E.getCursorPosition(ifd, ofd)
		if err != nil {
			return
		}

		_, err = ofd.Write([]byte("\x1b[999C\x1b[999B"))
		if err != nil {
			return
		}
		rows, cols, err = E.getCursorPosition(ifd, ofd)
		if err != nil {
			return
		}

		// restore position
		seq := fmt.Sprintf("\x1b[%d;%dH", orig_row, orig_col)
		ofd.Write([]byte(seq))
	} else {
		rows, cols = int(ws.Row), int(ws.Col)
	}

	return
}

func (E *Editor) getCursorPosition(ifd, ofd *os.File) (rows, cols int, err error) {
	// report cursor location
	_, err = ofd.Write([]byte("\x1b[6n"))
	if err != nil {
		return
	}

	// read the response: esc [ rows ; cols R
	var buf [32]byte
	for i := 0; i < len(buf)-1; i++ {
		_, err := ifd.Read(buf[i : i+1])
		if err != nil {
			break
		}
		if buf[i] == 'R' {
			break
		}
	}

	// parse it
	if buf[0] != ESC || buf[1] != '[' {
		err = fmt.Errorf("Unable to query cursor position string")
		return
	}
	n, _ := fmt.Sscanf(string(buf[2:]), "%d;%d", &rows, &cols)
	if n != 2 {
		err = fmt.Errorf("Unable to get row/col from cursor position string")
	}
	return
}

func (E *Editor) SelectSyntaxHighlight(filename string) {
	for _, s := range HLDB {
		for i := range s.filematch {
			if strings.Index(filename, s.filematch[i]) >= 0 {
				if !strings.HasPrefix(s.filematch[i], ".") || strings.HasSuffix(filename, s.filematch[i]) {
					E.syntax = &s
					return
				}
			}
		}
	}
}

func (E *Editor) DisableRawMode(f *os.File) {
	// don't even check the return value as it's too late
	if E.rawmode {
		fd := int(f.Fd())
		posix.Tcsetattr(fd, unix.TCSAFLUSH, &E.orig_termios)
		E.rawmode = false
	}
}

// raw mode: 1960 magic shit
func (E *Editor) EnableRawMode(f *os.File) bool {
	// already enabled
	if E.rawmode {
		return true
	}

	if !posix.Isatty(posix.STDIN_FILENO) {
		return false
	}

	fd := int(f.Fd())
	err := posix.Tcgetattr(fd, &E.orig_termios)
	if err != nil {
		return false
	}

	// modify the original mode
	raw := E.orig_termios
	// input modes: no break, no CR to NL, no parity check, no strip char, no start/stop output control
	raw.Iflag &^= (unix.BRKINT | unix.ICRNL | unix.INPCK | unix.ISTRIP | unix.IXON)
	// output modes - disable post processing
	raw.Oflag &^= unix.OPOST
	// control modes - set 8 bit chars
	raw.Cflag |= unix.CS8
	// local modes - echoing off, canonical off, no extend functions, no signal chars
	raw.Lflag &^= (unix.ECHO | unix.ICANON | unix.IEXTEN | unix.ISIG)
	// control chars - set return condition: min number of bytes and timer
	// return each byte, or zero for timeout
	raw.Cc[unix.VMIN] = 0
	// 100 ms timeout (units is in tens of second)
	raw.Cc[unix.VTIME] = 1

	// put terminal in raw mode after flushing
	err = posix.Tcsetattr(fd, unix.TCSAFLUSH, &raw)
	if err != nil {
		return false
	}
	E.rawmode = true

	return true
}

// load the specified program in the editor memory
func (E *Editor) Open(filename string) {
	E.dirty = false
	E.filename = filename

	f, err := os.OpenFile(filename, os.O_RDWR|os.O_CREATE, 0644)
	if err != nil {
		f, err = os.Open(filename)
	}
	if err != nil {
		log.Fatal(err)
	}
	defer f.Close()
	r := bufio.NewReader(f)

	for {
		l, err := r.ReadBytes('\n')
		if err != nil {
			break
		}
		l = bytes.TrimSuffix(l, []byte("\n"))
		l = bytes.TrimSuffix(l, []byte("\r"))
		E.insertRow(E.numrows, l)
	}
	E.dirty = false
}

// insert a row at the specified position, shifting the other rows on
// the bottom if required
func (E *Editor) insertRow(at int, s []byte) {
	if at > E.numrows {
		return
	}
	E.row = reallocRows(E.row, E.numrows+1)
	if at != E.numrows {
		copy(E.row[at+1:], E.row[at:])
		for i := at + 1; i <= E.numrows; i++ {
			E.row[i].idx++
		}
	}
	E.row[at].size = len(s)
	E.row[at].chars = append([]byte{}, s...)
	E.row[at].hl = nil
	E.row[at].hl_oc = false
	E.row[at].render = nil
	E.row[at].rsize = 0
	E.row[at].idx = at
	E.updateRow(&E.row[at])
	E.numrows++
	E.dirty = true
}

// remove the row at the specified position, shifting the remaining on the top
func (E *Editor) delRow(at int) {
	if at >= E.numrows {
		return
	}
	copy(E.row[at:], E.row[at+1:])
	for i := at; i < E.numrows-1; i++ {
		E.row[i].idx++
	}
	E.numrows--
	E.dirty = true
}

// turn the editor rows into a single string
func (E *Editor) rowsToString() []byte {
	var p []byte
	for i := 0; i < E.numrows; i++ {
		r := &E.row[i]
		p = append(p, r.chars[:r.size]...)
		p = append(p, '\n')
	}
	return p
}

// insert a character at the specified position in a row,
// moving the remaining chars on the right if needed
func (E *Editor) rowInsertChar(row *Row, at, c int) {
	if at > row.size {
		// pad the string with spaces if the insert location
		// is outside the current length by more than
		// a single character
		padlen := at - row.size
		// in the next line +1 means: new char
		row.chars = reallocBytes(row.chars, row.size+padlen+1)
		memset(row.chars[row.size:], ' ', padlen)
		row.size += padlen
	} else {
		// if we are in the middle of the string just
		// make space for 1 new char
		row.chars = reallocBytes(row.chars, row.size+1)
		copy(row.chars[at+1:], row.chars[at:])
		row.size++
	}

	row.chars[at] = byte(c)
	E.updateRow(row)
	E.dirty = true
}

// append the string 's' at the end of the row
func (E *Editor) rowAppendString(row *Row, s []byte) {
	row.chars = reallocBytes(row.chars, row.size+len(s))
	copy(row.chars[row.size:], s)
	row.size += len(s)
	E.updateRow(row)
	E.dirty = true
}

// delete the character at offset 'at' from the specified row
func (E *Editor) rowDelChar(row *Row, at int) {
	if row.size <= at {
		return
	}
	copy(row.chars[at:], row.chars[at+1:])
	E.updateRow(row)
	row.size--
	E.dirty = true
}

// update the rendered version and syntax highlight of a row
func (E *Editor) updateRow(row *Row) {
	// create a version of the row we can directly print on screen
	// respecting tabs, substituting non-printable characters with '?'
	tabs := 0
	nonprint := 0
	for i := range row.chars {
		if row.chars[i] == TAB {
			tabs++
		}
	}

	row.render = make([]byte, row.size+tabs*E.tabstop+nonprint*9)
	idx := 0
	for i := 0; i < row.size; i++ {
		if row.chars[i] == TAB {
			row.render[idx], idx = ' ', idx+1
			for (idx+1)%E.tabstop != 0 {
				row.render[idx], idx = ' ', idx+1
			}
		} else {
			row.render[idx], idx = row.chars[i], idx+1
		}
	}
	row.rsize = idx
	// update the syntax highlighting attributes of the row
	E.updateSyntax(row)
}

func isSeparator(c byte) bool {
	return c == 0 || unicode.IsSpace(rune(c)) || strings.IndexByte(",.()+-/*=~%[];", c) >= 0
}

// return true if the specified row last char is part of a multi line comment
// that starts at this row or at one before, and does not end at the end
// of the row but spawns to the next row
func (E *Editor) rowHasOpenComment(row *Row) bool {
	if row.hl != nil && row.rsize != 0 && row.hl[row.rsize-1] == HL_MLCOMMENT &&
		(row.rsize < 2 || (row.render[row.rsize-2] != '*' || row.render[row.rsize-1] != '/')) {
		return true
	}
	return false
}

// set every byte of row.hl (that corresponds to every character in line to the right
// syntax highlight type (HL_* defines)
func (E *Editor) updateSyntax(row *Row) {
	row.hl = reallocBytes(row.hl, row.rsize)
	memset(row.hl, HL_NORMAL, row.rsize)

	if E.syntax == nil {
		return
	}

	keywords := E.syntax.keywords
	scs := []byte(E.syntax.singleline_comment_start)
	mcs := []byte(E.syntax.multiline_comment_start)
	mce := []byte(E.syntax.multiline_comment_end)

	// point to the first non-space char
	p := row.render
	// current char offset
	i := 0
	for len(p) > 0 && unicode.IsSpace(rune(p[0])) {
		p, i = p[1:], i+1
	}

	// tell the parser if 'i' points to start of word
	prev_sep := true
	// are we inside "" or '' ?
	in_string := byte(0)
	// are we inside multi-line comment?
	in_comment := false

	// if the previous line has an open comment, this
	// line starts with an open comment state
	if row.idx > 0 && E.rowHasOpenComment(&E.row[row.idx-1]) {
		in_comment = true
	}

	for len(p) > 0 {
		if len(row.hl) <= i {
			row.hl = reallocBytes(row.hl, i+1)
		}
		// handle // comments
		if prev_sep && bytes.HasPrefix(p, scs) {
			// from here to end is a comment
			memset(row.hl[i:], HL_COMMENT, row.size-i)
			return
		}

		// handle multi-line comments
		if in_comment {
			row.hl[i] = HL_MLCOMMENT
			if bytes.HasPrefix(p, mce) {
				row.hl[i+1] = HL_MLCOMMENT
				p, i = p[2:], i+2
				in_comment = false
				prev_sep = true
				continue
			} else {
				prev_sep = false
				p, i = p[1:], i+1
				continue
			}
		} else if bytes.HasPrefix(p, mcs) {
			row.hl[i] = HL_MLCOMMENT
			row.hl[i+1] = HL_MLCOMMENT
			p, i = p[2:], i+2
			in_comment = true
			prev_sep = false
			continue
		}

		// handle "" and ''
		if in_string != 0 {
			row.hl[i] = HL_STRING
			if len(p) >= 2 && p[0] == '\\' {
				row.hl[i+1] = HL_STRING
				p, i = p[2:], i+2
				prev_sep = false
				continue
			}
			if p[0] == in_string {
				in_string = 0
			}
			p, i = p[1:], i+1
			continue
		} else {
			if p[0] == '"' || p[0] == '\'' {
				in_string = p[0]
				row.hl[i] = HL_STRING
				p, i = p[1:], i+1
				prev_sep = false
				continue
			}
		}

		// handle non-printable chars
		if !unicode.IsPrint(rune(p[0])) {
			row.hl[i] = HL_NONPRINT
			p, i = p[1:], i+1
			prev_sep = false
			continue
		}

		// handle numbers
		if (unicode.IsDigit(rune(p[0])) && (prev_sep || row.hl[i-1] == HL_NUMBER)) || (p[0] == '.' && i > 0 && row.hl[i-1] == HL_NUMBER) {
			row.hl[i] = HL_NUMBER
			p, i = p[1:], i+1
			prev_sep = false
			continue
		}

		// handle keywords and lib calls
		if prev_sep {
			found := false
			for j := range keywords {
				klen := len(keywords[j])
				kw2 := keywords[j][klen-1] == '|'
				if kw2 {
					klen--
				}
				keyword := []byte(keywords[j][:klen])
				if len(p) >= klen && bytes.Compare(p[:len(keyword)], keyword) == 0 && (len(p) == klen || isSeparator(p[klen])) {
					// keyword
					kval := byte(HL_KEYWORD1)
					if kw2 {
						kval = HL_KEYWORD2
					}
					memset(row.hl[i:], kval, klen)
					p, i = p[klen:], i+klen
					found = true
					break
				}
			}
			if found {
				prev_sep = false
				// we had a keyword match
				continue
			}
		}

		// not special chars
		prev_sep = isSeparator(p[0])
		p, i = p[1:], i+1
	}

	// propagate syntax change to the next row if the open comment state
	// changed. this may recursively affect all the following rows in the file
	oc := E.rowHasOpenComment(row)
	if row.hl_oc != oc && row.idx+1 < E.numrows {
		E.updateSyntax(&E.row[row.idx+1])
	}
	row.hl_oc = oc
}

func (E *Editor) syntaxToColor(hl byte) int {
	switch hl {
	case HL_COMMENT:
		fallthrough
	case HL_MLCOMMENT: // cyan
		return 36
	case HL_KEYWORD1: // yellow
		return 33
	case HL_KEYWORD2: // green
		return 32
	case HL_STRING: // magenta
		return 35
	case HL_NUMBER: // red
		return 31
	case HL_MATCH: // blue
		return 34
	default: // white
		return 37
	}
}

func (E *Editor) FileWasModified() bool {
	return E.dirty
}

func (E *Editor) RefreshScreen() {
	ab := new(bytes.Buffer)

	// hide cursor
	ab.WriteString("\x1b[?25l")
	// go home
	ab.WriteString("\x1b[H")

	for y := 0; y < E.screenrows; y++ {
		// status bar
		filerow := E.rowoff + y
		if filerow >= E.numrows {
			if E.numrows == 0 && y == E.screenrows/3 {
				welcome := fmt.Sprintf("Kilo editor -- version %s\x1b[0K\r\n", KILO_VERSION)
				padding := (E.screencols - len(welcome)) / 2
				if padding != 0 {
					ab.WriteString("~")
					padding--
				}
				for ; padding != 0; padding-- {
					ab.WriteString(" ")
				}
				ab.WriteString(welcome)
			} else {
				ab.WriteString("~\x1b[0K\r\n")
			}
			continue
		}

		// syntax highlighting
		r := E.row[filerow]
		len_ := r.rsize - E.coloff
		current_color := -1
		if len_ > 0 {
			if len_ > E.screencols {
				len_ = E.screencols
			}
			c := r.render[E.coloff:]
			hl := r.hl[E.coloff:]
			for i := 0; i < len_; i++ {
				switch hl[i] {
				case HL_NONPRINT:
					ab.WriteString("\x1b[7m")
					sym := byte('?')
					if c[i] <= 26 {
						sym = '@' + c[i]
					}
					ab.WriteByte(sym)
					ab.WriteString("\x1b[0m")
				case HL_NORMAL:
					if current_color != -1 {
						ab.WriteString("\x1b[39m")
						current_color = -1
					}
					ab.WriteByte(c[i])
				default:
					color := E.syntaxToColor(hl[i])
					if color != current_color {
						buf := fmt.Sprintf("\x1b[%dm", color)
						current_color = color
						ab.WriteString(buf)
					}
					ab.WriteByte(c[i])
				}
			}
		}
		ab.WriteString("\x1b[39m")
		ab.WriteString("\x1b[0K")
		ab.WriteString("\r\n")
	}

	// create two rows status, first row
	ab.WriteString("\x1b[0K")
	ab.WriteString("\x1b[7m")
	dirty := ""
	if E.dirty {
		dirty = "(modified)"
	}
	status := fmt.Sprintf("%.20s - %d lines %s", E.filename, E.numrows, dirty)
	rstatus := fmt.Sprintf("%d/%d", E.rowoff+E.cy+1, E.numrows)
	len_ := len(status)
	rlen := len(rstatus)
	if len_ > E.screencols {
		len_ = E.screencols
	}
	ab.WriteString(status[:len_])
	for len_ < E.screencols {
		if E.screencols-len_ == rlen {
			ab.WriteString(rstatus)
			break
		} else {
			ab.WriteString(" ")
			len_++
		}
	}
	ab.WriteString("\x1b[0m\r\n")

	// second row depends on E.statusmsg and the status message update time
	ab.WriteString("\x1b[0K")
	if len(E.statusmsg) != 0 && time.Since(E.statusmsg_time) < 5*time.Second {
		msglen := len(E.statusmsg)
		if msglen > E.screencols {
			msglen = E.screencols
		}
		ab.WriteString(E.statusmsg[:msglen])
	}

	// put cursor at its current position.
	// note that the horizontal position
	// at which the cursor is displayed may be
	// different compared to E.cx because of TABs
	cx := 1
	filerow := E.rowoff + E.cy
	var row *Row
	if filerow < E.numrows {
		row = &E.row[filerow]
	}
	if row != nil {
		for i := E.coloff; i < (E.cx + E.coloff); i++ {
			if i < row.size && row.chars[i] == TAB {
				cx += (E.tabstop - 1) - (cx % E.tabstop)
			}
			cx++
		}
	}
	buf := fmt.Sprintf("\x1b[%d;%dH", E.cy+1, cx)
	ab.WriteString(buf)
	// show cursor
	ab.WriteString("\x1b[?25h")
	os.Stdout.Write(ab.Bytes())
}

func (E *Editor) moveCursor(key int) {
	filerow := E.rowoff + E.cy
	filecol := E.coloff + E.cx
	var row *Row
	var rowlen int
	if filerow < E.numrows {
		row = &E.row[filerow]
	}

	switch key {
	case ARROW_LEFT:
		if E.cx == 0 {
			if E.coloff != 0 {
				E.coloff--
			} else {
				if filerow > 0 {
					E.cy--
					E.cx = E.row[filerow-1].size
					if E.cx > E.screencols-1 {
						E.coloff = E.cx - E.screencols + 1
						E.cx = E.screencols - 1
					}
				}
			}
		} else {
			E.cx--
		}
	case ARROW_RIGHT:
		if row != nil && filecol < row.size {
			if E.cx == E.screencols-1 {
				E.coloff++
			} else {
				E.cx++
			}
		} else if row != nil && filecol == row.size {
			E.cx = 0
			E.coloff = 0
			if E.cy == E.screenrows-1 {
				E.rowoff++
			} else {
				E.cy++
			}
		}
	case ARROW_UP:
		if E.cy == 0 {
			if E.rowoff != 0 {
				E.rowoff--
			}
		} else {
			E.cy--
		}
	case ARROW_DOWN:
		if filerow < E.numrows {
			if E.cy == E.screenrows-1 {
				E.rowoff++
			} else {
				E.cy++
			}
		}
	}

	// fix cx if the current line has not enough chars
	filerow = E.rowoff + E.cy
	filecol = E.coloff + E.cx
	row = nil
	rowlen = 0
	if filerow < E.numrows {
		row = &E.row[filerow]
		rowlen = row.size
	}
	if filecol > rowlen {
		E.cx -= filecol - rowlen
		if E.cx < 0 {
			E.coloff += E.cx
			E.cx = 0
		}
	}
}

func (E *Editor) ProcessKeypress(f *os.File) {
	const KILO_QUIT_TIMES = 3

	c := E.readKey(f)
	switch c {
	case ENTER: // Enter
		E.insertNewline()
	case CTRL_C: // Ctrl-c
		// we ignore ctrl-c, it can't be so simple to lose changes to edited file
	case CTRL_Q: // Ctrl-q
		// quit if the file was already saved
		if E.safetybelt && E.dirty && E.quit_times != 0 {
			E.SetStatusMessage("WARNING!!! File has unsaved changes. Press Q %d more times to quit.", E.quit_times)
			E.quit_times--
			return
		}
		exit(nil)
	case CTRL_S: // Ctrl-s
		E.save()
	case CTRL_F:
		E.find(f)
	case BACKSPACE, // Backspace
		CTRL_H, // Ctrl-h
		DEL_KEY:
		E.delChar()
	case PAGE_UP, PAGE_DOWN:
		if c == PAGE_UP && E.cy != 0 {
			E.cy = 0
		} else if c == PAGE_DOWN && E.cy != E.screenrows-1 {
			E.cy = E.screenrows - 1
		}
		for times := E.screenrows; times != 0; times-- {
			pos := ARROW_DOWN
			if c == PAGE_UP {
				pos = ARROW_UP
			}
			E.moveCursor(pos)
		}
	case ARROW_UP, ARROW_DOWN, ARROW_LEFT, ARROW_RIGHT:
		E.moveCursor(c)
	case CTRL_L: // Ctrl-l, clear screen
		// just refresh this line as side effect
	case ESC:
		// nothing to do for ESC in this mode
	default:
		E.insertChar(c)
	}

	E.quit_times = KILO_QUIT_TIMES
}

// read a key from the terminal put in raw mode, trying to handle
// escape sequences
func (E *Editor) readKey(f *os.File) int {
	var seq [3]byte
	var c byte
	for {
		n, err := f.Read(seq[0:1])
		if n == 1 {
			c = seq[0]
			break
		}
		if err == io.EOF {
			continue
		}
		if err != nil {
			exit(err)
		}
	}

	for {
		switch c {
		case ESC: // escape sequence
			// if this is just an ESC, we'll timeout here
			n, _ := f.Read(seq[0:1])
			if n == 0 {
				return ESC
			}
			n, _ = f.Read(seq[1:2])
			if n == 0 {
				return ESC
			}

			// ESC [ sequences
			switch seq[0] {
			case '[':
				switch {
				case '0' <= seq[1] && seq[1] <= '9':
					// extended escape, read additional byte
					n, _ = f.Read(seq[2:3])
					if n == 0 {
						return ESC
					}
					if seq[2] == '~' {
						switch seq[1] {
						case '3':
							return DEL_KEY
						case '5':
							return PAGE_UP
						case '6':
							return PAGE_DOWN
						}
					}

				default:
					switch seq[1] {
					case 'A':
						return ARROW_UP
					case 'B':
						return ARROW_DOWN
					case 'C':
						return ARROW_RIGHT
					case 'D':
						return ARROW_LEFT
					case 'H':
						return HOME_KEY
					case 'F':
						return END_KEY
					}
				}

			case 'O': // ESC 0 sequencues
				switch seq[1] {
				case 'H':
					return HOME_KEY
				case 'F':
					return END_KEY
				}
			}
		default:
			return int(c)
		}
	}
	return 0
}

// set an editor status message for the second line of the status,
// at the end of screen
func (E *Editor) SetStatusMessage(format string, args ...interface{}) {
	E.statusmsg = fmt.Sprintf(format, args...)
	E.statusmsg_time = time.Now()
}

// insert the specified char at current prompt position
func (E *Editor) insertChar(c int) {
	filerow := E.rowoff + E.cy
	filecol := E.coloff + E.cx
	var row *Row
	if filerow < E.numrows {
		row = &E.row[filerow]
	}

	// if the row where the cursor is currently located does not exist
	// in our logical representation of the file, add enough empty rows
	// as needed
	if row == nil {
		for E.numrows <= filerow {
			E.insertRow(E.numrows, nil)
		}
	}
	row = &E.row[filerow]
	E.rowInsertChar(row, filecol, c)
	if E.cx == E.screencols-1 {
		E.coloff++
	} else {
		E.cx++
	}
	E.dirty = true
}

// inserting a newline is slightly complex as we have to handle insert a
// newline in the middle of a line, splitting the line as needed
func (E *Editor) insertNewline() {
	fixcursor := func() {
		if E.cy == E.screenrows-1 {
			E.rowoff++
		} else {
			E.cy++
		}
		E.cx = 0
		E.coloff = 0
	}

	filerow := E.rowoff + E.cy
	filecol := E.coloff + E.cx
	var row *Row
	if filerow < E.numrows {
		row = &E.row[filerow]
	}
	if row == nil {
		if filerow == E.numrows {
			E.insertRow(filerow, nil)
			fixcursor()
		}
		return
	}

	// if the cursor is over the current line size, we want to conceptually
	// think it's just over the last character.
	if filecol >= row.size {
		filecol = row.size
	}
	if filecol == 0 {
		E.insertRow(filerow, nil)
	} else {
		// we are in the middle of a line
		// split it between two rows
		E.insertRow(filerow+1, row.chars[filecol:row.size])
		row = &E.row[filerow]
		row.size = filecol
		E.updateRow(row)
	}
	fixcursor()
}

// delete the char at the current prompt position
func (E *Editor) delChar() {
	filerow := E.rowoff + E.cy
	filecol := E.coloff + E.cx
	var row *Row
	if filerow < E.numrows {
		row = &E.row[filerow]
	}

	if row == nil || (filecol == 0 && filerow == 0) {
		return
	}
	if filecol == 0 {
		// handle the case of column 0, we need to move
		// the current line on the right of the previous one
		filecol = E.row[filerow-1].size
		E.rowAppendString(&E.row[filerow-1], row.chars[:row.size])
		E.delRow(filerow)
		row = nil
		if E.cy == 0 {
			E.rowoff--
		} else {
			E.cy--
		}
		E.cx = filecol
		if E.cx >= E.screencols {
			shift := (E.screencols - E.cx) + 1
			E.cx -= shift
			E.coloff += shift
		}
	} else {
		E.rowDelChar(row, filecol-1)
		if E.cx == 0 && E.coloff != 0 {
			E.coloff--
		} else {
			E.cx--
		}
	}
	if row != nil {
		E.updateRow(row)
	}
	E.dirty = true
}

// save the current file on disk
func (E *Editor) save() error {
	buf := E.rowsToString()
	err := os.WriteFile(E.filename, buf, 0644)
	if err != nil {
		E.SetStatusMessage("Can't save! I/O error: %v", err)
		return err
	}
	E.dirty = false
	E.SetStatusMessage("%d bytes written on disk", len(buf))
	return nil
}

func (E *Editor) find(f *os.File) {
	// if 1 search next, if -1 search prev
	find_next := -1
	// no saved hl
	saved_hl_line := -1

	var saved_hl []byte
	findRestoreHl := func() {
		if saved_hl != nil {
			copy(E.row[saved_hl_line].hl, saved_hl[:E.row[saved_hl_line].rsize])
			saved_hl = nil
		}
	}

	// last line where a match was found. -1 for none
	last_match := -1
	// save the cursor position in order to restore it later
	saved_cx := E.cx
	saved_cy := E.cy
	saved_coloff := E.coloff
	saved_rowoff := E.rowoff
	var query []byte
	for {
		E.SetStatusMessage("Search: %s (Use ESC/Arrows/Enter)", query)
		E.RefreshScreen()

		c := E.readKey(f)
		switch {
		case c == DEL_KEY || c == CTRL_H || c == BACKSPACE:
			if l := len(query); l > 0 {
				query = query[:l-1]
				last_match = -1
			}
		case c == ESC || c == ENTER:
			if c == ESC {
				E.cx = saved_cx
				E.cy = saved_cy
				E.coloff = saved_coloff
				E.rowoff = saved_rowoff
			}
			findRestoreHl()
			E.SetStatusMessage("")
			return
		case c == ARROW_RIGHT || c == ARROW_DOWN:
			find_next = 1
		case c == ARROW_LEFT || c == ARROW_UP:
			find_next = -1
		case unicode.IsPrint(rune(c)):
			query = append(query, byte(c))
			last_match = -1
		}

		// search occurence
		if last_match == -1 {
			find_next = 1
		}
		if find_next != 0 {
			var match []byte
			match_offset := 0
			current := last_match
			for i := 0; i < E.numrows; i++ {
				current += find_next
				if current == -1 {
					current = E.numrows - 1
				} else if current == E.numrows {
					current = 0
				}
				match_offset = bytes.Index(E.row[current].render, query)
				if match_offset >= 0 {
					match = E.row[current].render[match_offset:]
				}
				if match != nil {
					break
				}
			}
			find_next = 0

			// highlight
			findRestoreHl()

			if match != nil {
				row := &E.row[current]
				last_match = current
				if row.hl != nil {
					saved_hl_line = current
					saved_hl = make([]byte, row.rsize)
					copy(saved_hl, row.hl)
					memset(row.hl[match_offset:], HL_MATCH, len(query))
				}
				E.cy = 0
				E.cx = match_offset
				E.rowoff = current
				E.coloff = 0
				// scroll horizontally as needed
				if E.cx > E.screencols {
					diff := E.cx - E.screencols
					E.cx -= diff
					E.coloff += diff
				}
			}
		}
	}
}
