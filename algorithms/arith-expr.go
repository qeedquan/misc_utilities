// evaluating to an AST tree by walking in a prefix/infix/postfix manner
// gives the prefix/infix/postfix notation respectively
package main

import (
	"flag"
	"fmt"
	"go/ast"
	"go/parser"
	"log"
	"os"
	"strings"
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("eval: ")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		test()
		return
	}

	expr := strings.Join(flag.Args(), "")
	walk(expr)
}

func test() {
	exprs := []string{
		"(5-6)*7",
		"5-(6*7)",
		"10 / 5",
		"3 - 4 + 5",
		"3 - 4*5",
		"(3-20)%42",
		"tau/xdwd+4563*34",
		"(14-25-(3*4)/(435))",
		" A * ( B + C ) / D",
		"A * (B + C) / D",
		"A * (B + C / D)",
		"(A - B/C) * (A/K-L)",
		"sin(x+4*2, 453-338)*cos(x^2)",
		"tan(sin(x/2+3))",
		"f(a[4535], 56/(a[2^2*2]-45))",
		" ((15 / (7 - (1 + 1))) * 3) - (2 + (1 + 1))",
	}
	for _, expr := range exprs {
		walk(expr)
	}
}

func walk(expr string) {
	tree, err := parser.ParseExpr(expr)
	if err != nil {
		fmt.Println(err)
		return
	}

	fmt.Printf("%s\n", expr)

	fmt.Printf("prefix: ")
	prefix(tree)
	fmt.Println()

	fmt.Printf("infix: ")
	infix(tree)
	fmt.Println()

	fmt.Printf("postfix: ")
	postfix(tree)
	fmt.Println()

	fmt.Println()
}

func prefix(e ast.Expr) {
	switch e := e.(type) {
	case *ast.IndexExpr:
		prefix(e.X)
		fmt.Printf("[ ")
		prefix(e.Index)
		fmt.Printf(" ] ")
	case *ast.CallExpr:
		fmt.Printf(" ")
		prefix(e.Fun)
		fmt.Printf("( ")
		for i, a := range e.Args {
			prefix(a)
			if i+1 < len(e.Args) {
				fmt.Printf(", ")
			}
		}
		fmt.Printf(" )")
	case *ast.Ident:
		fmt.Printf("%v ", e.Name)
	case *ast.BasicLit:
		fmt.Printf("%v ", e.Value)
	case *ast.BinaryExpr:
		fmt.Printf("%v ", e.Op)
		prefix(e.X)
		prefix(e.Y)
	case *ast.UnaryExpr:
		prefix(e.X)
	case *ast.ParenExpr:
		prefix(e.X)
	default:
		log.Fatalf("unsupported expression %T", e)
	}
}

func infix(e ast.Expr) {
	switch e := e.(type) {
	case *ast.IndexExpr:
		infix(e.X)
		fmt.Printf("[")
		infix(e.Index)
		fmt.Printf("]")
	case *ast.CallExpr:
		infix(e.Fun)
		fmt.Printf("(")
		for i, a := range e.Args {
			infix(a)
			if i+1 < len(e.Args) {
				fmt.Printf(",")
			}
		}
		fmt.Printf(")")
	case *ast.Ident:
		fmt.Printf("%v", e.Name)
	case *ast.BasicLit:
		fmt.Printf("%v", e.Value)
	case *ast.BinaryExpr:
		infix(e.X)
		fmt.Printf("%v", e.Op)
		infix(e.Y)
	case *ast.UnaryExpr:
		fmt.Printf("%v", e.Op)
		infix(e.X)
	case *ast.ParenExpr:
		fmt.Printf("(")
		infix(e.X)
		fmt.Printf(")")
	}
}

func postfix(e ast.Expr) {
	switch e := e.(type) {
	case *ast.IndexExpr:
		postfix(e.X)
		fmt.Printf("[ ")
		postfix(e.Index)
		fmt.Printf(" ] ")
	case *ast.CallExpr:
		fmt.Printf(" ")
		postfix(e.Fun)
		fmt.Printf("( ")
		for i, a := range e.Args {
			postfix(a)
			if i+1 < len(e.Args) {
				fmt.Printf(", ")
			}
		}
		fmt.Printf(") ")
	case *ast.Ident:
		fmt.Printf("%v ", e.Name)
	case *ast.BasicLit:
		fmt.Printf("%v ", e.Value)
	case *ast.BinaryExpr:
		postfix(e.X)
		postfix(e.Y)
		fmt.Printf("%v ", e.Op)
	case *ast.UnaryExpr:
		postfix(e.X)
	case *ast.ParenExpr:
		postfix(e.X)
	default:
		log.Fatalf("unsupported expression %T", e)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: <expr>")
	flag.PrintDefaults()
	os.Exit(2)
}
