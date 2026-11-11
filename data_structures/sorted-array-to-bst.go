// sorted array to a bst
// algorithm is basically mergesort but instead of sorting
// add keep splitting the list left and right at the middle point
// and use the middle point as the new root
package main

import (
	"fmt"
	"math/rand"
	"sort"
	"time"
)

func main() {
	rand.Seed(time.Now().UnixNano())
	preorder(build([]int{1, 2, 3, 4, 5, 6, 7}))
	preorder(build(rsarr(1e6)))
}

func rsarr(n int) []int {
	p := make([]int, n)
	for i := range p {
		p[i] = rand.Int()
	}
	sort.Ints(p)
	return p
}

type Node struct {
	data  int
	left  *Node
	right *Node
}

func build(a []int) *Node {
	return buildrec(a, 0, len(a)-1)
}

func buildrec(a []int, lo, hi int) *Node {
	if lo > hi {
		return nil
	}
	mid := lo + (hi-lo)/2
	return &Node{
		data:  a[mid],
		left:  buildrec(a, lo, mid-1),
		right: buildrec(a, mid+1, hi),
	}
}

func preorder(n *Node) {
	if n == nil {
		return
	}
	fmt.Println(n.data)
	preorder(n.left)
	preorder(n.right)
}
