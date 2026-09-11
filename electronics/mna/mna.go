// https://lpsa.swarthmore.edu/Systems/Electrical/mna/MNA1.html
package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"log"
	"math"
	"os"
	"strconv"
	"strings"
	"text/tabwriter"
)

func main() {
	log.SetPrefix("mna: ")
	log.SetFlags(0)
	flag.Parse()
	flag.Usage = usage
	if flag.NArg() < 1 {
		usage()
	}

	for _, name := range flag.Args() {
		err := solve(name)
		if err != nil {
			fmt.Fprintf(os.Stderr, "%v: %v\n", name, err)
		}
	}
}

func solve(name string) error {
	defer func() {
		if e := recover(); e != nil {
			fmt.Fprintf(os.Stderr, "%v: failed to solve due to error: %v\n", name, e)
			return
		}
	}()
	ci := &Circuit{}
	err := ci.ReadNetFile(name)
	if err != nil {
		return err
	}

	ci.Dump(os.Stdout)
	return nil
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] netfile ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func max(a, b int) int {
	if a > b {
		return a
	}
	return b
}

type Matrix [][][]Element

type Circuit struct {
	Elems          []*Element
	Conns          map[int][]*Element
	NumNodes       int
	NumVoltSources int
}

type Element struct {
	Type     rune
	Name     string
	Value    float64
	Nodes    [4]int
	Source   string
	Symbolic bool
}

func (ci *Circuit) ReadNetFile(name string) error {
	f, err := os.Open(name)
	if err != nil {
		return err
	}
	defer f.Close()

	sc := bufio.NewScanner(f)
	ci.Elems = ci.Elems[:0]
	for nl := 1; sc.Scan(); nl++ {
		ln := strings.TrimSpace(sc.Text())
		if ln == "" || ln[0] == '#' || ln[0] == '*' {
			continue
		}

		var (
			e   Element
			n   int
			v   string
			err error
		)
		switch ln[0] {
		case 'R', 'L', 'C', 'I', 'V':
			n, err = fmt.Sscanf(ln, "%c%v %v %v %v", &e.Type, &e.Name, &e.Nodes[0], &e.Nodes[1], &v)
			if n != 5 {
				err = fmt.Errorf("expected %d values, got %d", 5, n)
			}
			if v == "Symbolic" {
				e.Value, e.Symbolic = 1, true
			} else {
				e.Value, _ = strconv.ParseFloat(v, 64)
			}

		case 'O':
			n, err = fmt.Sscanf(ln, "%c%v %v %v %v", &e.Type, &e.Name, &e.Nodes[0], &e.Nodes[1], &e.Nodes[2])
			if n != 5 {
				err = fmt.Errorf("expected %d values, got %d", 5, n)
			}

		case 'E', 'G':
			n, err = fmt.Sscanf(ln, "%c%v %v %v %v %v %v", &e.Type, &e.Name, &e.Nodes[0], &e.Nodes[1], &e.Nodes[2], &e.Nodes[3], &v)
			if n != 7 {
				err = fmt.Errorf("expected %d values, got %d", 7, n)
			}
			if v == "g" {
				e.Value, e.Symbolic = 1, true
			} else {
				e.Value, _ = strconv.ParseFloat(v, 64)
			}

		case 'H', 'F':
			n, err = fmt.Sscanf(ln, "%c%v %v %v %v %v", &e.Type, &e.Name, &e.Nodes[0], &e.Nodes[1], &e.Source, &v)
			if n != 6 {
				err = fmt.Errorf("expected %d values, got %d", 6, n)
			}
			if (ln[0] == 'H' && v == "r") || (ln[0] == 'F' && v == "f") {
				e.Value, e.Symbolic = 1, true
			} else {
				e.Value, _ = strconv.ParseFloat(v, 64)
			}

		default:
			err = fmt.Errorf("unknown circuit element '%c'", ln[0])
		}
		e.Name = fmt.Sprintf("%c%s", e.Type, e.Name)

		if err != nil {
			return fmt.Errorf("%v:%d: syntax error: %v", name, nl, err)
		}

		ci.Elems = append(ci.Elems, &e)
	}

	ci.Conns = make(map[int][]*Element)
	for _, ce := range ci.Elems {
		for _, nd := range ce.Nodes {
			ci.Conns[nd] = append(ci.Conns[nd], ce)
		}
	}

	ci.NumNodes = len(ci.Conns) - 1
	ci.NumVoltSources = 0
	for _, ce := range ci.Elems {
		switch ce.Type {
		case 'V', 'O', 'E', 'H':
			ci.NumVoltSources++
		}
	}

	for i := 0; i < ci.NumNodes; i++ {
		if _, found := ci.Conns[i]; !found {
			return fmt.Errorf("%v: node %d does not exist", name, i)
		}
	}

	return nil
}

