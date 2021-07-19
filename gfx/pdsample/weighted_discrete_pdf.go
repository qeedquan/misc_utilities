package main

type WDPDFNode struct {
	mark                bool
	Parent, Left, Right *WDPDFNode
	Key                 int
	Weight, SumWeights  float64
}

func NewWDPDFNode(key int, weight float64, parent *WDPDFNode) *WDPDFNode {
	return &WDPDFNode{
		Key:    key,
		Weight: weight,
		Parent: parent,
	}
}

func (n *WDPDFNode) LeftIsBlack() bool  { return n.Left == nil || n.Left.IsBlack() }
func (n *WDPDFNode) RightIsBlack() bool { return n.Right == nil || n.Right.IsBlack() }

func (n *WDPDFNode) LeftIsRed() bool  { return !n.LeftIsBlack() }
func (n *WDPDFNode) RightIsRed() bool { return !n.RightIsBlack() }

func (n *WDPDFNode) IsRed() bool   { return n.mark }
func (n *WDPDFNode) IsBlack() bool { return !n.mark }

func (n *WDPDFNode) MarkRed()   { n.mark = true }
func (n *WDPDFNode) MarkBlack() { n.mark = false }

func (n *WDPDFNode) Sibling() *WDPDFNode {
	if n == n.Parent.Left {
		return n.Parent.Right
	}
	return n.Parent.Left
}

func (n *WDPDFNode) SetSum() {
	n.SumWeights = n.Weight
	if n.Left != nil {
		n.SumWeights += n.Left.SumWeights
	}
	if n.Right != nil {
		n.SumWeights += n.Right.SumWeights
	}
}

type WDPDF struct {
	root *WDPDFNode
}

func NewWDPDF() *WDPDF {
	return &WDPDF{}
}

func (wd *WDPDF) Walk(typ int, f func(n *WDPDFNode)) {
	wd.walk(typ, wd.root, f)
}

func (wd *WDPDF) walk(typ int, n *WDPDFNode, f func(*WDPDFNode)) {
	if n == nil {
		return
	}
	switch typ {
	case 0:
		f(n)
		wd.walk(typ, n.Left, f)
		wd.walk(typ, n.Right, f)

	case 1:
		wd.walk(typ, n.Left, f)
		f(n)
		wd.walk(typ, n.Right, f)

	case 2:
		wd.walk(typ, n.Left, f)
		wd.walk(typ, n.Right, f)
		f(n)
	}
}

func (wd *WDPDF) Insert(item int, weight float64) {
	var p *WDPDFNode
	n := wd.root
	for n != nil {
		if n.LeftIsRed() && n.RightIsRed() {
			wd.split(n)
		}

		p = n
		if n.Key == item {
			panic("insert: argument(item) already in tree")
		} else {
			if item < n.Key {
				n = n.Left
			} else {
				n = n.Right
			}
		}
	}

	n = NewWDPDFNode(item, weight, p)

	if p == nil {
		wd.root = n
	} else {
		if item < p.Key {
			p.Left = n
		} else {
			p.Right = n
		}
		wd.split(n)
	}
	wd.propagateSumsUp(n)
}

func (wd *WDPDF) Update(item int, weight float64) {
	np, _ := wd.Lookup(item)
	n := *np
	if n == nil {
		panic("update: argument(item) not in tree")
	}

	delta := weight - n.Weight
	n.Weight = weight
	for ; n != nil; n = n.Parent {
		n.SumWeights += delta
	}
}

func (wd *WDPDF) Lookup(item int) (out **WDPDFNode, parent_out *WDPDFNode) {
	var p *WDPDFNode
	np := &wd.root
	for {
		n := *np
		if n == nil {
			break
		}
		if n.Key == item {
			break
		}
		p = n
		if item < n.Key {
			np = &n.Left
		} else {
			np = &n.Right
		}
	}

	return np, p
}

