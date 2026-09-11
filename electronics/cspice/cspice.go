// ported from https://github.com/andrewytliu/cspice
package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"log"
	"math"
	"math/cmplx"
	"os"
	"sort"
	"strings"
)

func main() {
	log.SetPrefix("cspice: ")
	log.SetFlags(0)

	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 3 {
		usage()
	}

	ci := &Circuit{}
	sm := &Simulation{}
	err := ci.LoadNetFile(flag.Arg(0))
	ck(err)

	ci.Print(os.Stdout)
	err = sm.Init(ci, flag.Arg(1), flag.Arg(2))
	ck(err)

	for _, cfg := range ci.Analyses {
		err = sm.Simulate(cfg)
		ck(err)
	}
	err = sm.Simulate(&Analysis{Type: 'C'})
	ck(err)

	err = sm.PlotFile.Close()
	ck(err)

	err = sm.DotFile.Close()
	ck(err)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] netlist output.plt output.gv")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func max(a, b int) int {
	if a > b {
		return a
	}
	return b
}

func min(a, b int) int {
	if a < b {
		return a
	}
	return b
}

func rad2deg(r float64) float64 {
	return r * 180 / math.Pi
}

func fnv1a(str string) uint64 {
	const prime = 1099511628211
	result := uint64(14695981039346656037)
	length := uint64(0)
	for _, ch := range str {
		result ^= uint64(ch)
		result *= prime
		length++
	}
	result ^= length
	result *= prime
	return result
}

func parseUnit(str string) float64 {
	var val float64
	var unit string
	fmt.Sscanf(str, "%v%v", &val, &unit)
	switch unit {
	case "m":
		val *= 1e-3
	case "u":
		val *= 1e-6
	case "n":
		val *= 1e-9
	case "p":
		val *= 1e-12
	case "f":
		val *= 1e-15
	case "k", "K":
		val *= 1e3
	case "M":
		val *= 1e6
	case "G":
		val *= 1e9
	case "T":
		val *= 1e12
	}
	return val
}

func trapezoidIntegrate(times []float64, tf *Transfer, shift int) (result []float64) {
	// shift has a default value of 8

	// this is for unit step response, that is,
	// In[t] = 1.0 for t >= 0
	//
	// for convenient, make Num and Den have same size.
	N := max(len(tf.Den), len(tf.Num))
	den := make([]float64, N)
	num := make([]float64, N)
	copy(den, tf.Den)
	copy(num, tf.Num)

	newU := make([]float64, N)
	oldU := make([]float64, N)
	oldU[0] = 1 / den[N-1]

	result = append(result, 0)
	for t := 1; t < len(times); t++ {
		step := (times[t] - times[t-1]) / float64(N<<shift) // Real time step is smaller than output set
		DEN := 0.0

		for k := 0; k < N; k++ {
			DEN = DEN*(step/2.0) + den[k]
		}

		for i := 0; i < N<<shift; i++ {
			newU[0] = 0.0
			sum1 := oldU[0] * (step / 2.0)
			sum2 := 0.0
			for k := 1; k < N; k++ {
				sum2 = sum2*(step/2.0) + oldU[k]
				newU[0] += den[N-k-1] * (sum1 + sum2)
				sum1 = (step / 2.0) * (sum1 + oldU[k])
			}
			newU[0] = (1.0 - newU[0]) / DEN

			for k := 1; k < N; k++ {
				newU[k] = oldU[k] + (step/2.0)*(oldU[k-1]+newU[k-1])
			}
			for k := 0; k < N; k++ {
				oldU[k] = newU[k]
			}
		}

		out := 0.0
		for k := 0; k < N; k++ {
			out += num[N-k-1] * newU[k]
		}

		result = append(result, out)
	}

	return
}

// the idea is to use node analysis to find the transfer function
// a node and connections represents a graph G(V, E)
// node = vertex; connection = edges

// circuit elements are stored inside connections, they represent
// elements one encounters traversing the connection from one node to another node,
// the path between nodes can be directed or undirected, in the cases of circuit elements
// they are mostly undirected (resistor, capacitor, inductor, etc)

