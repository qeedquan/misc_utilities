package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"strconv"
	"text/scanner"
)

func main() {
	var (
		disasm = flag.Bool("d", false, "show disassembly")
		trace  = flag.Bool("t", false, "trace program")
	)
	log.SetFlags(0)
	log.SetPrefix("whitespace: ")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 1 {
		usage()
	}

	vm := NewVM()
	err := vm.LoadProg(flag.Arg(0))
	ck(err)

	if *disasm {
		for _, ip := range vm.Inst {
			if ip.Op != LABEL {
				fmt.Printf("\t")
			}
			fmt.Printf("%s\n", vm.Disasm(ip, false))
		}
		return
	}

	vm.Trace = *trace
	vm.Run()
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] file")
	flag.PrintDefaults()
	os.Exit(2)
}

type Token int

const (
	EOF = iota
	NOP

	PUSH
	DUP
	SWAP
	DISCARD

	ADD
	SUB
	MUL
	DIV
	MOD

	STORE
	RETRIEVE

	LABEL
	CALL
	JMP
	JZ
	JN
	RET
	HLT

	OUTC
	OUTN
	INC
	INN
)

func (t Token) String() string {
	switch t {
	case EOF:
		return "eof"
	case NOP:
		return "nop"
	case PUSH:
		return "push"
	case DUP:
		return "dup"
	case SWAP:
		return "swap"
	case DISCARD:
		return "discard"
	case ADD:
		return "add"
	case SUB:
		return "sub"
	case MUL:
		return "mul"
	case DIV:
		return "div"
	case MOD:
		return "mod"
	case STORE:
		return "store"
	case RETRIEVE:
		return "retrieve"
	case LABEL:
		return "label"
	case CALL:
		return "call"
	case JMP:
		return "jmp"
	case JZ:
		return "jz"
	case JN:
		return "jn"
	case RET:
		return "ret"
	case HLT:
		return "hlt"
	case OUTC:
		return "outc"
	case OUTN:
		return "outn"
	case INC:
		return "inc"
	case INN:
		return "inn"
	}
	return fmt.Sprintf("token(%d)", int(t))
}

type Lexer struct {
	r   io.Reader
	b   *bufio.Reader
	pos scanner.Position
}

const eof = -1

func (l *Lexer) Init(r io.Reader, name string) {
	l.r = r
	l.b = bufio.NewReader(r)
	l.pos = scanner.Position{Filename: name}
}

func (l *Lexer) Lex() (pos scanner.Position, tok Token, val string) {
	pos = l.pos
	tok = NOP
	switch l.next() {
	case ' ':
		switch ch := l.next(); ch {
		case ' ', '\t':
			tok = PUSH
			val = l.number(ch)
		case '\n':
			switch l.next() {
			case ' ':
				tok = DUP
			case '\t':
				tok = SWAP
			case '\n':
				tok = DISCARD
			}
		}

	case '\t':
		switch l.next() {
		case ' ':
			switch l.next() {
			case ' ':
				switch l.next() {
				case ' ':
					tok = ADD
				case '\t':
					tok = SUB
				case '\n':
					tok = MUL
				}
			case '\t':
				switch l.next() {
				case ' ':
					tok = DIV
				case '\t':
					tok = MOD
				}
			}

		case '\t':
			switch l.next() {
			case ' ':
				tok = STORE
			case '\t':
				tok = RETRIEVE
			}

		case '\n':
			switch l.next() {
			case ' ':
				switch l.next() {
				case ' ':
					tok = OUTC
				case '\t':
					tok = OUTN
				}
			case '\t':
				switch l.next() {
				case ' ':
					tok = INC
				case '\t':
					tok = INN
				}
			}
		}

	case '\n':
		switch l.next() {
		case ' ':
			switch l.next() {
			case ' ':
				tok = LABEL
				val = l.label()
			case '\t':
				tok = CALL
				val = l.label()
			case '\n':
				tok = JMP
				val = l.label()
			}

		case '\t':
			switch l.next() {
			case ' ':
				tok = JZ
				val = l.label()
			case '\t':
				tok = JN
				val = l.label()
			case '\n':
				tok = RET
			}

		case '\n':
			switch l.next() {
			case '\n':
				tok = HLT
			}
		}

	case eof:
		tok = EOF
	}
	return
}

