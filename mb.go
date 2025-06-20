package main

import (
	"bufio"
	"encoding/binary"
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"path/filepath"
	"sort"
	"strings"
)

var flags struct {
	Consecutive  bool
	Recursive    bool
	PatternFiles MultiFlag
	Files        []string
	Base         uint64
}

var (
	matcher = NewMatcher()
	status  int
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("mb: ")
	parseFlags()
	if len(flags.Files) == 0 {
		matcher.Match("<stdin>", os.Stdin, os.Stdout)
	} else {
		for _, name := range flags.Files {
			matcher.MatchFile(name)
		}
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] pattern file ...")
	flag.PrintDefaults()
	fmt.Fprintln(os.Stderr, "\nendians: b l2 b2 l4 b4 l8 b8\n")
	fmt.Fprintln(os.Stderr, "patterns (delimited by semicolons):")
	fmt.Fprintln(os.Stderr, "  a [endian] <addr> <value>:  append a value to the file")
	fmt.Fprintln(os.Stderr, "  s [endian] <value>:         searches for a value in the file")
	fmt.Fprintln(os.Stderr, "  m [endian] <addr> <value>:  modify a value in the file")
	fmt.Fprintln(os.Stderr, "  r [endian] <value>:         replace a value in the file")
	os.Exit(2)
}

func parseFlags() {
	flag.Uint64Var(&flags.Base, "b", 0, "add base when printing offset")
	flag.BoolVar(&flags.Consecutive, "c", false, "only print out when all consecutive rule matches (for searching)")
	flag.BoolVar(&flags.Recursive, "R", false, "recursive mode")
	flag.Var(&flags.PatternFiles, "l", "add pattern file")
	flag.Usage = usage
	flag.Parse()
	flags.Files = flag.Args()

	for _, name := range flags.PatternFiles {
		ek(matcher.AddPatternFile(name))
	}

	if len(flags.PatternFiles) == 0 && flag.NArg() < 1 {
		usage()
	}

	ck(matcher.AddPattern(flag.Arg(0)))
	flags.Files = flags.Files[1:]
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "mb:", err)
		status = 1
	}
	return status == 1
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

type MultiFlag []string

func (m *MultiFlag) Set(s string) error {
	*m = append(*m, s)
	return nil
}

func (m *MultiFlag) String() string {
	return fmt.Sprint(*m)
}

type Matcher struct {
	searches []Pattern
	appends  []Pattern
	modifies []Pattern
	replaces []Pattern
	sorted   bool
	rdonly   bool

	an, mn     int
	bp, np, rp int
	lm         int

	cn   int
	cons [][]string

	addr   uint64
	rbuf   [16]byte
	buf    [8]byte
	values [8]uint64

	name string
	r    *bufio.Reader
	w    *bufio.Writer
}

type Pattern struct {
	Op     uint8
	Endian int
	Addr   uint64
	Value  [2]uint64
}

type patternSlice []Pattern

func (p patternSlice) Len() int           { return len(p) }
func (p patternSlice) Swap(i, j int)      { p[i], p[j] = p[j], p[i] }
func (p patternSlice) Less(i, j int) bool { return p[i].Addr < p[j].Addr }

func NewMatcher() *Matcher {
	return &Matcher{
		rdonly: true,
	}
}

func (m *Matcher) Reset(name string, r io.Reader, w io.Writer) {
	if !m.sorted {
		sort.Sort(patternSlice(m.searches))
		sort.Sort(patternSlice(m.replaces))
		sort.Sort(patternSlice(m.appends))
		sort.Sort(patternSlice(m.modifies))
		m.cons = make([][]string, len(m.searches))
		m.sorted = true
	}

	m.name = name
	m.r = bufio.NewReader(r)
	m.w = bufio.NewWriter(w)
	m.bp = 0
	m.np = 0
	m.rp = 0
	m.an = 0
	m.mn = 0
	m.lm = 0
	m.cn = 0
	m.addr = 0
	for i := range m.buf {
		m.buf[i] = 0
	}
	for i := range m.rbuf {
		m.rbuf[i] = 0
	}
	for i := range m.values {
		m.values[i] = 0
	}
	for i := range m.cons {
		m.cons[i] = m.cons[i][:0]
	}
}