// Example:
// We have 2 nodes A and B
// Node A is connected to Node B and on the path we encounter a resistor and a capacitor

// Node A <- Connection{Resistor} -> Node B
// Node A <- Connection{Capacitor} -> Node B
// Node B <- Connection{Resistor} -> Node A
// Node B <- Connection{Capacitor} -> Node A
// Node A will contain 2 connection path to node B
// Node B will contain 2 connection path to node A
// The connection path are bidirectional

// We will store 2 Connection structure inside node A
// We will store 2 Connection structure inside node B
// Inside the connection structure we will store one resistor for one connection
// and one capacitor for the other connection

type Simulation struct {
	Circuit   *Circuit
	Transfers map[*Source]Transfer
	PlotFile  *os.File
	DotFile   *os.File
}

type Circuit struct {
	IDs      map[int]int // map node id to index to nodes
	Nodes    []*Node     // list of node indexed by a node id
	Elems    []Element   // list of circuit elements
	Sources  []*Source   // list of voltage/current sources
	OutputID [2]int      // node id to high/low node
	Analyses []*Analysis // frequency/time parameters for analysis
}

type Analysis struct {
	Type   int
	Start  float64
	End    float64
	Step   float64
	File   string
	Source string
}

type Transfer struct {
	Num []float64
	Den []float64
}

type Node struct {
	ID     int
	Conns  []*Connection // connections coming out of this node to other nodes
	Equivs []*Equivalent
}

type Connection struct {
	Dest *Node   // the destination node we get through if we traverse this connection
	Elem Element // a circuit element on this connection path
}

type Source struct {
	Type int
	Name string
	Prev float64
	Next float64
	Node [2]int
}

type Equivalent struct {
	Node   *Node
	Reason *Source
}

type Element interface {
	Base() *Component
	Formula() string
	Coeff() float64
	String() string
}

type Component struct {
	Name  string
	Value float64
	Sign  float64
	Order int
}

func (cp *Component) Coeff() float64 {
	return cp.Value
}

func (cp *Component) String() string {
	return fmt.Sprintf("%-5s % .03e", cp.Name, cp.Value)
}

func (cp *Component) Formula() string {
	return ""
}

func (cp *Component) Base() *Component {
	return cp
}

type Dummy struct{ Component }
type Resistor struct{ Component }
type Capacitor struct{ Component }
type Inductor struct{ Component }
type VCCS struct{ Component }

func (cp *Dummy) Formula() string {
	return " Dummy "
}

func (cp *VCCS) Formula() string {
	return fmt.Sprintf("   %v  ", cp.Name)
}

func (cp *Resistor) Coeff() float64 {
	return 1 / cp.Value
}

func (cp *Resistor) Formula() string {
	return fmt.Sprintf("1 /  %v", cp.Name)
}

func (cp *Capacitor) Formula() string {
	return fmt.Sprintf("  s%v  ", cp.Name)
}

func (cp *Inductor) Coeff() float64 {
	return 1 / cp.Value
}

func (cp *Inductor) Formula() string {
	return fmt.Sprintf("1 / s%v", cp.Name)
}

func (n *Node) AddConn(conn *Connection) {
	n.Conns = append(n.Conns, conn)
}

func (n *Node) AddEquiv(equiv *Equivalent) {
	n.Equivs = append(n.Equivs, equiv)
}

func (s *Source) Pulse() float64 {
	return s.Next - s.Prev
}

func (ci *Circuit) IndexByID(id int) int {
	index, found := ci.IDs[id]
	if found {
		return index
	}
	index = len(ci.Nodes)
	ci.Nodes = append(ci.Nodes, &Node{ID: id})
	ci.IDs[id] = index
	return index
}

func (ci *Circuit) NodeByID(id int) *Node {
	return ci.Nodes[ci.IndexByID(id)]
}