func (ci *Circuit) makeMatrix(n, m int) Matrix {
	p := make(Matrix, n)
	for i := range p {
		p[i] = make([][]Element, m)
	}
	for i := range p {
		for j := range p[i] {
			p[i][j] = ci.makeConstantElement(0)
		}
	}
	return p
}

func (ci *Circuit) makeConstantElement(v float64) []Element {
	return []Element{Element{Type: '0', Value: v}}
}

func (ci *Circuit) mergeMatrices(in string, m ...Matrix) Matrix {
	var (
		r, c   int
		rm, cm int
		i      int
	)
	for _, ch := range in {
		if ch == ';' {
			r, c = r+rm, max(c, cm)
			rm, cm = 0, 0
			continue
		}
		rm = max(rm, len(m[i]))
		cm += len(m[i][0])
		i++
	}
	r, c = r+rm, max(c, cm)

	p := ci.makeMatrix(r, c)
	r, c = 0, 0
	rm, cm = 0, 0
	i = 0
	for _, ch := range in {
		if ch == ';' {
			r, c = r+rm, 0
			rm = 0
			continue
		}

		for j := range m[i] {
			for k := range m[i][j] {
				p[r+j][c+k] = m[i][j][k]
			}
		}
		rm = max(rm, len(m[i]))
		c += len(m[i][0])
		i++
	}

	return p
}

// The MNA matrix is the modified nodal analysis matrix to solve for all voltages in the system
// and some currents in the system in the form of Ax = z, we can solve the system by finding A^-1
func (ci *Circuit) GenMNA() (A Matrix, Z Matrix) {
	// The A matrix can be made of 4 smaller submatrices
	// A = [G B]
	//     [C D]
	G := ci.genG()
	B := ci.genB()
	C := ci.genC()
	D := ci.genD()
	A = ci.mergeMatrices("GB;CD", G, B, C, D)
	ci.cleanupMatrix(A)

	// The Z matrix can be made of 2 smaller submatrices
	// z = [i]
	//     [e]
	Z = ci.genZ()

	return
}

func (ci *Circuit) findNodeElements(nd int, pat string) []Element {
	var p []Element
	for _, ce := range ci.Conns[nd] {
		if strings.IndexRune(pat, ce.Type) >= 0 {
			p = append(p, *ce)
		}
	}
	return p
}

func (ci *Circuit) findNodeElementsPair(nd1, nd2 int, pat string) []Element {
	var p []Element
	x := ci.findNodeElements(nd1, pat)
	y := ci.findNodeElements(nd2, pat)
	for i := range x {
		for j := range y {
			if x[i].Name == y[j].Name {
				p = append(p, x[i])
			}
		}
	}
	return p
}

func (ci *Circuit) findElements(pat string) []Element {
	var p []Element
	for _, ce := range ci.Elems {
		if strings.IndexRune(pat, ce.Type) >= 0 {
			p = append(p, *ce)
		}
	}
	return p
}

func (ci *Circuit) negateNodeElements(p []Element) {
	for i := range p {
		p[i].Value = -p[i].Value
	}
}

func (ci *Circuit) cleanupMatrix(M Matrix) {
	for i := range M {
		for j := range M[i] {
			if M[i][j][0].Type == '0' && M[i][j][0].Value == 0 && len(M[i][j]) > 1 {
				M[i][j] = M[i][j][1:]
			}
		}
	}
}