func (m *Matcher) AddPatternFile(name string) error {
	fd, err := os.Open(name)
	if err != nil {
		return err
	}
	defer fd.Close()

	scan := bufio.NewScanner(fd)
	for lineno := 1; scan.Scan(); lineno++ {
		err := m.AddPattern(scan.Text())
		if err != nil {
			fmt.Fprintf(os.Stderr, "%s:%d: %v\n", name, lineno, err)
		}
	}
	return nil
}

func (m *Matcher) AddPattern(pattern string) error {
	pattern = strings.TrimSpace(pattern)
	if pattern == "" || strings.HasPrefix(pattern, "#") {
		return nil
	}

	var list []Pattern
	var endian string

	toks := strings.Split(pattern, ";")
	for _, tok := range toks {
		tok = strings.TrimSpace(tok)
		if tok == "" {
			continue
		}
		str := tok
		if strings.IndexByte("amrs", tok[0]) < 0 {
			str = "s " + str
		}

		v := Pattern{Endian: -1}
		n, _ := fmt.Sscanf(str, "%c %v", &v.Op, &v.Value[0])
		if n == 2 && v.Op == 's' {
			list = append(list, v)
			continue
		}

		n, _ = fmt.Sscanf(str, "%c %s %v", &v.Op, &endian, &v.Value[0])
		if n == 3 && v.Op == 's' && m.setEndian(endian, &v.Endian) {
			list = append(list, v)
			continue
		}

		n, _ = fmt.Sscanf(str, "%c %s %v %v", &v.Op, &endian, &v.Value[0], &v.Value[1])
		if n == 4 && v.Op == 'r' && m.setEndian(endian, &v.Endian) {
			list = append(list, v)
			continue
		}

		n, _ = fmt.Sscanf(str, "%c %s %v %v", &v.Op, &endian, &v.Addr, &v.Value[0])
		if n == 4 && (v.Op == 'a' || v.Op == 'm') && m.setEndian(endian, &v.Endian) {
			list = append(list, v)
			continue
		}

		return fmt.Errorf("invalid syntax: %q", tok)
	}

	m.sorted = false
	for _, p := range list {
		err := m.checkPatternConflict(p)
		if err != nil {
			return err
		}

		if p.Op != 's' {
			m.rdonly = false
		}
		switch p.Op {
		case 's':
			m.searches = append(m.searches, p)
		case 'a':
			m.appends = append(m.appends, p)
		case 'm':
			m.modifies = append(m.modifies, p)
		case 'r':
			m.replaces = append(m.replaces, p)
		}
	}
	return nil
}

func (m *Matcher) isPatternConflicting(l []Pattern, p Pattern) bool {
	sizes := []uint64{1, 2, 2, 4, 4, 8, 8}
	for i := range l {
		q := &l[i]

		a := p.Addr
		b := p.Addr + sizes[p.Endian]

		d := q.Addr
		e := q.Addr + sizes[q.Endian]

		if d <= a && a <= e {
			return true
		}
		if d <= b && b <= e {
			return true
		}
	}
	return false
}

func (m *Matcher) checkPatternConflict(p Pattern) error {
	if m.isPatternConflicting(m.appends, p) {
		return fmt.Errorf("conflicts with an append pattern")
	}

	if m.isPatternConflicting(m.modifies, p) {
		return fmt.Errorf("conflicts with a modify pattern")
	}

	if m.isPatternConflicting(m.replaces, p) {
		return fmt.Errorf("conflicts with a replace pattern")
	}

	return nil
}

func (m *Matcher) setEndian(s string, v *int) bool {
	tab := []string{"b", "l2", "b2", "l4", "b4", "l8", "b8"}
	for i := range tab {
		if s == tab[i] {
			*v = i
			return true
		}
	}
	return false
}