func (ci *Circuit) Print(w io.Writer) {
	fmt.Fprintf(w, "===== Circuit Detail =====\n")
	for _, n := range ci.Nodes {
		fmt.Fprintf(w, "[%2d]\n", n.ID)
		for _, e := range n.Conns {
			fmt.Fprintf(w, " -> [%2d] %v\n", e.Dest.ID, e.Elem)
		}
	}

	fmt.Fprintf(w, "--------- Source ---------\n")
	for _, s := range ci.Sources {
		fmt.Fprintf(w, "%v [%v] -> [%v] %.3e %.3e\n", s.Name, s.Node[0], s.Node[1], s.Prev, s.Next)
	}
	fmt.Fprintf(w, "==========================\n")
}

func (ci *Circuit) addComponent(line string, typ rune) error {
	var (
		name   string
		n1, n2 int
		v1     string
	)
	_, err := fmt.Sscan(line, &name, &n1, &n2, &v1)
	if err != nil {
		return err
	}

	var ce Element
	cp := Component{name, parseUnit(v1), 1, 0}
	switch typ {
	case 'R':
		cp.Order = 0
		ce = &Resistor{cp}
	case 'L':
		cp.Order = -1
		ce = &Inductor{cp}
	case 'C':
		cp.Order = 1
		ce = &Capacitor{cp}
	}

	ci.Elems = append(ci.Elems, ce)
	nod1 := ci.NodeByID(n1)
	nod2 := ci.NodeByID(n2)
	nod1.AddConn(&Connection{nod2, ce})
	nod2.AddConn(&Connection{nod1, ce})
	return nil
}

func (ci *Circuit) addSource(line string, typ int) error {
	var (
		name   string
		n1, n2 int
		s1, s2 string
		v1, v2 float64
	)

	fmt.Sscan(line, &name, &n1, &n2, &s1, &s2)
	v1 = parseUnit(s1)
	v2 = parseUnit(s2)
	if n1 == n2 {
		if typ == 'V' && v1 != 0 || v2 != 0 {
			return fmt.Errorf("illegal voltage source: %v", line)
		}
		return nil
	}

	ci.Sources = append(ci.Sources, &Source{
		Type: typ,
		Name: name,
		Node: [2]int{n1, n2},
		Prev: v1,
		Next: v2,
	})
	if typ == 'V' {
		nod1 := ci.NodeByID(n1)
		nod2 := ci.NodeByID(n2)
		nod1.AddEquiv(&Equivalent{nod2, ci.Sources[len(ci.Sources)-1]})
		nod2.AddEquiv(&Equivalent{nod1, ci.Sources[len(ci.Sources)-1]})
	}

	return nil
}

func (ci *Circuit) addVCCS(line string) error {
	var (
		name           string
		n1, n2, n3, n4 int
		s1             string
		v1             float64
	)
	_, err := fmt.Sscan(line, &name, &n1, &n2, &n3, &n4, &s1)
	if err != nil {
		return err
	}
	v1 = parseUnit(s1)

	ce := &VCCS{Component{name, v1, 1, 0}}
	re := &VCCS{Component{name, v1, -1, 0}}

	nod1 := ci.NodeByID(n1)
	nod2 := ci.NodeByID(n2)
	nod3 := ci.NodeByID(n3)
	nod4 := ci.NodeByID(n4)

	ci.Elems = append(ci.Elems, ce)
	ci.Elems = append(ci.Elems, re)

	if n3 != n2 {
		nod3.AddConn(&Connection{nod2, ce})
	}
	if n3 != n1 {
		nod3.AddConn(&Connection{nod1, re})
	}
	if n4 != n2 {
		nod4.AddConn(&Connection{nod2, re})
	}
	if n4 != n1 {
		nod4.AddConn(&Connection{nod1, ce})
	}

	return err
}

func (ci *Circuit) addAnalysis(line string, typ int) error {
	var (
		op               string
		start, end, step string
		source, file     string
		err              error
	)

	if typ == 'F' {
		_, err = fmt.Sscan(line, &op, &start, &end, &step, &source, &file)
	} else {
		_, err = fmt.Sscan(line, &op, &start, &end, &step, &file)
	}
	if err != nil {
		return err
	}

	ci.Analyses = append(ci.Analyses, &Analysis{
		Type:   typ,
		Start:  parseUnit(start),
		End:    parseUnit(end),
		Step:   parseUnit(step),
		Source: source,
		File:   file,
	})
	return nil
}