func (wd *WDPDF) Remove(item int) {
	np, _ := wd.Lookup(item)
	n := *np
	if n == nil {
		panic("argument(item) not in tree")
	}

	if n.Left != nil {
		leftMaxp := &n.Left
		for (*leftMaxp).Right != nil {
			leftMaxp = &(*leftMaxp).Right
		}
		n.Key = (*leftMaxp).Key
		n.Weight = (*leftMaxp).Weight

		np = leftMaxp
		n = *np
	}

	// node now has at most one child
	var child *WDPDFNode
	if n.Left != nil {
		child = n.Left
	} else {
		child = n.Right
	}
	*np = child

	if child != nil {
		child.Parent = n.Parent

		if n.IsBlack() {
			wd.lengthen(child)
		}
	}

	wd.propagateSumsUp(n.Parent)
	n.Left, n.Right = nil, nil
}

func (wd *WDPDF) Choose(p float64) int {
	if p < 0 || p >= 1 {
		panic("argument(p) outside of valid range")
	}
	if wd.root == nil {
		panic("choose() called on empty tree")
	}

	w := wd.root.SumWeights * p
	n := wd.root

	for {
		if n.Left != nil {
			if w < n.Left.SumWeights {
				n = n.Left
				continue
			} else {
				w -= n.Left.SumWeights
			}
		}

		// shouldn't be necessary, sanity check
		if w < n.Weight || n.Right == nil {
			break
		}
		w -= n.Weight
		n = n.Right
	}

	return 0
}

func (wd *WDPDF) split(n *WDPDFNode) {
	if n.Left != nil {
		n.Left.MarkBlack()
	}
	if n.Right != nil {
		n.Right.MarkBlack()
	}

	if n.Parent != nil {
		p := n.Parent

		n.MarkRed()

		if p.IsRed() {
			p.Parent.MarkRed()

			// not same direction
			if !((n == p.Left && p == p.Parent.Left) || (n == p.Right && p == p.Parent.Right)) {
				wd.rotate(n)
				p = n
			}

			wd.rotate(p)
			p.MarkBlack()
		}
	}
}

func (wd *WDPDF) rotate(n *WDPDFNode) {
	p := n.Parent
	pp := p.Parent

	n.Parent = pp
	p.Parent = n

	if n == p.Left {
		p.Left = n.Right
		n.Right = p
		if p.Left != nil {
			p.Left.Parent = p
		}
	} else {
		p.Right = n.Left
		n.Left = p
		if p.Right != nil {
			p.Right.Parent = p
		}
	}

	n.SetSum()
	p.SetSum()

	if pp == nil {
		wd.root = n
	} else {
		if p == pp.Left {
			pp.Left = n
		} else {
			pp.Right = n
		}
	}
}

func (wd *WDPDF) lengthen(n *WDPDFNode) {
	if n.IsRed() {
		n.MarkBlack()
	} else if n.Parent != nil {
		sibling := n.Sibling()

		if sibling != nil && sibling.IsRed() {
			n.Parent.MarkRed()
			sibling.MarkBlack()

			// node sibling is now old sibling child, must be black
			wd.rotate(sibling)
			sibling = n.Sibling()
		}

		// sibling is black
		if sibling == nil {
			wd.lengthen(n.Parent)
		} else if sibling.LeftIsBlack() && sibling.RightIsBlack() {
			if n.Parent.IsBlack() {
				sibling.MarkRed()
				wd.lengthen(n.Parent)
			} else {
				sibling.MarkRed()
				n.Parent.MarkBlack()
			}
		} else {
			if n == n.Parent.Left && sibling.RightIsBlack() {
				// sibling->left must be red
				wd.rotate(sibling.Left)
				sibling.Parent.MarkBlack()
				sibling = sibling.Parent
			} else if n == n.Parent.Right && sibling.LeftIsBlack() {
				wd.rotate(sibling.Right)
				sibling.MarkRed()
				sibling.Parent.MarkBlack()
				sibling = sibling.Parent
			}

			// sibling is black, and sibling's far child is red
			wd.rotate(sibling)
			if n.Parent.IsRed() {
				sibling.MarkRed()
			}
			sibling.Left.MarkBlack()
			sibling.Right.MarkBlack()
		}
	}
}

func (wd *WDPDF) propagateSumsUp(n *WDPDFNode) {
	for n != nil {
		n.SetSum()
		n = n.Parent
	}
}