func (m *Matcher) Match(name string, r io.Reader, w io.Writer) {
	m.Reset(name, r, w)
	defer m.w.Flush()

	for {
		b, err := m.r.ReadByte()
		if err != nil {
			break
		}
		m.step(b)
		m.addr++
	}
	m.flush(m.rp)
}

func (m *Matcher) MatchFile(name string) {
	fd, err := os.Open(name)
	if ek(err) {
		return
	}
	defer fd.Close()

	fi, err := fd.Stat()
	if ek(err) {
		return
	}

	if fi.IsDir() && flags.Recursive {
		err = filepath.Walk(name, func(path string, info os.FileInfo, err error) error {
			if info.IsDir() {
				return nil
			}
			m.MatchFile(path)
			return nil
		})
		ek(err)
	} else {
		var wfd *os.File

		dir := filepath.Dir(name)
		if m.rdonly {
			m.Match(name, fd, wfd)
			return
		}

		wfd, err = os.CreateTemp(dir, name)
		if ek(err) {
			return
		}
		m.Match(name, fd, wfd)
		os.Rename(wfd.Name(), name)
	}
}

func (m *Matcher) mod(b byte) byte {
	ranges := []uint64{0, 1, 1, 3, 3, 7, 7}
	for ; m.mn < len(m.modifies); m.mn++ {
		p := &m.modifies[m.mn]
		r := ranges[p.Endian]
		if p.Addr+r >= m.addr {
			break
		}
	}

	for i := m.mn; i < len(m.modifies); i++ {
		p := &m.modifies[i]
		r := ranges[p.Endian]
		if !(p.Addr <= m.addr && m.addr <= p.Addr+r) {
			break
		}
		a := p.Addr + r - m.addr
		v := m.getval(p.Endian, p.Value[0])

		fmt.Printf("modified location %#x (new %d %#x) (old %d %#x)\n",
			m.addr-p.Addr, v[a], v[a], b, b)

		b = v[a]
	}

	return b
}

func (m *Matcher) step(b byte) {
	b = m.mod(b)
	copy(m.buf[0:], m.buf[1:])
	m.buf[7] = b
	if m.bp < len(m.buf) {
		m.bp++
	}

	m.np = 0
	switch n := m.bp; {
	case n >= 8:
		m.np = 7
		m.values[5] = binary.LittleEndian.Uint64(m.buf[:])
		m.values[6] = binary.BigEndian.Uint64(m.buf[:])
		fallthrough
	case n >= 4:
		m.np = max(m.np, 5)
		m.values[3] = uint64(binary.LittleEndian.Uint32(m.buf[4:]))
		m.values[4] = uint64(binary.BigEndian.Uint32(m.buf[4:]))
		fallthrough
	case n >= 2:
		m.np = max(m.np, 3)
		m.values[1] = uint64(binary.LittleEndian.Uint16(m.buf[6:]))
		m.values[2] = uint64(binary.BigEndian.Uint16(m.buf[6:]))
		fallthrough
	default:
		m.np = max(m.np, 1)
		m.values[0] = uint64(m.buf[7])
	}
	m.search()
	m.append()
	m.replace(b)
}

func (m *Matcher) resetConsecutive() {
	for i := range m.cons {
		m.cons[i] = m.cons[i][:0]
	}
	m.cn = 0
}

func (m *Matcher) queueSearchResult(index int, texts []string) {
	if !flags.Consecutive {
		for i := range texts {
			fmt.Print(texts[i])
		}
		return
	}

	if len(texts) == 0 {
		m.resetConsecutive()
	} else {
		m.cons[m.cn] = texts
		m.cn++
	}

	if m.cn == len(m.searches) {
		for i := range m.cons {
			for j := range m.cons[i] {
				fmt.Print(m.cons[i][j])
			}
		}
		m.resetConsecutive()
	}
}