func (ci *Circuit) LoadNetFile(name string) error {
	f, err := os.Open(name)
	if err != nil {
		return err
	}
	defer f.Close()

	ci.IDs = make(map[int]int)

	sc := bufio.NewScanner(f)
	for sc.Scan() {
		line := sc.Text()
		line = strings.TrimSpace(line)
		if line == "" || line[0] == '#' {
			continue
		}

		switch line[0] {
		case 'I':
			// I[ID] [NODE1] [NODE2] [VALUE1] [VALUE2] # Current source
			err = ci.addSource(line, 'I')

		case 'V':
			// V[ID] [NODE1] [NODE2] [VALUE1] [VALUE2] # Voltage source
			err = ci.addSource(line, 'V')

		case 'G':
			// G[ID] [NODE1] [NODE2] [NODE3] [NODE4] [VALUE]
			// VCCS: I = (VALUE) * (V1 - V2)
			err = ci.addVCCS(line)

		case 'O':
			// OUT [HINODE] [LONODE]
			var op string
			_, err = fmt.Sscan(line, &op, &ci.OutputID[0], &ci.OutputID[1])

		case 'R':
			// R[ID] [NODE1] [NODE2] [VALUE] # Resistor
			err = ci.addComponent(line, 'R')

		case 'L':
			// L[ID] [NODE1] [NODE2] [VALUE] # Inductor
			err = ci.addComponent(line, 'L')

		case 'C':
			// C[ID] [NODE1] [NODE2] [VALUE] # Capacitor
			err = ci.addComponent(line, 'C')

		default:
			switch xline := strings.ToUpper(line); {
			case strings.HasPrefix(xline, "FREQ"):
				// FREQ  [START] [END] [STEP] [SRC] [OUTPUT] # Frequency response
				err = ci.addAnalysis(line, 'F')

			case strings.HasPrefix(xline, "TIME"):
				// TIME  [START] [END] [STEP] [OUTPUT] # Transient response
				err = ci.addAnalysis(line, 'T')

			default:
				err = fmt.Errorf("unknown directive")
			}
		}

		if err != nil {
			return fmt.Errorf("failed to read netlist: %q: %v", line, err)
		}
	}
	return nil
}

func (ci *Circuit) propagateEquivalents(index int, visited *[]bool, currentSource *Source) {
	eq := ci.Nodes[index].Equivs
	for _, e := range eq {
		if e.Reason != currentSource {
			// this source is not "on" currently
			i := ci.IndexByID(e.Node.ID)
			if (*visited)[i] != (*visited)[index] {
				// found a node with different status
				(*visited)[i] = (*visited)[index]
				ci.propagateEquivalents(i, visited, currentSource)
			}
		}
	}
}

// visited[i]  -> has nodes[i] been contained in the tree yet?
// used[i][j]  -> has nodes[i].connections[j] been used yet?
// current_tree-> an array storing current tree edges
// result      -> an array storing tree edges of all spanning trees
func (ci *Circuit) dfs(visited *[]bool, used *[][]bool, currentTree *[]Element, result *[][]Element, trees *[][2]int64, currentSource *Source) {
	var recoverList [][2]int

	done := true
	for v, node := range ci.Nodes {
		if (*visited)[v] {
			continue
		}

		done = false
		for i, conn := range node.Conns {
			destID := conn.Dest.ID
			destIndex := ci.IndexByID(destID)

			if (*visited)[destIndex] && !(*used)[v][i] {
				recoverList = append(recoverList, [2]int{v, i})

				(*visited)[v] = true
				ci.propagateEquivalents(v, visited, currentSource)
				(*used)[v][i] = true
				(*currentTree) = append(*currentTree, conn.Elem)

				ci.dfs(visited, used, currentTree, result, trees, currentSource)

				(*currentTree) = (*currentTree)[: len(*currentTree)-1 : len(*currentTree)-1]
				(*visited)[v] = false
				ci.propagateEquivalents(v, visited, currentSource)
			}
		}
	}

	// recover used
	for _, r := range recoverList {
		(*used)[r[0]][r[1]] = false
	}

	if done {
		// check if elimination could be done
		ci.doElimination(currentTree, result, trees)
	}
}

