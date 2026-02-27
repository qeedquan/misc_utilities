// ported from llvm kaleidoscope
package main

import (
	"bufio"
	"fmt"
	"io"
	"log"
	"os"
	"strconv"
	"unicode"
	"unicode/utf8"

	"llvm.org/llvm/bindings/go/llvm"
)

func main() {
	log.SetFlags(0)
	llvm.LinkInInterpreter()
	llvm.InitializeAllTargets()
	llvm.InitializeAllAsmPrinters()
	llvm.InitializeAllAsmParsers()

	parser := NewParser(os.Stdin)
	parser.Main()
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func errv(msg string) llvm.Value {
	fmt.Fprintln(os.Stderr, msg)
	return llvm.Value{}
}

func errp(msg string) *ProtoDecl {
	fmt.Fprintln(os.Stderr, msg)
	return nil
}

func errx(msg string) Expr {
	fmt.Fprintln(os.Stderr, msg)
	return nil
}

const (
	EOF = -1 - iota
	DEF
	EXTERN
	IDENT
	NUMBER
	IF
	THEN
	ELSE
	FOR
	IN
	BINARY
	UNARY
	VAR
)

type Lexer struct {
	Ident  string
	Number float64
	r      *bufio.Reader
	ch     rune
}

func isAlnum(r rune) bool { return unicode.IsLetter(r) || unicode.IsDigit(r) }
func isAscii(r rune) bool { return r&^0x7f == 0 }

func NewLexer(r io.Reader) *Lexer {
	return &Lexer{
		r:  bufio.NewReader(r),
		ch: ' ',
	}
}

func (l *Lexer) getch() rune {
	ch, _, err := l.r.ReadRune()
	if err != nil {
		return EOF
	}
	return ch
}

func (l *Lexer) Next() rune {
	for unicode.IsSpace(l.ch) {
		l.ch = l.getch()
	}

	if unicode.IsLetter(l.ch) {
		l.Ident = string(l.ch)
		for isAlnum(l.ch) {
			l.ch = l.getch()
		}

		switch l.Ident {
		case "def":
			return DEF
		case "extern":
			return EXTERN
		case "if":
			return IF
		case "then":
			return THEN
		case "else":
			return ELSE
		case "for":
			return FOR
		case "in":
			return IN
		case "binary":
			return BINARY
		case "unary":
			return UNARY
		case "var":
			return VAR
		default:
			return IDENT
		}
	}

	if unicode.IsDigit(l.ch) || l.ch == '.' {
		var num string
		for {
			num += string(l.ch)
			l.ch = l.getch()
			if !(unicode.IsDigit(l.ch) || l.ch == '.') {
				break
			}
		}

		l.Number, _ = strconv.ParseFloat(num, 64)
		return NUMBER
	}

	if l.ch == '#' {
		for {
			l.ch = l.getch()
			if !(l.ch != EOF && l.ch != '\n' && l.ch != '\r') {
				break
			}
		}
		if l.ch != EOF {
			return l.Next()
		}
	}

	if l.ch == EOF {
		return EOF
	}

	ch := l.ch
	l.ch = l.getch()
	return ch
}

type Parser struct {
	lex  *Lexer
	tok  rune
	cg   *CG
	Prec map[rune]int
}

func NewParser(r io.Reader) *Parser {
	return &Parser{
		lex: NewLexer(r),
		cg:  NewCG(),
		Prec: map[rune]int{
			'=': 2,
			'<': 10,
			'+': 20,
			'-': 20,
			'*': 40,
		},
	}
}

func (p *Parser) Main() {
	fmt.Fprint(os.Stderr, "ready> ")
	p.next()

	for {
		switch p.tok {
		case EOF:
			fmt.Fprintln(os.Stderr)
			return
		case ';':
			p.next()
		case DEF:
			p.handleDef()
		case EXTERN:
			p.handleExtern()
		default:
			p.handleTop()
		}
		fmt.Fprint(os.Stderr, "ready> ")
	}
}

func (p *Parser) next() rune {
	p.tok = p.lex.Next()
	return p.tok
}

func (p *Parser) precedence() int {
	if prec, found := p.Prec[p.tok]; found {
		return prec
	}
	return -1
}

func (p *Parser) handleDef() {
	if fn := p.parseDef(); fn != nil {
		if ir := fn.Gen(p, p.cg); !ir.IsNil() {
			fmt.Fprint(os.Stderr, "Read function definition:")
			ir.Dump()
			p.cg.Init()
		}
	} else {
		p.next()
	}
}

func (p *Parser) handleExtern() {
	if proto := p.parseExtern(); proto != nil {
		if ir := proto.Gen(p.cg); !ir.IsNil() {
			fmt.Fprint(os.Stderr, "Read extern: ")
			ir.Dump()
			p.cg.Protos[proto.Name] = proto
		}
	} else {
		p.next()
	}
}

func (p *Parser) handleTop() {
	if fn := p.parseTop(); fn != nil {
		if ir := fn.Gen(p, p.cg); !ir.IsNil() {
			ir.Dump()
			ret := p.cg.EE.RunFunction(ir, []llvm.GenericValue{})
			fmt.Printf("Evaluated to: %v\n", ret.Float(llvm.DoubleType()))
			p.cg.Init()
		}
	} else {
		p.next()
	}
}

func (p *Parser) parseDef() *FuncDecl {
	p.next()
	proto := p.parseProto()
	if proto == nil {
		return nil
	}

	expr := p.parseExpr()
	if expr == nil {
		return nil
	}

	return &FuncDecl{proto, expr}
}

func (p *Parser) parseTop() *FuncDecl {
	if expr := p.parseExpr(); expr != nil {
		proto := &ProtoDecl{Name: "__anon_expr"}
		return &FuncDecl{proto, expr}
	}
	return nil
}

func (p *Parser) parseExtern() *ProtoDecl {
	p.next()
	return p.parseProto()
}

func (p *Parser) parseProto() *ProtoDecl {
	name := ""
	kind := 0
	prec := 30
	switch p.tok {
	default:
		return errp("Expected function name in prototype")
	case IDENT:
		name = p.lex.Ident
		kind = 0
		p.next()
	case UNARY:
		p.next()
		if !isAscii(p.tok) {
			return errp("Expected unary operator")
		}
		name = "unary" + string(p.tok)
		kind = 1
		p.next()
	case BINARY:
		p.next()
		if !isAscii(p.tok) {
			return errp("Expected binary operator")
		}
		name = "binary" + string(p.tok)
		kind = 2
		p.next()

		if p.tok == NUMBER {
			if p.lex.Number < 1 || p.lex.Number > 100 {
				return errp("Invalid precedence: must be 1..100")
			}
			prec = int(p.lex.Number)
			p.next()
		}
	}

	if p.tok != '(' {
		return errp("Expected '(' in prototype")
	}

	var args []string
	for p.next() != IDENT {
		args = append(args, p.lex.Ident)
	}
	if p.tok != ')' {
		return errp("Expected ')' in prototype")
	}

	p.next()

	if kind != 0 && len(args) != kind {
		return errp("Invalid number of operands for operator")
	}

	return &ProtoDecl{name, args, kind != 0, prec}
}

func (p *Parser) parseExpr() Expr {
	lhs := p.parseUnary()
	if lhs == nil {
		return nil
	}
	return p.parseBinOpRHS(0, lhs)
}

func (p *Parser) parseUnary() Expr {
	if !isAscii(p.tok) || p.tok == '(' || p.tok == ',' {
		return p.parsePrimary()
	}

	op := p.tok
	p.next()

	operand := p.parseUnary()
	if operand == nil {
		return nil
	}

	return &UnaryExpr{op, operand}
}

func (p *Parser) parseBinOpRHS(prec int, lhs Expr) Expr {
	for {
		tokPrec := p.precedence()
		if tokPrec < prec {
			return lhs
		}

		op := p.tok
		p.next()

		rhs := p.parseUnary()
		if rhs == nil {
			return nil
		}

		nextPrec := p.precedence()
		if tokPrec < nextPrec {
			rhs = p.parseBinOpRHS(tokPrec+1, rhs)
			if rhs == nil {
				return nil
			}
		}

		lhs = &BinaryExpr{op, lhs, rhs}
	}
}

func (p *Parser) parsePrimary() Expr {
	switch p.tok {
	default:
		return errx("unknown token when expecting an expression")
	case IDENT:
		return p.parseIdent()
	case NUMBER:
		return p.parseNumber()
	case '(':
		return p.parseParen()
	case IF:
		return p.parseIf()
	case FOR:
		return p.parseFor()
	case VAR:
		return p.parseVar()
	}
}

func (p *Parser) parseIdent() Expr {
	id := p.lex.Ident

	p.next()
	if p.tok != '(' {
		return &VariableExpr{id}
	}

	p.next()
	var args []Expr
	if p.tok != ')' {
		for {
			if arg := p.parseExpr(); arg != nil {
				args = append(args, arg)
			} else {
				return nil
			}

			if p.tok == ')' {
				break
			}

			if p.tok != ',' {
				return errx("Expected ')' or ',' in argument list")
			}
			p.next()
		}
	}
	p.next()

	return &CallExpr{id, args}
}

func (p *Parser) parseNumber() Expr {
	expr := &NumberExpr{p.lex.Number}
	p.next()
	return expr
}

func (p *Parser) parseParen() Expr {
	p.next()
	expr := p.parseExpr()
	if expr == nil {
		return nil
	}

	if p.tok != ')' {
		return errp("expected ')'")
	}
	p.next()

	return nil
}

func (p *Parser) parseIf() Expr {
	p.next()

	cond := p.parseExpr()
	if cond != nil {
		return nil
	}

	if p.tok != THEN {
		return errx("expected then")
	}
	p.next()

	then := p.parseExpr()
	if then == nil {
		return nil
	}

	if p.tok != ELSE {
		return errx("expected else")
	}

	p.next()

	else_ := p.parseExpr()
	if else_ == nil {
		return nil
	}

	return &IfExpr{cond, then, else_}
}

func (p *Parser) parseFor() Expr {
	p.next()

	if p.tok != IDENT {
		return errx("Expected identifier after for")
	}

	id := p.lex.Ident
	p.next()

	if p.tok != '=' {
		return errx("Expected '=' after for")
	}
	p.next()

	start := p.parseExpr()
	if start == nil {
		return nil
	}
	if p.tok != ',' {
		return errx("expected ',' after for start value")
	}
	p.next()

	end := p.parseExpr()
	if end == nil {
		return nil
	}

	var step Expr
	if p.tok == ',' {
		p.next()
		step = p.parseExpr()
		if step == nil {
			return nil
		}
	}

	if p.tok != IN {
		return errx("expected 'in' after for")
	}
	p.next()

	body := p.parseExpr()
	if body == nil {
		return nil
	}

	return &ForExpr{id, start, end, step, body}
}

func (p *Parser) parseVar() Expr {
	p.next()

	names := make(map[string]Expr)
	if p.tok != IDENT {
		return errx("Expected identifier after var")
	}

	for {
		name := p.lex.Ident
		p.next()

		var init Expr
		if p.tok == '=' {
			p.next()

			init = p.parseExpr()
			if init == nil {
				return nil
			}
		}

		names[name] = init

		if p.tok != ',' {
			break
		}
		p.next()

		if p.tok != IDENT {
			return errx("expected identifier list after var")
		}
	}

	if p.tok != IN {
		return errx("expected 'in' keyword after 'var'")
	}
	p.next()

	body := p.parseExpr()
	if body == nil {
		return nil
	}

	return &VarExpr{names, body}
}

type Expr interface {
	Gen(*CG) llvm.Value
}

type NumberExpr struct {
	Value float64
}

type VariableExpr struct {
	Name string
}

type UnaryExpr struct {
	Op      rune
	Operand Expr
}

type BinaryExpr struct {
	Op       rune
	Lhs, Rhs Expr
}

type CallExpr struct {
	Callee string
	Args   []Expr
}

type IfExpr struct {
	Cond, Then, Else Expr
}

type ForExpr struct {
	Var                    string
	Start, End, Step, Body Expr
}

type VarExpr struct {
	Names map[string]Expr
	Body  Expr
}

type ProtoDecl struct {
	Name       string
	Args       []string
	IsOperator bool
	Precedence int
}

type FuncDecl struct {
	Proto *ProtoDecl
	Body  Expr
}

func (n *NumberExpr) Gen(cg *CG) llvm.Value {
	return llvm.ConstFloat(cg.DoubleType(), n.Value)
}

func (n *VariableExpr) Gen(cg *CG) llvm.Value {
	v := cg.NamedValues[n.Name]
	if v.IsNil() {
		return errv("Unknown variable name")
	}
	return cg.CreateLoad(v, n.Name)
}

func (n *IfExpr) Gen(cg *CG) llvm.Value {
	condv := n.Cond.Gen(cg)
	if condv.IsNil() {
		return errv("Code generation failed for if expression")
	}

	fun := cg.GetInsertBlock().Parent()
	thenBB := cg.AddBasicBlock(fun, "then")
	elseBB := cg.AddBasicBlock(fun, "else")
	mergeBB := cg.AddBasicBlock(fun, "merge")
	cg.CreateCondBr(condv, thenBB, elseBB)

	cg.SetInsertPointAtEnd(thenBB)
	thenv := n.Then.Gen(cg)
	if thenv.IsNil() {
		return errv("Code generation failed for then expression")
	}
	cg.CreateBr(mergeBB)
	thenBB = cg.GetInsertBlock()

	cg.SetInsertPointAtEnd(elseBB)
	elsev := n.Else.Gen(cg)
	if elsev.IsNil() {
		return errv("Code generation failed for else expression")
	}

	cg.CreateBr(mergeBB)
	elseBB = cg.GetInsertBlock()

	cg.SetInsertPointAtEnd(mergeBB)
	pn := cg.CreatePHI(cg.DoubleType(), "iftmp")
	pn.AddIncoming([]llvm.Value{thenv}, []llvm.BasicBlock{thenBB})
	pn.AddIncoming([]llvm.Value{elsev}, []llvm.BasicBlock{elseBB})

	return pn
}

func (n *UnaryExpr) Gen(cg *CG) llvm.Value {
	v := n.Operand.Gen(cg)
	if v.IsNil() {
		return llvm.Value{}
	}

	f := cg.GetFunction("unary" + string(n.Op))
	return cg.CreateCall(f, []llvm.Value{v}, "unop")
}

func (n *BinaryExpr) Gen(cg *CG) llvm.Value {
	if n.Op == '=' {
		lhse, _ := n.Lhs.(*VariableExpr)
		if lhse == nil {
			return errv("Destination of '=' must be a variable")
		}

		val := n.Rhs.Gen(cg)
		if val.IsNil() {
			return llvm.Value{}
		}

		variable := cg.NamedValues[lhse.Name]
		if variable.IsNil() {
			return errv("Unknown variable name")
		}

		cg.CreateStore(val, variable)
		return val
	}

	l := n.Lhs.Gen(cg)
	r := n.Rhs.Gen(cg)
	if l.IsNil() || r.IsNil() {
		return llvm.Value{}
	}

	switch n.Op {
	case '+':
		return cg.CreateFAdd(l, r, "addtmp")
	case '-':
		return cg.CreateFSub(l, r, "subtmp")
	case '*':
		return cg.CreateFMul(l, r, "multmp")
	case '<':
		l = cg.CreateFCmp(llvm.FloatULT, l, r, "cmptmp")
		return cg.CreateUIToFP(l, cg.DoubleType(), "booltmp")
	}

	f := cg.GetFunction("binary" + string(n.Op))
	if f.IsNil() {
		panic("Function is nil")
	}

	return cg.CreateCall(f, []llvm.Value{l, r}, "binop")
}

func (n *VarExpr) Gen(cg *CG) llvm.Value {
	fun := cg.GetInsertBlock().Parent()

	var oldBindingNames []string
	var oldBindingValues []llvm.Value
	for name, init := range n.Names {
		var initVal llvm.Value
		if init != nil {
			init.Gen(cg)
			if initVal.IsNil() {
				return llvm.Value{}
			}
		} else {
			initVal = llvm.ConstFloat(cg.FloatType(), 0)
		}

		alloca := createEntryBlockAlloca(fun, name)
		cg.CreateStore(initVal, alloca)

		oldBindingNames = append(oldBindingNames, name)
		oldBindingValues = append(oldBindingValues, cg.NamedValues[name])
		cg.NamedValues[name] = alloca
	}

	bodyVal := n.Body.Gen(cg)
	if bodyVal.IsNil() {
		return llvm.Value{}
	}

	for i, name := range oldBindingNames {
		cg.NamedValues[name] = oldBindingValues[i]
	}

	return bodyVal
}

func (n *ForExpr) Gen(cg *CG) llvm.Value {
	startVal := n.Start.Gen(cg)
	if startVal.IsNil() {
		return errv("Code generation failed for start expression")
	}

	fun := cg.GetInsertBlock().Parent()
	alloca := createEntryBlockAlloca(fun, n.Var)
	cg.CreateStore(startVal, alloca)

	loopBB := llvm.AddBasicBlock(fun, "loop")
	cg.CreateBr(loopBB)
	cg.SetInsertPointAtEnd(loopBB)

	oldVal := cg.NamedValues[n.Var]
	cg.NamedValues[n.Var] = alloca

	if n.Body.Gen(cg).IsNil() {
		return llvm.Value{}
	}

	var stepVal llvm.Value
	if n.Step != nil {
		stepVal = n.Step.Gen(cg)
		if stepVal.IsNil() {
			return llvm.ConstNull(llvm.DoubleType())
		}
	} else {
		stepVal = llvm.ConstFloat(llvm.DoubleType(), 1)
	}

	endVal := n.End.Gen(cg)
	if endVal.IsNil() {
		return llvm.Value{}
	}

	curVar := cg.CreateLoad(alloca, n.Var)
	nextVar := cg.CreateFAdd(curVar, stepVal, "nextvar")
	cg.CreateStore(nextVar, alloca)

	endVal = cg.CreateFCmp(llvm.FloatONE, endVal, llvm.ConstFloat(llvm.DoubleType(), 0), "loopcond")
	afterBB := cg.AddBasicBlock(fun, "afterloop")

	cg.CreateCondBr(endVal, loopBB, afterBB)

	cg.SetInsertPointAtEnd(afterBB)

	if !oldVal.IsNil() {
		cg.NamedValues[n.Var] = oldVal
	} else {
		delete(cg.NamedValues, n.Var)
	}

	return llvm.ConstFloat(llvm.DoubleType(), 0)
}

func (n *CallExpr) Gen(cg *CG) llvm.Value {
	callee := cg.Mod.NamedFunction(n.Callee)
	if callee.IsNil() {
		return errv("Unknown function referenced: " + n.Callee)
	}

	if callee.ParamsCount() != len(n.Args) {
		return errv("Incorrect number of arguments passed")
	}

	args := []llvm.Value{}
	for _, arg := range n.Args {
		args = append(args, arg.Gen(cg))
		if args[len(args)-1].IsNil() {
			return errv("An argument was nil")
		}
	}
	return cg.CreateCall(callee, args, "calltmp")
}

func (n *ProtoDecl) Gen(cg *CG) llvm.Value {
	var args []llvm.Type
	for _ = range n.Args {
		args = append(args, llvm.DoubleType())
	}
	typ := llvm.FunctionType(llvm.DoubleType(), args, false)
	fun := llvm.AddFunction(cg.Mod, n.Name, typ)

	if fun.BasicBlocksCount() != 0 {
		return errv("redefinition of function: " + n.Name)
	}

	if fun.ParamsCount() != len(n.Args) {
		return errv("redefinition of function with different number of args")
	}

	for i, param := range fun.Params() {
		param.SetName(n.Args[i])
	}

	return fun
}

func (n *ProtoDecl) IsUnaryOp() bool {
	return n.IsOperator && len(n.Args) == 1
}

func (n *ProtoDecl) IsBinaryOp() bool {
	return n.IsOperator && len(n.Args) == 2
}

func (n *ProtoDecl) OperatorName() rune {
	if !(n.IsUnaryOp() || n.IsBinaryOp()) {
		panic("Not an operator")
	}
	r, _ := utf8.DecodeLastRuneInString(n.Name)
	return r
}

func (n *FuncDecl) Gen(p *Parser, cg *CG) llvm.Value {
	cg.Protos[n.Proto.Name] = n.Proto
	fun := cg.GetFunction(n.Proto.Name)
	if fun.IsNil() {
		return errv("Prototype is missing")
	}

	if n.Proto.IsBinaryOp() {
		p.Prec[n.Proto.OperatorName()] = p.precedence()
	}

	bb := llvm.AddBasicBlock(fun, "entry")
	cg.SetInsertPointAtEnd(bb)

	cg.NamedValues = make(map[string]llvm.Value)

	args := fun.Params()
	for i := range args {
		alloca := createEntryBlockAlloca(fun, n.Proto.Args[i])
		cg.CreateStore(args[i], alloca)
		cg.NamedValues[n.Proto.Args[i]] = alloca
	}

	retVal := n.Body.Gen(cg)
	if !retVal.IsNil() {
		cg.CreateRet(retVal)
		llvm.VerifyFunction(fun, llvm.PrintMessageAction)
		cg.Fpm.RunFunc(fun)
		return fun
	}

	fun.EraseFromParentAsFunction()
	if n.Proto.IsBinaryOp() {
		delete(p.Prec, n.Proto.OperatorName())
	}

	return llvm.Value{}
}

type CG struct {
	llvm.Context
	llvm.Builder
	EE          llvm.ExecutionEngine
	Mod         llvm.Module
	Fpm         llvm.PassManager
	NamedValues map[string]llvm.Value
	Protos      map[string]*ProtoDecl
}

func NewCG() *CG {
	cg := &CG{
		Context:     llvm.GlobalContext(),
		Builder:     llvm.NewBuilder(),
		Mod:         llvm.NewModule("kaleidoscope"),
		NamedValues: make(map[string]llvm.Value),
		Protos:      make(map[string]*ProtoDecl),
	}
	cg.Init()
	return cg
}

func (c *CG) Init() {
	var err error
	c.Mod = c.NewModule("kaleidoscope")
	c.EE, err = llvm.NewInterpreter(c.Mod)
	ck(err)
	c.Fpm = llvm.NewFunctionPassManagerForModule(c.Mod)

	c.Fpm.Add(c.EE.TargetData())
	c.Fpm.AddInstructionCombiningPass()
	c.Fpm.AddReassociatePass()
	c.Fpm.AddGVNPass()
	c.Fpm.AddCFGSimplificationPass()
	c.Fpm.InitializeFunc()
}

func (cg *CG) GetFunction(name string) llvm.Value {
	f := cg.Mod.NamedFunction(name)
	if !f.IsNil() {
		return f
	}

	fi, found := cg.Protos[name]
	if found {
		return fi.Gen(cg)
	}

	return llvm.Value{}
}

func createEntryBlockAlloca(f llvm.Value, name string) llvm.Value {
	b := llvm.NewBuilder()
	b.SetInsertPoint(f.EntryBasicBlock(), f.EntryBasicBlock().FirstInstruction())
	return b.CreateAlloca(llvm.DoubleType(), name)
}
