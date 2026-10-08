/*

https://luthert.web.illinois.edu/blog/posts/434.html
https://eli.thegreenplace.net/2009/11/23/visualizing-binary-trees-with-graphviz

A one-to-one mapping between binary trees and natural numbers.
A binary tree is either empty, or it has two binary trees as children. Following is a technique for mapping non-negative integers to binary trees in an efficient manner.

To de-interleave a number I write it in binary and create two numbers from it, one using the odd bits and the other the even bits.
For example, to de-interleave 71 I’d write it in binary as 1000111 then I’d take the odd bits 1000111 to make 1011, which is 11, and the even bits 1000111 to make 001, which is 1; thus 71 becomes (11, 1).

The tree for a number n is empty is n is zero; otherwise it has as its two children the trees for the de-interleaving of n − 1

*/

package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"log"
	"math"
	"os"
	"os/exec"
	"strconv"
)

var (
	outdir = flag.String("o", ".", "output directory")
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	n, _ := strconv.Atoi(flag.Arg(0))
	m := n
	if flag.NArg() > 1 {
		m, _ = strconv.Atoi(flag.Arg(1))
	}
	l := int(math.Log10(float64(m))) + 1

	os.MkdirAll(*outdir, 0755)
	for i := n; i <= m; i++ {
		dotname := fmt.Sprintf("%v/%0*d.dot", *outdir, l, i)
		imgname := fmt.Sprintf("%v/%0*d.png", *outdir, l, i)

		fmt.Println("Writing", dotname)

		f, err := os.Create(dotname)
		if err != nil {
			log.Fatal(err)
		}

		w := bufio.NewWriter(f)
		printdot(w, i, gen(i))
		w.Flush()
		f.Close()

		c := exec.Command("dot", dotname, "-Tpng", "-o", imgname)
		c.Run()
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] start end")
	flag.PrintDefaults()
	os.Exit(2)
}

type Node struct {
	val   int
	left  *Node
	right *Node
}

func gen(n int) *Node {
	if n < 0 {
		return nil
	}

	v := 0
	s := treeof(n)
	p := []*Node{}
	for i := len(s) - 1; i >= 0; i-- {
		if s[i] == 'L' {
			p = append(p, &Node{val: v})
		} else {
			n := len(p)
			p[n-2] = &Node{
				val:   v,
				left:  p[n-1],
				right: p[n-2],
			}
			p = p[:n-1]
		}
		v++
	}

	if len(p) == 0 {
		return nil
	}

	return p[0]
}

func treeof(n int) string {
	if n <= 0 {
		return "L"
	}

	a, b := deinterleave(n - 1)
	return "R" + treeof(a) + treeof(b)
}

func deinterleave(n int) (int, int) {
	a, b, s := 0, 0, 1
	for n > 0 {
		if n&1 == 1 {
			a |= s
		}
		if n&2 == 2 {
			b |= s
		}
		n >>= 2
		s <<= 1
	}
	return a, b
}

func printdot(w io.Writer, i int, t *Node) {
	nl := 0
	fmt.Fprintf(w, "digraph Tree {\n")
	fmt.Fprintf(w, "\tcomment=\"%d %s\";\n", i, treeof(i))
	switch {
	case t == nil:
		fmt.Fprintf(w, "\n")
	case t.left == nil && t.right != nil:
		fmt.Fprintf(w, "\t%d;\n", t.val)
	default:
		printdotaux(w, t, &nl)
	}
	fmt.Fprintf(w, "}\n")
}

func printdotaux(w io.Writer, t *Node, nl *int) {
	switch {
	case t.left != nil:
		fmt.Fprintf(w, "\t%d -> %d;\n", t.val, t.left.val)
		printdotaux(w, t.left, nl)
	default:
		printdotnull(w, t.val, *nl)
		*nl += 1
	}

	switch {
	case t.right != nil:
		fmt.Fprintf(w, "\t%d -> %d;\n", t.val, t.right.val)
		printdotaux(w, t.right, nl)
	default:
		printdotnull(w, t.val, *nl)
		*nl += 1
	}
}

func printdotnull(w io.Writer, v int, nl int) {
	fmt.Fprintf(w, "\tnull%d [shape=point];\n", nl)
	fmt.Fprintf(w, "\t%d -> null%d;\n", v, nl)
}