func (ci *Circuit) genG() (G Matrix) {
	// The G matrix is an n×n matrix formed in two steps
	// 1) Each element in the diagonal matrix is equal to the sum of the conductance (one over the resistance)
	//    of each element connected to the corresponding node. So the first diagonal element
	//    is the sum of conductances connected to node 1, the second diagonal element is the sum of conductances connected to node 2, and so on.
	// 2) The off diagonal elements are the negative conductance of the element connected to the pair of
	//    corresponding node. Therefore a resistor between nodes 1 and 2 goes into the G matrix at location (1,2) and locations (2,1).
	G = ci.makeMatrix(ci.NumNodes, ci.NumNodes)
	for i := range G {
		for j := range G[i] {
			var p []Element
			if i == j {
				p = ci.findNodeElements(i+1, "RLC")
			} else {
				p = ci.findNodeElementsPair(i+1, j+1, "RLC")
				ci.negateNodeElements(p)
			}

			if p != nil {
				G[i][j] = p
			}
		}
	}

	// To add Voltage Controlled Current Source (VCCS)
	// If you examine the defining equations above you see that
	// in the row of G associated with "N+" we add "+Value"
	// to the column associated with "NC+" and "-Value" to the node associated with "NC-".
	// In the row of G associated with "N-" we add "-Value" to the column associated with "NC+"
	// and "+Value" to the node associated with "NC-".
	elems := ci.findElements("G")
	for _, ce := range elems {
		np := ce.Nodes[0] - 1
		nm := ce.Nodes[1] - 1
		ncp := ce.Nodes[2] - 1
		ncm := ce.Nodes[3] - 1

		G[np][ncp] = append(G[np][ncp], ce)
		G[nm][ncm] = append(G[nm][ncm], ce)

		ce.Value = -ce.Value
		G[np][ncm] = append(G[np][ncm], ce)
		G[nm][ncp] = append(G[nm][ncp], ce)
	}

	return
}

func (ci *Circuit) genB() (B Matrix) {
	// The B matrix is an n×m matrix with only 0, 1 and -1 elements.
	// Each location in the matrix corresponds to a particular voltage source
	// (first dimension) or a node (second dimension).
	// If the positive terminal of the ith voltage source is connected to node k,
	// then the element (i,k) in the B matrix is a 1.
	// If the negative terminal of the ith voltage source is connected to node k,
	// then the element (i,k) in the B matrix is a -1. Otherwise, elements of the B matrix are zero.

	// If a voltage source is ungrounded, it will have two elements in the B matrix
	// (a 1 and a -1 in the same column). If it is grounded it will only have one element in the matrix.

	// To add Current Controlled Voltage Source (CCVS)
	// B matrix: We add a column to the B matrix for the new voltage source and add +1 and -1 in the row numbered "N+" and "N-"
	// to account for the current added to (or subtracted from) that node.
	B = ci.makeMatrix(ci.NumNodes, ci.NumVoltSources)
	elems := ci.findElements("VOEH")
	for v, ce := range elems {
		i := ce.Nodes[0]
		j := ce.Nodes[1]
		if ce.Type == 'O' {
			i = ce.Nodes[2]
			j = 0
		}

		if i > 0 {
			B[i-1][v][0].Value = 1
		}
		if j > 0 {
			B[j-1][v][0].Value = -1
		}
	}

	// To add Current Controlled Current Source (CCCS)
	// For this element we need to add a current equal
	// to "Value·I_Vxxxxx" to node "N+" and subtract it from node "N-".
	// To do this we find the column of B that corresponds to the voltage source "Vxxxxx".
	// To the row "N+" of this column we add "Value" (so that it multiplies "I_Vxxxxx")
	// which effectively adds "Value·I_Vxxxxx" to that node.
	// Likewise, we subtract "Value" from the row "N-" of this same column of B.
	celems := ci.findElements("F")
	for _, fe := range celems {
		for v, ce := range elems {
			if fe.Source == ce.Name {
				i := fe.Nodes[0]
				j := fe.Nodes[1]

				if i > 0 {
					if B[i-1][v][0].Type == '0' {
						B[i-1][v] = B[i-1][v][:0]
					}

					B[i-1][v] = append(B[i-1][v], fe)
				}

				if j > 0 {
					if B[j-1][v][0].Type == '0' {
						B[j-1][v] = B[j-1][v][:0]
					}

					fe.Value = -fe.Value
					B[j-1][v] = append(B[j-1][v], fe)
				}
			}
		}
	}

	return
}

