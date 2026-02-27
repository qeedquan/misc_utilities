package unionfind

const size = 512

type Set struct {
	parent []int
	rank   []int
}

func max(a, b int) int {
	if a > b {
		return a
	}
	return b
}

func grow(b []int, size int) []int {
	if size <= len(b) {
		return b
	}
	a := make([]int, size)
	copy(a, b)
	return a
}

func New() *Set {
	u := &Set{
		parent: make([]int, size),
		rank:   make([]int, size),
	}
	u.Reset()
	return u
}

func (u *Set) Reset() {
	for i := range u.parent {
		u.parent[i] = i
	}
	for i := range u.rank {
		u.rank[i] = 0
	}
}

func (u *Set) Union(x, y int) *Set {
	xroot := u.Find(x)
	yroot := u.Find(y)

	u.rank = grow(u.rank, max(xroot, yroot)*2)

	if u.rank[xroot] > u.rank[yroot] {
		u.parent[yroot] = xroot
	} else if u.rank[xroot] < u.rank[yroot] {
		u.parent[xroot] = yroot
	} else if xroot != yroot {
		if xroot <= yroot {
			u.parent[yroot] = xroot
			u.rank[xroot]++
		} else {
			u.parent[xroot] = yroot
			u.rank[yroot]++
		}
	}

	return u
}

func (u *Set) Find(n int) int {
	u.parent = grow(u.parent, n*2)

	p := u.parent[n]
	for i := n; u.parent[i] != i; {
		p, i = u.parent[i], u.parent[i]
	}

	for i := n; u.parent[i] != i; {
		u.parent[i], i = p, u.parent[i]
	}

	return p
}