func (m *Matcher) search() {
	tab := []string{" b", "l2", "b2", "l4", "b4", "l8", "b8"}
	off := []uint64{0, 1, 1, 3, 3, 7, 7}

	var texts []string
	for i := m.cn; i < len(m.searches); i++ {
		var text string
		p := &m.searches[i]
		switch {
		case p.Endian >= 0 && p.Endian < m.np && p.Value[0] == m.values[p.Endian]:
			if p.Value[0] == m.values[p.Endian] {
				addr := m.addr - off[p.Endian]
				text = fmt.Sprintf("%s: found %s: (%d | %#x) at (%d | %#x)\n",
					m.name, tab[p.Endian], p.Value[0], p.Value[0], flags.Base+addr, flags.Base+addr)
				texts = append(texts, text)
			}

		case p.Endian == -1:
			for j := 0; j < m.np; j++ {
				if p.Value[0] == m.values[j] {
					addr := m.addr - off[j]
					text = fmt.Sprintf("%s: found %s: (%d | %#x) at (%d | %#x)\n",
						m.name, tab[j], p.Value[0], p.Value[0], flags.Base+addr, flags.Base+addr)
					texts = append(texts, text)
				}
			}
		}

		m.queueSearchResult(i, texts)
		if flags.Consecutive {
			break
		}
	}
}

func (m *Matcher) append() {
	for i := m.an; i < len(m.appends); i++ {
		p := &m.appends[i]
		if p.Addr > m.addr {
			break
		}
		if p.Addr == m.addr {
			m.writeval(p.Endian, p.Value[0])
		}
	}
}

func (m *Matcher) replace(b byte) {
	if len(m.replaces) == 0 && m.w != nil {
		m.w.WriteByte(b)
		return
	}

	m.rbuf[m.rp] = b
	m.rp++

	sizes := []int{1, 2, 2, 4, 4, 8, 8}
	var lm int
	for i := range m.replaces {
		p := &m.replaces[i]
		if p.Endian < m.np {
			if p.Value[0] == m.values[p.Endian] {
				m.flush(m.rp - sizes[p.Endian])
				m.writeval(p.Endian, p.Value[1])
				m.rp = 0
				return
			} else {
				x := m.getval(p.Endian, p.Value[0])
				y := m.getval(p.Endian, m.values[p.Endian])
				l := longestMatch(x, y)
				lm = max(lm, l)
			}
		}
	}

	if m.lm > lm {
		m.flush(1)
	}
	m.lm = lm
}

func (m *Matcher) flush(n int) {
	n = max(0, min(n, m.rp))
	if m.w != nil {
		for i := 0; i < n; i++ {
			m.w.WriteByte(m.rbuf[i])
		}
	}
	copy(m.rbuf[:n], m.rbuf[n+1:])
	m.rp -= n
}

func (m *Matcher) writeval(e int, v uint64) {
	if m.w == nil {
		return
	}
	p := m.getval(e, v)
	m.w.Write(p)
}

func (m *Matcher) getval(e int, v uint64) []byte {
	var p [8]byte
	var n int
	switch e {
	case 0:
		p[0] = uint8(v)
		n = 1
	case 1:
		binary.LittleEndian.PutUint16(p[:], uint16(v))
		n = 2
	case 2:
		binary.BigEndian.PutUint16(p[:], uint16(v))
		n = 2
	case 3:
		binary.LittleEndian.PutUint32(p[:], uint32(v))
		n = 4
	case 4:
		binary.BigEndian.PutUint32(p[:], uint32(v))
		n = 4
	case 5:
		binary.LittleEndian.PutUint64(p[:], v)
		n = 8
	case 6:
		binary.BigEndian.PutUint64(p[:], v)
		n = 8
	}
	return p[:n]
}

func longestMatch(x, y []byte) int {
	n := min(len(x), len(y))
	m := 0
	for i := 0; i < n; i++ {
		if x[i] != y[i] {
			break
		}
		m++
	}
	return m
}
