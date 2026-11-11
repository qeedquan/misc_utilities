package main

import (
	"fmt"
	"io"
	"math/rand"
	"strings"
	"time"
)

func main() {
	var p Puzzle
	p.Play()
}

type Puzzle struct {
	grid [4][4]int
	x, y int
}

func (p *Puzzle) Play() {
	var input string

	rand.Seed(time.Now().UnixNano())
	p.reset()
loop:
	for {
		p.drawGrid()
		fmt.Printf("\nenter a move: ")
		_, err := fmt.Scanln(&input)
		fmt.Println("")

		if err == io.EOF {
			break
		}
		input = strings.TrimSpace(input)
		input = strings.ToLower(input)
		switch input {
		case "n":
			p.randomize()
		case "w", "a", "s", "d":
			if !p.move(p.conv(input[0])) {
				fmt.Printf("cannot move there!\n\n")
				continue
			}
		case "h":
			fmt.Printf("wasd: move up/left/down/right\n")
			fmt.Printf("n:    start a new game\n")
			fmt.Printf("q:    quit\n")
			fmt.Printf("\n\n")
		case "":
		case "q":
			fmt.Printf("quitting\n\n")
			break loop
		default:
			fmt.Printf("unknown move, enter again!\n\n")
			continue
		}

		if p.checkSolution() {
			fmt.Printf("you win!\n\n")
			break
		}
	}
}

func (*Puzzle) conv(c byte) byte {
	switch c {
	case 'w':
		return 'u'
	case 'a':
		return 'l'
	case 's':
		return 'd'
	case 'd':
		return 'r'
	}
	return 0
}

func (p *Puzzle) randomize() {
	const r = "lrdu"
	const rr = "rlud"
	for i := 0; i < 1000; i++ {
		n := rand.Intn(len(r))
		if !p.move(r[n]) {
			p.move(rr[n])
		}
	}
}

func (p *Puzzle) move(dir uint8) bool {
	g := p.grid[:][:]
	x, y := p.x, p.y
	switch dir {
	case 'l':
		if y-1 < 0 {
			return false
		}
		g[x][y], g[x][y-1] = g[x][y-1], g[x][y]
		p.y--
	case 'r':
		if y+1 > 3 {
			return false
		}
		g[x][y], g[x][y+1] = g[x][y+1], g[x][y]
		p.y++
	case 'd':
		if x+1 > 3 {
			return false
		}
		g[x][y], g[x+1][y] = g[x+1][y], g[x][y]
		p.x++
	case 'u':
		if x-1 < 0 {
			return false
		}
		g[x][y], g[x-1][y] = g[x-1][y], g[x][y]
		p.x--
	}
	return true
}

func (p *Puzzle) reset() {
	n := 1
	for i := range p.grid {
		for j := range p.grid[i] {
			p.grid[i][j], n = n, n+1
		}
	}
	p.x, p.y = 3, 3
	p.grid[p.x][p.y] = -1
	p.randomize()
}

func (p *Puzzle) drawGrid() {
	for i := range p.grid {
		for j := range p.grid {
			if p.grid[i][j] < 0 {
				fmt.Printf("%2c ", '.')
			} else {
				fmt.Printf("%2v ", p.grid[i][j])
			}
		}
		fmt.Println("")
	}
}

func (p *Puzzle) checkSolution() bool {
	n := 1
loop:
	for i := range p.grid {
		for j := range p.grid[i] {
			if p.grid[i][j] != n {
				if i == 3 && j == 3 && p.grid[i][j] == -1 {
					break loop
				}
				return false
			}
			n++
		}
	}
	return true
}