func (ci *Circuit) doElimination(currentTree *[]Element, result *[][]Element, trees *[][2]int64) {
	sign := int64(1)
	hval := uint64(0)
	for _, ce := range *currentTree {
		// if two elements only different in sign,
		// they should have same formula.
		// Ex: gm and -gm

		// use + , since it's commutative, that is,
		// the order of elements doesn't matter.

		// Be ware that, we have to take the risk of
		// hash(a) + hash(b) == hash(c) + hash(d),
		// though I don't think it would happen so easily.
		cp := ce.Base()
		hval += fnv1a(ce.Formula())
		sign *= int64(cp.Sign)
	}
	elimID := -1
	for i, t := range *trees {
		if t[0] == -sign && uint64(t[1]) == hval {
			elimID = i
			break
		}
	}

	if elimID == -1 {
		*result = append(*result, *currentTree)
		*trees = append(*trees, [2]int64{sign, int64(hval)})
	} else {
		*result = append((*result)[:elimID], (*result)[elimID+1:]...)
		*trees = append((*trees)[:elimID], (*trees)[elimID+1:]...)
	}
}

func (ci *Circuit) enumTree(ref *Node, currentSource *Source) [][]Element {
	var (
		result      [][]Element
		currentTree []Element
		trees       [][2]int64
	)
	size := len(ci.Nodes)
	visited := make([]bool, size)
	used := make([][]bool, size)
	for i := 0; i < size; i++ {
		used[i] = make([]bool, len(ci.Nodes[i].Conns))
	}

	refIndex := ci.IndexByID(ref.ID)
	visited[refIndex] = true
	ci.propagateEquivalents(refIndex, &visited, currentSource)
	ci.dfs(&visited, &used, &currentTree, &result, &trees, currentSource)
	return result
}

func (sm *Simulation) Init(ci *Circuit, plotfile, dotfile string) error {
	var err error
	sm.PlotFile, err = os.Create(plotfile)
	if err != nil {
		return err
	}
	sm.DotFile, err = os.Create(dotfile)
	if err != nil {
		sm.PlotFile.Close()
		return err
	}

	sm.Circuit = ci
	sm.findTransfers()
	return nil
}

func (sm *Simulation) findTransfers() {
	sm.Transfers = make(map[*Source]Transfer)

	ci := sm.Circuit
	for _, src := range ci.Sources {
		tf := sm.findFormula(src)
		sm.Transfers[src] = tf
	}
}

