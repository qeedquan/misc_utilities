// ported from https://github.com/mevdschee/2048.c
package main

import (
	"fmt"
	"math/rand"
	"os"
	"os/signal"
	"time"

	"github.com/qeedquan/go-media/posix"
	"golang.org/x/sys/unix"
)

const (
	SIZE = 4
)

var (
	score  uint64
	scheme int
)

func main() {
	rand.Seed(time.Now().UnixNano())
	board := newBoard()
	if len(os.Args) == 2 {
		switch os.Args[1] {
		case "blackwhite":
			scheme = 1
		case "bluered":
			scheme = 2
		}
	}

	fmt.Printf("\033[?25l\033[2J\033[H")
	go sigHandler()

	addRandom(board)
	addRandom(board)
	drawBoard(board)
	setBufferedInput(false)

	for {
		ch := getChar()
		success := false
		switch ch {
		case 97, 104, 68: // 'a', 'h', left
			success = moveLeft(board)
		case 100, 108, 67: // 'd', 'l', right
			success = moveRight(board)
		case 119, 107, 65: /// 'w', 'k', up
			success = moveUp(board)
		case 115, 106, 66: // 's', 'j', down
			success = moveDown(board)
		}

		if success {
			drawBoard(board)
			time.Sleep(150000 * time.Microsecond)
			addRandom(board)
			drawBoard(board)
			if gameEnded(board) {
				fmt.Printf("         GAME OVER          \n")
				break
			}
		}

		if ch == 'q' {
			fmt.Printf("        QUIT? (y/n)         \n")
			for {
				ch = getChar()
				if ch == 'y' {
					setBufferedInput(true)
					fmt.Printf("\033[?25h")
					os.Exit(0)
				} else {
					drawBoard(board)
					break
				}
			}
		}

		if ch == 'r' {
			fmt.Printf("       RESTART? (y/n)       \n")
			for {
				ch = getChar()
				if ch == 'y' {
					zeroBoard(board)
					addRandom(board)
					addRandom(board)
					drawBoard(board)
					break
				} else {
					drawBoard(board)
					break
				}
			}
		}
	}

	setBufferedInput(true)
	fmt.Printf("\033[?25h")
}

func getChar() byte {
	var b [1]byte
	os.Stdin.Read(b[:])
	return b[0]
}

func getColor(value int) string {
	original := []int{8, 255, 1, 255, 2, 255, 3, 255, 4, 255, 5, 255, 6, 255, 7, 255, 9, 0, 10, 0, 11, 0, 12, 0, 13, 0, 14, 0, 255, 0, 255, 0}
	blackWhite := []int{232, 255, 234, 255, 236, 255, 238, 255, 240, 255, 242, 255, 244, 255, 246, 0, 248, 0, 249, 0, 250, 0, 251, 0, 252, 0, 253, 0, 254, 0, 255, 0}
	blueRed := []int{235, 255, 63, 255, 57, 255, 93, 255, 129, 255, 165, 255, 201, 255, 200, 255, 199, 255, 198, 255, 197, 255, 196, 255, 196, 255, 196, 255, 196, 255, 196, 255}
	schemes := [][]int{original, blackWhite, blueRed}

	palette := schemes[scheme]
	bg := 0
	fg := 1

	if value > 0 {
		for {
			value >>= 1
			if value == 0 {
				break
			}
			if bg+2 < len(palette) {
				bg += 2
				fg += 2
			}
		}
	}

	return fmt.Sprintf("\033[38;5;%d;48;5;%dm", palette[fg], palette[bg])
}

func newBoard() [][]int {
	b := make([][]int, SIZE)
	for i := range b {
		b[i] = make([]int, SIZE)
	}
	return b
}

func zeroBoard(board [][]int) {
	for i := range board {
		for j := range board[i] {
			board[i][j] = 0
		}
	}
}

func addRandom(board [][]int) {
	var list [SIZE * SIZE][2]int

	l := 0
	for x := 0; x < SIZE; x++ {
		for y := 0; y < SIZE; y++ {
			if board[x][y] == 0 {
				list[l][0] = x
				list[l][1] = y
				l++
			}
		}
	}

	if l > 0 {
		r := rand.Intn(l)
		x := list[r][0]
		y := list[r][1]
		n := (rand.Intn(10)/9 + 1) * 2
		board[x][y] = n
	}
}