func (l *Lexer) acceptws() string {
	var s string
loop:
	for {
		switch r := l.peek(); r {
		case ' ':
			s += "0"
		case '\t':
			s += "1"
		case '\n':
			l.next()
			fallthrough
		default:
			break loop
		}
		l.next()
	}
	return s
}

func (l *Lexer) number(sgn rune) string {
	s := l.acceptws()
	if sgn == '\t' {
		s = "-" + s
	}
	return s
}

func (l *Lexer) label() string {
	s := l.acceptws()
	if s == "" {
		return ">"
	}
	return s
}

func (l *Lexer) peek() rune {
	r := l.next()
	l.backup()
	return r
}

func (l *Lexer) backup() {
	l.b.UnreadByte()
}

func (l *Lexer) next() rune {
	var r rune
	for {
		c, err := l.b.ReadByte()
		if err != nil {
			return eof
		}
		r = rune(c)

		l.pos.Offset++
		if c == '\n' {
			l.pos.Line++
		}
		if isspace(r) {
			break
		}
	}
	return r
}

func isspace(r rune) bool {
	switch r {
	case ' ', '\t', '\n':
		return true
	}
	return false
}

type Inst struct {
	Op     Token
	Number int
	Label  string
}

type VM struct {
	Inst  []Inst
	Call  []int
	Stack []int
	Heap  map[int]int
	Label map[string]int
	PC    int
	Halt  bool

	In  io.Reader
	Out io.Writer

	Trace bool
}

func NewVM() *VM {
	return &VM{
		In:  os.Stdin,
		Out: os.Stdout,
	}
}

func (vm *VM) Disasm(ip Inst, trace bool) string {
	dis := ip.Op.String()
	switch ip.Op {
	case LABEL:
		dis = fmt.Sprintf("%s:", ip.Label)
	case PUSH:
		dis += fmt.Sprintf(" %d (%q)", ip.Number, string(ip.Number))
	case ADD, SUB, MUL, DIV, MOD:
		if trace {
			dis += fmt.Sprintf(" %d %d", vm.peek(-2), vm.peek(-1))
		}
	case JMP, JZ, JN, CALL:
		dis += fmt.Sprintf(" %s", ip.Label)
	case OUTC:
		if trace {
			dis += fmt.Sprintf(" %q", vm.top())
		}
	case OUTN:
		if trace {
			dis += fmt.Sprintf(" %d", vm.top())
		}
	case STORE:
		if trace {
			dis += fmt.Sprintf(" %d %d", vm.peek(-2), vm.peek(-1))
		}
	case RETRIEVE:
		if trace {
			dis += fmt.Sprintf(" %d", vm.peek(-1))
		}
	}
	return dis
}

func (vm *VM) LoadProg(name string) error {
	f, err := os.Open(name)
	if err != nil {
		return err
	}
	defer f.Close()

	var lx Lexer
	lx.Init(f, name)
	for {
		_, tok, val := lx.Lex()
		if tok == EOF {
			break
		}

		if tok == PUSH {
			n, _ := strconv.ParseInt(val, 2, 64)
			vm.Inst = append(vm.Inst, Inst{Op: tok, Number: int(n)})
		} else {
			vm.Inst = append(vm.Inst, Inst{Op: tok, Label: val})
		}
	}

	ln := 0
	lb := make(map[string]string)
	for i := range vm.Inst {
		ip := &vm.Inst[i]
		if ip.Op == LABEL {
			lb[ip.Label] = fmt.Sprintf("label_%d", ln)
			ln++
		}
	}
	for i := range vm.Inst {
		ip := &vm.Inst[i]
		if ip.Label == "" {
			continue
		}
		lp, found := lb[ip.Label]
		if !found {
			return fmt.Errorf("label %q does not exist", ip.Label)
		}
		ip.Label = lp
	}

	return nil
}

func (vm *VM) Reset() {
	vm.Call = vm.Call[:0]
	vm.Stack = vm.Stack[:0]
	vm.Heap = make(map[int]int)
	vm.Label = make(map[string]int)
	vm.PC = 0
	vm.Halt = false
	for i, ip := range vm.Inst {
		if ip.Op == LABEL {
			vm.Label[ip.Label] = i
		}
	}
}

