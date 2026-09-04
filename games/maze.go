// generates a maze based on the dfs algorithm
package main

import (
	"flag"
	"fmt"
	"math/rand"
	"os"
	"strconv"
	"time"
)

func main() {
	flag.Parse()
	if flag.NArg() != 2 {
		usage()
	}

	w, h := atoi(flag.Arg(0)), atoi(flag.Arg(1))
	rand.Seed(time.Now().UnixNano())
	g := gendfs(w, h)
	output(g, w, h)
}

func atoi(s string) int {
	v, err := strconv.Atoi(s)
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
	return v
}

func usage() {
	fmt.Fprintf(os.Stderr, "usage: w h\n")
	os.Exit(1)
}

func output(g [][]uint8, w, h int) {
	fmt.Print(" ")
	for i := 0; i < 2*w-1; i++ {
		fmt.Print("_")
	}
	fmt.Print("\n")

	for y := 0; y < h; y++ {
		fmt.Print("|")
		for x := 0; x < w; x++ {
			if g[y][x]&S != 0 {
				fmt.Print(" ")
			} else {
				fmt.Print("_")
			}

			if g[y][x]&E != 0 {
				if (g[y][x]|g[y][x+1])&S != 0 {
					fmt.Print(" ")
				} else {
					fmt.Print("_")
				}
			} else {
				fmt.Print("|")
			}
		}
		fmt.Print("\n")
	}
}

const (
	N = 1 << iota
	S
	E
	W
)

var (
	DX       = []int{E: 1, W: -1, N: 0, S: 0}
	DY       = []int{E: 0, W: 0, N: -1, S: 1}
	OPPOSITE = []uint8{E: W, W: E, N: S, S: N}
)

func gendfs(w, h int) [][]uint8 {
	p := make([]uint8, w*h)
	g := make([][]uint8, h)
	for i := range g {
		g[i] = p[i*w : (i+1)*w]
	}
	dfs(g, 0, 0)
	return g
}

func dfs(g [][]uint8, cx, cy int) {
	dir := [4]uint8{N, S, E, W}

	p := rand.Perm(4)
	for i := range p {
		dir[i], dir[p[i]] = dir[p[i]], dir[i]
	}

	for _, d := range dir {
		nx, ny := cx+DX[d], cy+DY[d]
		if 0 <= ny && ny < len(g) && 0 <= nx && nx < len(g[ny]) && g[ny][nx] == 0 {
			g[cy][cx] |= d
			g[ny][nx] |= OPPOSITE[d]
			dfs(g, nx, ny)
		}
	}
}