func (ci *Circuit) genC() (C Matrix) {
	// The C matrix is an m×n matrix with only 0, 1 and -1 elements.
	// Each location in the matrix corresponds to a particular node (first dimension) or
	// voltage source (second dimension).
	// If the positive terminal of the ith voltage source is connected to node k, then the element (k,i) in the C matrix is a 1.
	// If the negative terminal of the ith voltage source is connected to node k, then the element (k,i) in the C matrix is a -1.
	// Otherwise, elements of the C matrix are zero.
	// In other words, the C matrix is the transpose of the B matrix. (This is not the case when dependent sources are present.)

	// To add Operational Amplifiers
	// For each op-amp let the positive input terminal be at node k and negative terminal at node j;
	// the corresponding (ith) row of the C matrix has a 1 at location corresponding to the
	// positive terminal (k,i), and a -1 at the location corresponding to the negative terminal (j,i).
	// Otherwise, elements of the C matrix are zero.

	// To add Current Controlled Voltage Source (CCVS)
	// C matrix: We add a row for the new voltage source and we add +1 in the column corresponding to "N+" and "-1" in the column corresponding to "N-".

	C = ci.makeMatrix(ci.NumVoltSources, ci.NumNodes)
	elems := ci.findElements("VOEH")
	for v, ce := range elems {
		i := ce.Nodes[0]
		j := ce.Nodes[1]
		if i > 0 {
			C[v][i-1][0].Value = 1
		}
		if j > 0 {
			C[v][j-1][0].Value = -1
		}

		if ce.Type == 'E' {
			np := ce.Nodes[2]
			nm := ce.Nodes[3]

			C[v][nm-1] = append(C[v][nm-1], ce)

			ce.Value = -ce.Value
			C[v][np-1] = append(C[v][np-1], ce)
		}
	}
	return
}

func (ci *Circuit) genD() (D Matrix) {
	// The D matrix is an m×m matrix that is composed entirely of zeros.
	// (It can be non-zero if dependent sources are considered.)
	D = ci.makeMatrix(ci.NumVoltSources, ci.NumVoltSources)

	// D matrix: In the row corresponding to the new voltage source,
	// and the column corresponding to the controlling source we
	// subtract "Value". This has the effect of making that row correspond to the equation:
	elems := ci.findElements("VOEH")
	for v, he := range elems {
		if he.Type != 'H' {
			continue
		}

		he.Value = -he.Value
		for u, ce := range elems {
			if he.Source == ce.Name {
				D[v][u] = append(D[v][u], he)
				break
			}
		}
	}
	return
}