func drawBoard(board [][]int) {
	fmt.Printf("\033[H")
	fmt.Printf("2048.go %17d pts\n\n", score)
	reset := "\033[m"

	for y := 0; y < SIZE; y++ {
		for x := 0; x < SIZE; x++ {
			color := getColor(board[x][y])
			fmt.Printf("%s", color)
			fmt.Printf("       ")
			fmt.Printf("%s", reset)
		}

		fmt.Printf("\n")

		for x := 0; x < SIZE; x++ {
			color := getColor(board[x][y])
			fmt.Printf("%s", color)
			if board[x][y] != 0 {
				s := fmt.Sprint(board[x][y])
				t := 7 - len(s)
				fmt.Printf("%*s%s%*s", t-t/2, "", s, t/2, "")
			} else {
				fmt.Printf("   ·   ")
			}
			fmt.Printf("%s", reset)
		}

		fmt.Printf("\n")
		for x := 0; x < SIZE; x++ {
			color := getColor(board[x][y])
			fmt.Printf("%s", color)
			fmt.Printf("       ")
			fmt.Printf("%s", reset)
		}

		fmt.Printf("\n")
	}

	fmt.Printf("\n")
	fmt.Printf("        ←,↑,→,↓ or q        \n")
	fmt.Printf("\033[A")
}

func rotateBoard(board [][]int) {
	n := SIZE
	for i := 0; i < n/2; i++ {
		for j := i; j < n-i-1; j++ {
			tmp := board[i][j]
			board[i][j] = board[j][n-i-1]
			board[j][n-i-1] = board[n-i-1][n-j-1]
			board[n-i-1][n-j-1] = board[n-j-1][i]
			board[n-j-1][i] = tmp
		}
	}
}

func moveUp(board [][]int) bool {
	success := false
	for x := 0; x < SIZE; x++ {
		if slideArray(board[x]) {
			success = true
		}
	}
	return success
}

func moveLeft(board [][]int) bool {
	rotateBoard(board)
	success := moveUp(board)
	rotateBoard(board)
	rotateBoard(board)
	rotateBoard(board)
	return success
}

func moveDown(board [][]int) bool {
	rotateBoard(board)
	rotateBoard(board)
	success := moveUp(board)
	rotateBoard(board)
	rotateBoard(board)
	return success
}

func moveRight(board [][]int) bool {
	rotateBoard(board)
	rotateBoard(board)
	rotateBoard(board)
	success := moveUp(board)
	rotateBoard(board)
	return success
}

func findPairDown(board [][]int) bool {
	success := false
	for x := 0; x < SIZE; x++ {
		for y := 0; y < SIZE-1; y++ {
			if board[x][y] == board[x][y+1] {
				return true
			}
		}
	}
	return success
}

func countEmpty(board [][]int) int {
	count := 0
	for x := 0; x < SIZE; x++ {
		for y := 0; y < SIZE; y++ {
			if board[x][y] == 0 {
				count++
			}
		}
	}
	return count
}

func gameEnded(board [][]int) bool {
	ended := true
	if countEmpty(board) > 0 {
		return false
	}
	if findPairDown(board) {
		return false
	}
	rotateBoard(board)
	if findPairDown(board) {
		ended = false
	}
	rotateBoard(board)
	rotateBoard(board)
	rotateBoard(board)
	return ended
}

func findTarget(array []int, x, stop int) int {
	if x == 0 {
		return x
	}
	for t := x - 1; t >= 0; t-- {
		if array[t] != 0 {
			if array[t] != array[x] {
				return t + 1
			}
			return t
		} else {
			if t == stop {
				return t
			}
		}
	}
	return x
}

func slideArray(array []int) bool {
	success := false

	stop := 0
	for x := 0; x < SIZE; x++ {
		if array[x] != 0 {
			t := findTarget(array, x, stop)
			if t != x {
				if array[t] != 0 {
					score += uint64(array[t]) + uint64(array[x])
					stop = t + 1
				}
				array[t] += array[x]
				array[x] = 0
				success = true
			}
		}
	}
	return success
}

func sigHandler() {
	ch := make(chan os.Signal, 1)
	signal.Notify(ch, os.Interrupt)
	<-ch
	fmt.Printf("         TERMINATED         \n")
	setBufferedInput(true)
	fmt.Printf("\033[?25h")
	os.Exit(0)
}

var (
	enabled = true
	old     unix.Termios
)

func setBufferedInput(enable bool) {
	var new_ unix.Termios
	if enable && !enabled {
		posix.Tcsetattr(posix.STDIN_FILENO, posix.TCSANOW, &old)
		enabled = true
	} else if !enable && enabled {
		posix.Tcgetattr(posix.STDIN_FILENO, &new_)
		old = new_
		new_.Lflag &= (^uint32(unix.ICANON) & ^uint32(unix.ECHO))
		posix.Tcsetattr(posix.STDIN_FILENO, posix.TCSANOW, &new_)
		enabled = false
	}
}
