/*

https://en.wikipedia.org/wiki/Levi-Civita_symbol
https://www.tau.ac.il/~tsirel/dump/Static/knowino.org/wiki/Levi-Civita_symbol.html
http://homepages.engineering.auckland.ac.nz/~pkel015/SolidMechanicsBooks/Part_III/Chapter_1_Vectors_Tensors/Vectors_Tensors_Complete.pdf
https://people.maths.bris.ac.uk/~maajh/afluids/VectorCalculus.pdf
https://en.wikipedia.org/wiki/Kronecker_delta

*/

package main

import "fmt"

func main() {
	t_parity()
	t_kronecker()
	t_identities()
}

func assert(x bool) {
	if !x {
		panic("assertion failed")
	}
}

/*

e_ijk*e_ipq = k_jp*k_kq - k_kp*k_jq
e_ijk*e_ijp = k_pk

ijkpq are any combination of indices with value [1, 2, 3]
ijk have to be independent of each other

*/

func t_identities() {
	fmt.Println("Testing identities")
	for i := 1; i <= 3; i++ {
		for j := 1; j <= 3; j++ {
			for k := 1; k <= 3; k++ {
				if i == j || i == k || j == k {
					continue
				}

				a := []int{i, j, k}
				for p := 1; p <= 3; p++ {
					for q := 1; q <= 3; q++ {
						b := []int{i, p, q}
						x := parity(a) * parity(b)
						y := kronecker(j, p)*kronecker(k, q) - kronecker(k, p)*kronecker(j, q)
						fmt.Println(i, j, k, k, q, "|", x, y)
						assert(x == y)

						b = []int{i, j, p}
						x = parity(a) * parity(b)
						y = kronecker(p, k)
						fmt.Println(i, j, k, p, "|", x, y)
						assert(x == y)
					}
				}
			}
		}
	}
}

func t_parity() {
	tab := [][]int{
		{2, 3, 3},
		{1, 1, 2},
		{3, 2, 3},
		{1, 1, 3},
		{1, 2, 2},
		{1, 3, 3},
		{1, 2, 3},
		{2, 3, 1},
		{3, 1, 2},
		{2, 1, 3},
		{3, 2, 1},
		{1, 3, 2},
	}

	fmt.Println("Testing Parity")
	for i := range tab {
		fmt.Println(tab[i], parity(tab[i]))
	}
	fmt.Println()
}

func t_kronecker() {
	fmt.Println("Testing Kronecker")
	for i := 0; i < 3; i++ {
		for j := 0; j < 3; j++ {
			fmt.Println(i, j, "|", kronecker(i, j))
		}
	}
	fmt.Println()
}

/*

Also known as the permutation, alternating, or Levi-Civita symbol.
We can calculate the parity by seeing how many swaps we need to get to the standard permutation

Example:
(3 2 1) -> (1 2 3) would take 1 swap (odd number so parity is -1)
(2 3 1) -> (1 2 3) would take 2 swap (even number so parity is +1)
(2 2 1) -> (1 2 3) is impossible     (parity is 0)

Common variable notion to permutation index:
i = x = 1
j = y = 2
k = z = 3

Example:
ijk = xyz = (1 2 3)

*/

func parity(a []int) int {
	b := append([]int{}, a...)
	p := 1
loop:
	for i := 0; i < len(b); i++ {
		for j := i; j < len(b); j++ {
			if b[j]-1 == i && i != j {
				b[i], b[j], p = b[j], b[i], -p
				if b[i] == b[j] {
					return 0
				}

				continue loop
			}
		}
		if b[i]-1 != i {
			return 0
		}
	}

	return p
}

/*

A kronecker symbol is also commonly used as a index symbol
o_ij = 1 if i == j
o_ij = 0 if i != j

Represents the diagonal indices in the matrix

*/

func kronecker(i, j int) int {
	if i == j {
		return 1
	}
	return 0
}