// find the transfer function at a source
func (sm *Simulation) findFormula(src *Source) Transfer {
	ci := sm.Circuit

	// ports from the source to output
	ih := ci.NodeByID(src.Node[0])
	il := ci.NodeByID(src.Node[1])
	oh := ci.NodeByID(ci.OutputID[0])
	ol := ci.NodeByID(ci.OutputID[1])

	var num, den [][]Element
	switch src.Type {
	case 'V':
		// 1. back up and remove all the elements leaving input_high
		backup := ih.Conns
		ih.Conns = nil

		// 2. find den:
		//    a. add a dummy cell (value = 1) from input_high to input_low
		ih.AddConn(&Connection{il, &Dummy{Component{"Dummy element for DEN (+1)", 1, 1, 0}}})

		//    b. get all spanning trees (input_low is the reference node)
		den = ci.enumTree(il, src)

		//    c. remove dummy cell
		ih.Conns = nil

		// 3. find num:
		//    a. add a dummy cell (value =-1) from input_high to output_high
		ih.AddConn(&Connection{oh, &Dummy{Component{"Dummy element for NUM (-1)", 1, -1, 0}}})

		//    b. add a dummy cell (value = 1) from input_high to output_low
		ih.AddConn(&Connection{ol, &Dummy{Component{"Dummy element for NUM (+1)", 1, 1, 0}}})

		//    c. get all spanning trees (input_low is the reference node)
		num = ci.enumTree(il, src)

		//    d remove dummy cell
		// 4. restore the elements leaving input_high
		ih.Conns = backup

	case 'I':
		// 1. find den:
		//    a. get all spanning trees (input_low is the reference node)
		den = ci.enumTree(il, src)

		// 2. back up and remove all the elements leaving input_high
		backup := ih.Conns
		ih.Conns = nil

		// 3. find num:
		//    a. add a dummy cell (value =-1) from input_high to output_high
		ih.AddConn(&Connection{oh, &Dummy{Component{"Dummy element for NUM (-1)", 1, -1, 0}}})

		//    b. add a dummy cell (value =+1) from input_high to output_low
		ih.AddConn(&Connection{ol, &Dummy{Component{"Dummy element for NUM (+1)", 1, 1, 0}}})

		//    c. get all spanning trees (input_low is the reference node)
		num = ci.enumTree(il, src)

		//    d remove dummy cell
		// 4. restore the elements leaving input_high
		ih.Conns = backup

	default:
		panic("unknown analysis")
	}

	fmt.Println("======== Print out Den Trees ========")
	printFormula(os.Stdout, den)
	fmt.Println()
	fmt.Println("======== Print out Num Trees ========")
	printFormula(os.Stdout, num)
	fmt.Println()

	// 5. expand formula
	cden := expandFormula(den)
	cnum := expandFormula(num)

	// 6. adjust the order of num and den
	cnum, cden = adjustFormulaOrder(cnum, cden)
	return Transfer{
		Num: convertFormulaToCoeff(cnum),
		Den: convertFormulaToCoeff(cden),
	}
}

// key, val => order, coefficient
// we want to map this to an array for more efficient proessing
func convertFormulaToCoeff(p map[int]float64) []float64 {
	var maxOrder int
	for order, _ := range p {
		maxOrder = max(maxOrder, order)
	}

	coeff := make([]float64, maxOrder+1)
	for order, value := range p {
		coeff[order] = value
	}
	return coeff
}

// find the minimal order and subtract them out to make the polynomial normalized
// Example:
// s^-4 + s^2 + s^3 becomes
// s^0 + s^6 + s^7
func adjustFormulaOrder(cnum, cden map[int]float64) (num, den map[int]float64) {
	var minOrder int
	for order, _ := range cnum {
		minOrder = min(minOrder, order)
	}
	for order, _ := range cden {
		minOrder = min(minOrder, order)
	}

	num = make(map[int]float64)
	den = make(map[int]float64)
	for order, coeff := range cnum {
		num[order-minOrder] = coeff
	}
	for order, coeff := range cden {
		den[order-minOrder] = coeff
	}
	return
}

// key: value => order:coefficient
// that is, coefficient * s^order
// need to shift the orders and make them all nonnegative,
// then one can use evalFormula(coefficients, freq) to evaluate.
func expandFormula(p [][]Element) map[int]float64 {
	result := make(map[int]float64)
	for i := range p {
		// each column of circuit elements represents
		// one coefficient of an order determined by all of the
		// circuit elements of that column

		// calculates coefficient * s^order
		value := 1.0
		order := 0
		for j := range p[i] {
			ce := p[i][j]
			cp := ce.Base()

			value *= ce.Coeff() * cp.Sign
			order += cp.Order
		}
		result[order] += value
	}
	return result
}

func printFormula(w io.Writer, p [][]Element) {
	for i := range p {
		line := ""
		sign := 1.0
		for j := range p[i] {
			ce := p[i][j]
			cp := ce.Base()

			_, isDummy := ce.(*Dummy)
			if !isDummy {
				line += fmt.Sprintf("(%v) ", ce.Formula())
			}
			sign *= cp.Sign
		}
		if sign == 1 {
			if i > 0 {
				fmt.Fprintf(w, " + %v\n", line)
			} else {
				fmt.Fprintf(w, "   %v\n", line)
			}
		} else {
			fmt.Fprintf(w, " - %v\n", line)
		}
	}
}