func (ci *Circuit) genZ() (Z Matrix) {
	// The z matrix holds our independent voltage and current sources and
	// will be developed as the combination of 2 smaller matrices i and e.

	// The z matrix is (m+n)×1 (n is the number of nodes, and m is the number of independent voltage sources) and:

	// the i matrix is n×1 and contains the sum of the currents through the passive elements into the corresponding node
	// (either zero, or the sum of independent current sources).
	// the e matrix is m×1 and holds the values of the independent voltage sources.

	// To add Operation Amplifiers:
	// The rule for the i matrix is unchanges.
	// The rule for the e matrix does change.
	// The e matrix is an 1×m matrix with each element of the matrix corresponding to a voltage source.
	// If the element in the e matrix corresponds to an independent source it is set equal to the value of that voltage source.
	// If the element corresponds to an op-amp, then its value is set to zero.

	// To add Current Controlled Voltage Source (CCVS):
	// e matrix: Add a row with a zero in it to account for the new dependent voltage source.

	Z = ci.makeMatrix(ci.NumNodes+ci.NumVoltSources, 1)

	elems := ci.findElements("I")
	maps := make(map[int][]Element)
	for _, ce := range elems {
		i := ce.Nodes[0] - 1
		j := ce.Nodes[1] - 1
		maps[i] = append(maps[i], ce)

		ce.Value = -ce.Value
		maps[j] = append(maps[j], ce)
	}
	for k, v := range maps {
		if k >= 0 {
			Z[k][0] = v
		}
	}

	elems = ci.findElements("VOHE")
	for i, j := ci.NumNodes, 0; i < ci.NumNodes+ci.NumVoltSources; i, j = i+1, j+1 {
		if elems[j].Type == 'V' {
			Z[i][0] = []Element{elems[j]}
		}
	}

	return
}

func (ci *Circuit) Dump(w io.Writer) {
	ci.dumpNetlist(w)
	fmt.Fprintf(w, "\n")
}

func (ci *Circuit) dumpNetlist(w io.Writer) {
	fmt.Fprintf(w, "%d Nodes, %d Voltage Sources\n", ci.NumNodes, ci.NumVoltSources)
	fmt.Fprintf(w, "Netlist:\n")
	for _, ce := range ci.Elems {
		v := fmt.Sprint(ce.Value)
		switch ce.Type {
		case 'R', 'L', 'C', 'I', 'V':
			if ce.Symbolic {
				v = "Symbolic"
			}
			fmt.Fprintf(w, "%s %v %v %v\n", ce.Name, ce.Nodes[0], ce.Nodes[1], v)

		case 'O':
			fmt.Fprintf(w, "%s %v %v %v\n", ce.Name, ce.Nodes[0], ce.Nodes[1], ce.Nodes[2])

		case 'E', 'G':
			if ce.Symbolic {
				v = "g"
			}
			fmt.Fprintf(w, "%s %v %v %v %v %v\n", ce.Name, ce.Nodes[0], ce.Nodes[1], ce.Nodes[2], ce.Nodes[3], v)

		case 'H', 'F':
			if ce.Symbolic {
				if ce.Type == 'H' {
					v = "r"
				} else {
					v = "f"
				}
			}
			fmt.Fprintf(w, "%s %v %v %v %v\n", ce.Name, ce.Nodes[0], ce.Nodes[1], ce.Source, v)
		}
	}

	A, z := ci.GenMNA()
	ci.dumpMatrix("\nA Matrix\n", w, A)
	ci.dumpMatrix("\nz Matrix\n", w, z)
}

func (ci *Circuit) dumpMatrix(title string, w io.Writer, m Matrix) {
	tw := tabwriter.NewWriter(w, 1, 4, 1, ' ', 0)
	defer tw.Flush()

	fmt.Fprintf(tw, "%s\n", title)
	for i := range m {
		fmt.Fprintf(tw, "[")
		for j := range m[i] {
			for k, ce := range m[i][j] {
				if ce.Value < 0 {
					fmt.Fprintf(tw, "-")
				}

				switch ce.Type {
				case '0':
					fmt.Fprintf(tw, "%.0f", math.Abs(ce.Value))

				case 'R':
					fmt.Fprintf(tw, "1/%v", ce.Name)

				case 'L':
					fmt.Fprintf(tw, "1/(s*%v)", ce.Name)

				case 'C':
					fmt.Fprintf(tw, "s*%v", ce.Name)

				case 'I', 'V', 'O', 'F', 'G', 'E', 'H':
					fmt.Fprintf(tw, "%v", ce.Name)

				default:
					panic("unreachable")
				}

				if k+1 < len(m[i][j]) {
					fmt.Fprintf(tw, " + ")
				}
			}

			if j+1 < len(m[i]) {
				fmt.Fprintf(tw, ",\t")
			}
		}
		fmt.Fprintf(tw, "]\n")
	}
}
