// playing around with elementary row operations
package main

import (
	"bufio"
	"bytes"
	"fmt"
	"math/big"
	"os"
	"sort"
	"strconv"
	"strings"
)

func main() {
	var m, n Matrix
	var u []Matrix
	var h []string
	var err error

	defer fmt.Println()
	stdin := &Stdin{}
	s := bufio.NewScanner(stdin)
	for {
		if n != nil {
			u = append(u, m.Clone())
			m, n = n, nil
			fmt.Println(m)
		} else if err != nil {
			fmt.Println(err)
		}

		fmt.Printf("> ")
		if !s.Scan() {
			break
		}

		line := strings.TrimSpace(s.Text())
		toks := strings.Split(line, " ")

		switch toks[0] {
		case "c":
			h = h[:0]

		case "l":
			n, err = load(toks[1:])

		case "h":
			help()

		case "p":
			fmt.Println(m)

		case "r":
			err = replay(stdin, toks[1:])

		case "q":
			return

		case "s":
			err = save(toks[1:], m)

		case "u":
			if len(u) == 0 {
				fmt.Println("undo stack is empty")
			} else {
				fmt.Println("undoing previous operation")
				m = u[len(u)-1]
				u = u[:len(u)-1]
				fmt.Println(m)
			}

		case "y":
			err = history(toks[1:], h)

		case "+":
			n, err = add(toks[1:], m)

		case "*":
			n, err = mul(toks[1:], m)

		case "^":
			n, err = swap(toks[1:], m)

		case "&":
			var v Vector
			v, err = backsub(m)
			if err == nil {
				fmt.Println(v)
			}

		case "":

		default:
			fmt.Printf("unknown command %q\n", toks[0])
		}

		if line != "" {
			h = append(h, line)
		}
	}
}

type Stdin struct {
	lines []string
}

func (s *Stdin) Append(line string) {
	if !strings.HasSuffix(line, "\n") {
		line += "\n"
	}
	s.lines = append(s.lines, line)
}

func (s *Stdin) Read(p []byte) (int, error) {
	if len(s.lines) == 0 {
		return os.Stdin.Read(p)
	}

	l := s.lines[0]
	n := 0
	for n = range p {
		if len(l) == 0 {
			break
		}
		p[n], l = l[0], l[1:]
	}

	if len(l) != 0 {
		s.lines[0] = l
	} else {
		s.lines = s.lines[1:]
		fmt.Println()
	}

	return n, nil
}

func replay(stdin *Stdin, args []string) error {
	if len(args) != 1 {
		return fmt.Errorf("expected filename")
	}

	f, err := os.Open(args[0])
	if err != nil {
		return err
	}
	defer f.Close()

	s := bufio.NewScanner(f)
	for s.Scan() {
		stdin.Append(strings.TrimSpace(s.Text()))
	}

	return nil
}

func load(args []string) (Matrix, error) {
	if len(args) != 1 {
		return nil, fmt.Errorf("expected filename")
	}

	f, err := os.Open(args[0])
	if err != nil {
		return nil, err
	}
	defer f.Close()

	var m Matrix
	s := bufio.NewScanner(f)
	r := -1
	for s.Scan() {
		toks := strings.Split(strings.TrimSpace(s.Text()), " ")
		if r >= 0 && len(toks) != r {
			return nil, fmt.Errorf("row with size %v doesn't match previous row size: %v", len(toks), r)
		} else {
			r = len(toks)
		}

		var n Vector
		for i := range toks {
			x := new(big.Rat)
			y, _ := x.SetString(toks[i])
			if y == nil {
				return nil, fmt.Errorf("invalid row %v: %q", i, s.Text())
			}
			n = append(n, x)
		}
		m = append(m, n)
	}

	return m, nil
}

func save(args []string, m Matrix) error {
	if len(args) != 1 {
		return fmt.Errorf("expected filename")
	}

	f, err := os.Create(args[0])
	if err != nil {
		return err
	}

	b := new(bytes.Buffer)
	for i := range m {
		fmt.Fprintf(b, "%v\n", m[i])
	}

	_, err = f.Write(b.Bytes())
	if err != nil {
		return err
	}

	fmt.Printf("save successfully to %q\n", args[0])

	return nil
}

func help() {
	fmt.Println("usage:")
	fmt.Println("\tc          [clear command history]")
	fmt.Println("\tl filename [loads a matrix from a file]")
	fmt.Println("\th          [prints out the help]")
	fmt.Println("\tp          [prints out the current matrix]")
	fmt.Println("\tr filename [batch execute from a file]")
	fmt.Println("\tq          [quit the program]")
	fmt.Println("\ts          [save current matrix to file]")
	fmt.Println("\tu          [undo a matrix operation]")
	fmt.Println("\ty filename [save command history to file]")
	fmt.Println("\t+ x y      [add two rows together]")
	fmt.Println("\t* x c      [multiply a row by a constant]")
	fmt.Println("\t^ x y      [swap two rows]")
	fmt.Println("\t&          [perform backsubstitution, assumes an augmented matrix and in a relaxed rref]")
}