// coefficients is an array contains a_0, a_1, a_2, ... a_n and this function
// evaluate sum[a_k * s^k], where s = i * 2 * pi * freq, k = 0 ... n
func evalFormula(coeffs []float64, freq float64) complex128 {
	omega := 2 * math.Pi * freq
	im := 0.0
	re := 0.0

	s := 1.0
	for order := range coeffs {
		// i^0 = 1, i^1 = i, i^2 = -1, i^3 = -i
		switch order & 3 {
		case 0:
			re += s * coeffs[order]
		case 1:
			im += s * coeffs[order]
		case 2:
			re -= s * coeffs[order]
		case 3:
			im -= s * coeffs[order]
		}
		s *= omega
	}
	return complex(re, im)
}

func (sm *Simulation) Simulate(cfg *Analysis) error {
	switch cfg.Type {
	case 'C':
		return sm.doConnections()
	case 'F':
		return sm.doFreq(cfg)
	case 'T':
		return sm.doTime(cfg)
	}
	return fmt.Errorf("unknown analysis type %q", cfg.Type)
}

func (sm *Simulation) doFreq(cfg *Analysis) error {
	var (
		src *Source
		tf  Transfer
		t   []float64
		y   []complex128
	)
	for key, val := range sm.Transfers {
		if key.Name == cfg.Source {
			src = key
			tf = val
			break
		}
	}
	if src == nil {
		return fmt.Errorf("can't find source %q", cfg.Source)
	}

	// step through the frequency spectrum and evaluate it using the transfer function
	// H(s) at points s = 2*pi*freq, this will give values we need to calculate
	// magnitude/phase response
	ratio := math.Exp(math.Log(10) / cfg.Step)
	for freq := cfg.Start; freq <= cfg.End; freq *= ratio {
		t = append(t, freq)
		y = append(y, evalFormula(tf.Num, freq)/evalFormula(tf.Den, freq))
	}
	sm.plotFreq(t, y, cfg)

	return nil
}

func (sm *Simulation) doTime(cfg *Analysis) error {
	var (
		t []float64
		y []float64
	)

	// 1. Find out the prev value of Vout
	// 2. For each src that changes value, find it's unit step response, V(t)
	// 3. Vout = DC + Sum{ Delta * V(t) , for each src }
	start, end := cfg.Start, cfg.End
	step := cfg.Step
	if start >= end {
		end, start = start, end
	}

	// if there are too less node, make config.step be 1 / 20 of total times
	if step <= 0 || (end-start)/step < 40 {
		step = (end - start) / 40
	}

	// generate times, first, from t = 0 to t = config.start
	// we need this for the integration for the inverse laplace transform
	if start > 0 {
		substep := start / 80
		for s := 0.0; s < start; s += substep {
			y = append(y, 0)
			t = append(t, s)
		}
	}

	// generate times from t = config.start to config.end
	for s := start; s <= end; s += step {
		y = append(y, 0)
		t = append(t, s)
	}

	// based on the superposition principle, we can loop over all
	// sources of current/voltage, evaluate them using the transfer function
	// and then add them up to get the full response
	prev := 0.0
	for src, tf := range sm.Transfers {
		// evaluate the transfer function at DC (freq=0)
		v := evalFormula(tf.Num, 0) / evalFormula(tf.Den, 0)

		// prev is the accumulator for all the output response
		// at the beginning, it represents a bias/shift to some value
		prev += cmplx.Abs(v) * src.Prev

		// if there is a pulse (unit step), we need to integrate it
		// to find the output at time t, every point is an integral
		// up to that time (we are calculating the impulse response of the
		// system given a transfer function, which is the inverse laplace transform)
		if src.Pulse() > 0 {
			yt := trapezoidIntegrate(t, &tf, 0)
			for i := range y {
				y[i] += yt[i] * src.Pulse()
			}
		}
	}

	// add the accumulated value at the beginning to bias/shift the output
	for i := range y {
		y[i] += prev
	}

	// chop of the transient response at the beginning
	// since the config start time did not specify it
	for i := range y {
		if t[i] >= start {
			t = t[i:]
			y = y[i:]
			break
		}
	}
	sm.plotTime(t, y, cfg)

	return nil
}

