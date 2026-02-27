// algorithm described here, parses math expressions
// https://www.engr.mun.ca/~theo/Misc/exp_parsing.htm
package main

import (
	"fmt"
	"go/ast"
	"go/token"
	"log"
	"math/big"
	"os"
	"strings"
	"text/scanner"
)

func main() {
	log.SetFlags(0)

	if len(os.Args) < 2 {
		log.Fatal("usage: <expr>")
	}

	expr := strings.Join(os.Args[1:], " ")
	defer func() {
		if e := recover(); e != nil {
			log.Fatalf("%v: %q", e, expr)
		}
	}()

	var scan scanner.Scanner
	scan.Init(strings.NewReader(expr))

	evaluator := &Evaluator{Scanner: &scan}
	tree := evaluator.Parse()
	fmt.Println(walk(tree))
}

func walk(n ast.Expr) string {
	switch n := n.(type) {
	case *ast.BinaryExpr:
		x := rat(walk(n.X))
		y := rat(walk(n.Y))
		switch n.Op {
		case token.ADD:
			x.Add(x, y)
		case token.SUB:
			x.Sub(x, y)
		case token.MUL:
			x.Mul(x, y)
		case token.QUO:
			x.Quo(x, y)
		default:
			panic("invalid binary operation")
		}
		return num(x)

	case *ast.UnaryExpr:
		x := rat(walk(n.X))
		switch n.Op {
		case token.SUB:
			x.Neg(x)
		case token.ADD:
		default:
			panic("invalid unary operation")
		}
		return num(x)

	case *ast.BasicLit:
		return num(rat(n.Value))
	}

	panic("invalid expression")
}

func rat(s string) *big.Rat {
	x, _ := new(big.Rat).SetString(s)
	return x
}

func num(x *big.Rat) string {
	if x.IsInt() {
		return x.Num().String()
	}
	return x.String()
}

type Evaluator struct {
	*scanner.Scanner
	tok rune
	lit string
}

var unaryOps = map[rune]token.Token{
	'+': token.ADD,
	'-': token.SUB,
}

var binaryOps = map[rune]token.Token{
	'+': token.ADD,
	'-': token.SUB,
	'*': token.MUL,
	'/': token.QUO,
}

func (z *Evaluator) next() {
	z.tok = z.Scan()
	z.lit = z.TokenText()
}

func (z *Evaluator) Parse() ast.Expr {
	z.next()
	return z.binary(0)
}

func (z *Evaluator) binary(p int) ast.Expr {
	t := z.unaryOrTerm()
	for {
		op, binary := binaryOps[z.tok]
		if !(binary && op.Precedence() >= p) {
			break
		}
		z.next()
		q := 1 + op.Precedence()
		t1 := z.binary(q)
		t = &ast.BinaryExpr{
			Op: op,
			X:  t,
			Y:  t1,
		}
	}
	return t
}

func (z *Evaluator) unaryOrTerm() ast.Expr {
	if op, exist := unaryOps[z.tok]; exist {
		z.next()
		q := op.Precedence()
		t := z.binary(q)
		return &ast.UnaryExpr{
			Op: op,
			X:  t,
		}
	} else if z.tok == '(' {
		z.next()
		t := z.binary(0)
		if z.tok != ')' {
			panic("missing paren")
		}
		z.next()
		return t
	} else if z.tok == scanner.Int || z.tok == scanner.Float {
		t := &ast.BasicLit{Value: z.lit}
		z.next()
		return t
	}

	panic("unknown expression")
}