func add(args []string, m Matrix) (Matrix, error) {
	if len(args) != 2 {
		return nil, fmt.Errorf("expected two rows")
	}

	r, err := rows(m, args[0], args[1])
	if err != nil {
		return nil, err
	}

	n := m.Clone()
	x, y := r[0], r[1]
	for i := range n[y] {
		n[y][i].Add(n[y][i], n[x][i])
	}

	return n, nil
}

func mul(args []string, m Matrix) (Matrix, error) {
	if len(args) != 2 {
		return nil, fmt.Errorf("expected row and constant")
	}

	r, err := rows(m, args[0])
	if err != nil {
		return nil, err
	}

	n := m.Clone()
	x := r[0]
	c := new(big.Rat)
	c.SetString(args[1])
	for i := range n[x] {
		n[x][i].Mul(n[x][i], c)
	}

	return n, nil
}

func swap(args []string, m Matrix) (Matrix, error) {
	if len(args) != 2 {
		return nil, fmt.Errorf("expected two rows")
	}

	r, err := rows(m, args[0], args[1])
	if err != nil {
		return nil, err
	}

	n := m.Clone()
	x, y := r[0], r[1]
	for i := range n[x] {
		n[x][i], n[y][i] = n[y][i], n[x][i]
	}

	return n, nil
}

func history(args, hist []string) error {
	if len(args) != 1 {
		return fmt.Errorf("expected filename")
	}

	f, err := os.Create(args[0])
	if err != nil {
		return err
	}

	w := bufio.NewWriter(f)
	for _, h := range hist {
		fmt.Fprintln(w, h)
	}

	err = w.Flush()
	xerr := f.Close()
	if err != nil {
		return err
	}

	return xerr
}

type index struct {
	r int
	n int
}

type indexSlice []index

func (p indexSlice) Len() int           { return len(p) }
func (p indexSlice) Less(i, j int) bool { return p[i].n < p[j].n }
func (p indexSlice) Swap(i, j int)      { p[i], p[j] = p[j], p[i] }

func backsub(m Matrix) (Vector, error) {
	if len(m) == 0 || len(m[0]) < 2 {
		return Vector{}, nil
	}

	coeffs := make(Vector, len(m[0])-1)
	cnts := make([]int, len(m))
	for i := range cnts {
		cnts[i] = -1
	}

	zero := new(big.Rat)
	for i := range m {
		nz := 0
		l := len(m[i]) - 1
		for j := range m[i][:l] {
			if m[i][j].Cmp(zero) != 0 {
				nz++
			}
		}
		if l-nz == l {
			return nil, fmt.Errorf("contains a zero row")
		}
		cnts[i] = nz
	}

	z := make([]index, len(cnts))
	for i := range z {
		z[i].r = i
		z[i].n = cnts[i]
	}
	sort.Sort(indexSlice(z))

	for i := range z {
		r := m[z[i].r]
		l := len(r) - 1
		w := new(big.Rat).Set(r[l])

		var v *big.Rat
		for j := range r[:l] {
			if r[j].Cmp(zero) == 0 {
				continue
			}

			if coeffs[j] != nil {
				u := new(big.Rat).Set(coeffs[j])
				u.Mul(u, r[j])
				w.Sub(w, u)
			} else if v != nil {
				return nil, fmt.Errorf("matrix not in rref form")
			} else {
				v = new(big.Rat).Set(r[j])
			}
		}
		if v.Cmp(zero) == 0 {
			return nil, fmt.Errorf("matrix not in rref form")
		}
		coeffs[z[i].r] = w.Quo(w, v)
	}

	return coeffs, nil
}

func rows(m Matrix, str ...string) ([]int, error) {
	var r []int
	for _, s := range str {
		n, err := strconv.Atoi(s)
		if err != nil {
			return nil, err
		}

		if n--; !(0 <= n && n < len(m)) {
			return nil, fmt.Errorf("row %v out of matrix bounds of size %v", n, len(m))
		}

		r = append(r, n)
	}
	return r, nil
}

type Vector []*big.Rat
type Matrix []Vector

func (v Vector) String() string {
	s := "["
	for _, x := range v {
		v := x.String()
		if x.IsInt() {
			v = x.Num().String()
		}
		s += v + " "
	}
	if len(s) != 1 {
		s = s[:len(s)-1]
	}
	s += "]"
	return s
}

func (v Vector) Clone() Vector {
	var p Vector
	for i := range v {
		x := new(big.Rat).Set(v[i])
		p = append(p, x)
	}
	return p
}

func (m Matrix) String() string {
	r := len(m)
	c := 0
	if r > 0 {
		c = len(m[0])
	}

	s := fmt.Sprintf("\n%vx%v matrix\n", r, c)
	for i := range m {
		s += m[i].String() + "\n"
	}
	return s
}

func (m Matrix) Clone() Matrix {
	var n Matrix
	for i := range m {
		n = append(n, m[i].Clone())
	}
	return n
}