// frequency response consist of magnitude and phase response
func (sm *Simulation) plotFreq(t []float64, y []complex128, cfg *Analysis) {
	w := bufio.NewWriter(sm.PlotFile)
	defer w.Flush()

	fmt.Fprintf(w, "set terminal postscript eps enhanced color solid\n")
	fmt.Fprintf(w, "set output '%s'\n", cfg.File)
	fmt.Fprintf(w, "set multiplot layout 2, 1 title 'Frequency response'\n")
	fmt.Fprintf(w, "set title 'Magnitude'\n")
	fmt.Fprintf(w, "set xlabel 'Frequency (Hz)'\n")
	fmt.Fprintf(w, "set ylabel 'Gain (dB)'\n")
	fmt.Fprintf(w, "set log x\n")
	fmt.Fprintf(w, "plot '-' title '' with line\n")

	// plot magnitude response
	// 20*log10(|H|)
	for i := range t {
		fmt.Fprintf(w, "%v %v\n", t[i], 20*math.Log10(cmplx.Abs(y[i])))
	}

	fmt.Fprintf(w, "e\n")
	fmt.Fprintf(w, "set title 'Phase'\n")
	fmt.Fprintf(w, "set xlabel 'Frequency (Hz)'\n")
	fmt.Fprintf(w, "set ylabel 'Argument (Degree)'\n")
	fmt.Fprintf(w, "set yrange [-180:180]\n")
	fmt.Fprintf(w, "plot '-' title '' with line\n")

	// plot phase response
	// degrees(arg(H))
	for i := range t {
		fmt.Fprintf(w, "%v %v\n", t[i], rad2deg(cmplx.Phase(y[i])))
	}

	fmt.Fprintf(w, "e\n")
	fmt.Fprintf(w, "unset multiplot\n")
	fmt.Fprintf(w, "set yrange [*:*]\n")
	fmt.Fprintf(w, "unset log\n")
}

func (sm *Simulation) plotTime(t, y []float64, cfg *Analysis) {
	w := bufio.NewWriter(sm.PlotFile)
	defer w.Flush()

	fmt.Fprintf(w, "set terminal postscript eps enhanced color solid\n")
	fmt.Fprintf(w, "set title 'Time response'\n")
	fmt.Fprintf(w, "set xlabel 'Time (s)\n")
	fmt.Fprintf(w, "set ylabel 'Voltage (V)'\n")
	fmt.Fprintf(w, "set output '%s'\n", cfg.File)
	fmt.Fprintf(w, "plot '-' with lines title ''\n")

	for i := range t {
		fmt.Fprintf(w, "%v %v\n", t[i], y[i])
	}

	fmt.Fprintf(w, "e\n")
}

func (sm *Simulation) doConnections() error {
	w := bufio.NewWriter(sm.DotFile)
	defer w.Flush()

	fmt.Fprintf(w, "graph circuit {\n")

	m := make(map[string][]int)
	ci := sm.Circuit
	for _, s := range ci.Sources {
		m[s.Name] = []int{s.Node[0], s.Node[1]}
	}
	for _, n := range ci.Nodes {
		for _, c := range n.Conns {
			cp := c.Elem.Base()
			m[cp.Name] = append(m[cp.Name], n.ID)
			m[cp.Name] = append(m[cp.Name], c.Dest.ID)
		}
	}

	for k := range m {
		m[k] = dedupInts(m[k])
	}

	s := make(map[[2]string]bool)
	for k1 := range m {
		for k2 := range m {
			if k1 == k2 {
				continue
			}

		loop:
			for _, v1 := range m[k1] {
				for _, v2 := range m[k2] {
					l := [2]string{k1, k2}
					sort.Strings(l[:])
					if v1 == v2 && !s[l] {
						fmt.Fprintf(w, "\t%s -- %s\n", k1, k2)
						s[l] = true
						break loop
					}
				}
			}
		}
	}

	fmt.Fprintf(w, "}\n")

	return nil
}

func dedupInts(p []int) []int {
	var r []int
	m := make(map[int]bool)
	for i := range p {
		if m[p[i]] {
			continue
		}
		r = append(r, p[i])
		m[p[i]] = true
	}
	return r
}