func (vm *VM) Run() {
	vm.Reset()

	for !vm.Halt {
		vm.Step()
	}
}

func (vm *VM) Step() {
	ip := vm.fetch()
	if vm.Trace {
		fmt.Printf("%s\n", vm.Disasm(ip, vm.Trace))
		fmt.Printf("Stack: ")
		for _, v := range vm.Stack {
			fmt.Printf("%d ", v)
		}
		fmt.Println()
		fmt.Printf("Heap: ")
		fmt.Printf("%v", vm.Heap)
		fmt.Println()
		fmt.Println()
	}

	switch ip.Op {
	case HLT:
		fallthrough
	default:
		vm.Halt = true

	case NOP:

	case PUSH:
		vm.push(ip.Number)
	case DUP:
		vm.push(vm.top())
	case SWAP:
		x := vm.pop()
		y := vm.pop()
		vm.push(x)
		vm.push(y)
	case DISCARD:
		vm.pop()

	case ADD, SUB, MUL, DIV, MOD:
		vm.binop(ip.Op)

	case STORE:
		v := vm.pop()
		a := vm.pop()
		vm.writemem(a, v)
	case RETRIEVE:
		vm.push(vm.readmem(vm.pop()))

	case LABEL:
	case CALL:
		vm.Call = append(vm.Call, vm.PC)
		vm.PC = vm.Label[ip.Label]
	case RET:
		if len(vm.Call) == 0 {
			vm.Halt = true
		} else {
			n := len(vm.Call)
			vm.PC, vm.Call = vm.Call[n-1], vm.Call[:n-1]
		}

	case JMP:
		vm.branch(true, ip.Label)
	case JZ:
		vm.branch(vm.pop() == 0, ip.Label)
	case JN:
		vm.branch(vm.pop() < 0, ip.Label)

	case INC:
		var c int
		_, err := fmt.Fscanf(vm.In, "%c", &c)
		if err == io.EOF {
			vm.Halt = true
		}
		vm.writemem(vm.pop(), c)
	case INN:
		var n int
		_, err := fmt.Fscanf(vm.In, "%d", &n)
		if err == io.EOF {
			vm.Halt = true
		}
		vm.writemem(vm.pop(), n)
	case OUTC:
		format := "%c"
		if vm.Trace {
			format = "\n%c\n"
		}
		fmt.Fprintf(vm.Out, format, vm.pop())
	case OUTN:
		format := "%d"
		if vm.Trace {
			format = "\n%d\n"
		}
		fmt.Fprintf(vm.Out, format, vm.pop())
	}
}

func (vm *VM) fetch() Inst {
	if len(vm.Inst) <= vm.PC {
		return Inst{
			Op: HLT,
		}
	}
	ip := vm.Inst[vm.PC]
	vm.PC = (vm.PC + 1) % len(vm.Inst)
	return ip
}

func (vm *VM) readmem(a int) int {
	return vm.Heap[a]
}

func (vm *VM) writemem(a, v int) {
	vm.Heap[a] = v
}

func (vm *VM) push(v int) {
	vm.Stack = append(vm.Stack, v)
}

func (vm *VM) pop() int {
	v := 0
	l := len(vm.Stack)
	if l > 0 {
		v, vm.Stack = vm.Stack[l-1], vm.Stack[:l-1]
	}
	return v
}

func (vm *VM) peek(n int) int {
	l := len(vm.Stack) + n
	if 0 <= l && l < len(vm.Stack) {
		return vm.Stack[l]
	}
	return 0
}

func (vm *VM) top() int {
	l := len(vm.Stack)
	if l == 0 {
		return 0
	}
	return vm.Stack[l-1]
}

func (vm *VM) binop(op Token) {
	y := vm.pop()
	x := vm.pop()
	r := 0
	switch op {
	case ADD:
		r = x + y
	case SUB:
		r = x - y
	case MUL:
		r = x * y
	case DIV:
		if y != 0 {
			r = x / y
		}
	case MOD:
		if y != 0 {
			r = x % y
		}
	}
	vm.push(r)
}

func (vm *VM) branch(cond bool, label string) {
	if cond {
		vm.PC = vm.Label[label]
	}
}
