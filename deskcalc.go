package main

import (
	"flag"
	"fmt"
	"go/ast"
	"go/parser"
	"go/token"
	"log"
	"math/big"
	"os"
	"strings"
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("deskcalc: ")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	str := strings.Join(flag.Args(), " ")
	expr, err := parser.ParseExpr(str)
	ck(err)

	v := eval(expr)
	fmt.Println(v.RatString())
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: deskcalc [options] expr")
	flag.PrintDefaults()
	os.Exit(2)
}

func eval(e ast.Expr) *big.Rat {
	switch e := e.(type) {
	case *ast.BinaryExpr:
		x := eval(e.X)
		y := eval(e.Y)
		switch e.Op {
		case token.ADD:
			return x.Add(x, y)
		case token.SUB:
			return x.Sub(x, y)
		case token.MUL:
			return x.Mul(x, y)
		case token.QUO:
			return x.Quo(x, y)
		default:
			log.Fatal("invalid expression")
		}
	case *ast.UnaryExpr:
		x := eval(e.X)
		switch e.Op {
		case token.ADD:
			return x
		case token.SUB:
			return x.Neg(x)
		}
	case *ast.BasicLit:
		return newrat(e.Value)
	default:
		log.Fatal("invalid expression!")
	}
	return nil
}

func newrat(s string) *big.Rat {
	r := new(big.Rat)
	_, ok := r.SetString(s)
	if !ok {
		log.Fatal("invalid expression")
	}
	return r
}
