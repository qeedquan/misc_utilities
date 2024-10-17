package main

import (
	"fmt"
	"image/color"
	"log"
	"math/rand"
	"runtime"
	"sync"
	"time"

	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlimage/sdlcolor"
)

const cellsize = 512
const rule = "1234/3"

type Life struct {
	Cells [][]int
	Sum   [][]int
	Color [9]color.RGBA
	Alive [9]bool
	Born  [9]bool
	wg    *sync.WaitGroup
}

type Display struct {
	*sdl.Window
	*sdl.Renderer
}

func NewDisplay(w, h int, flags sdl.WindowFlags) *Display {
	window, renderer, err := sdl.CreateWindowAndRenderer(w, h, flags)
	ck(err)
	return &Display{window, renderer}
}

func main() {
	runtime.LockOSThread()
	err := sdl.Init(sdl.INIT_EVERYTHING)
	ck(err)
	defer sdl.Quit()

	screen := NewDisplay(cellsize, cellsize, sdl.WINDOW_RESIZABLE)
	screen.SetTitle("Life")
	screen.SetLogicalSize(cellsize, cellsize)

	texture, err := screen.CreateTexture(sdl.PIXELFORMAT_ARGB8888, sdl.TEXTUREACCESS_STREAMING, cellsize, cellsize)
	ck(err)
	pixels := make([]uint32, cellsize*cellsize)

	life := NewLife(cellsize)
	rand.Seed(time.Now().UnixNano())
	rule := RandomRule()
	fmt.Println(rule)
	life.SetRule(rule)
loop:
	for {
		for {
			ev := sdl.PollEvent()
			if ev == nil {
				break
			}
			switch e := ev.(type) {
			case sdl.QuitEvent:
				break loop
			case sdl.KeyDownEvent:
				if e.Sym == sdl.K_ESCAPE {
					break loop
				} else if e.Sym == sdl.K_SPACE {
					rule := RandomRule()
					fmt.Println(rule)
					life.SetRule(rule)
				}
			}
		}

		render(pixels, life)
		texture.Update(nil, pixels, cellsize*4)
		screen.SetDrawColor(sdlcolor.Black)
		screen.Clear()
		screen.Copy(texture, nil, nil)
		screen.Present()
		life.Run()
	}
}

var colors = [9]color.RGBA{
	{23, 48, 34, 255},
	{0, 255, 0, 255},
	{0, 0, 255, 255},
	{111, 111, 111, 255},
	{200, 30, 100, 255},
	{150, 150, 0, 255},
	{193, 129, 193, 255},
	{19, 38, 200, 255},
	{190, 100, 200, 255},
}

func NewLife(size int) *Life {
	l := &Life{}
	l.Color = colors
	l.wg = new(sync.WaitGroup)

	cells := make([]int, size*size)
	sum := make([]int, size*size)
	l.Cells = make([][]int, size)
	l.Sum = make([][]int, size)
	for i := range l.Cells {
		l.Cells[i] = cells[size*i:]
		l.Sum[i] = sum[size*i:]
	}

	for i := range cells {
		n := rand.Intn(1000)
		if n < 500 {
			cells[i] = 1
		}
	}

	return l
}

func RandomRule() string {
	var rule string

	for i := 0; ; i++ {
		lower, upper := rand.Intn(9), rand.Intn(9)
		if lower > upper {
			lower, upper = upper, lower
		}

		for i := lower; i <= upper; i++ {
			rule += string('0' + i)
		}

		if i == 1 {
			break
		}

		rule += "/"
	}

	return rule
}

func (l *Life) SetRule(rule string) {
	alive := l.Alive[:]
	born := l.Born[:]

	for i := range alive {
		alive[i] = false
		born[i] = false
	}

	var p int
	for i, c := range rule {
		if c == '/' {
			p = i + 1
			break
		}
		if c < '0' || c > '8' {
			panic(fmt.Sprintf("invalid rule: %c", c))
		}

		alive[int(c-'0')] = true
	}

	for _, c := range rule[p:] {
		if c < '0' || c > '8' {
			panic(fmt.Sprintf("invalid rule: %c", c))
		}

		born[int(c-'0')] = true
	}
}

func (l *Life) Run() {
	size := len(l.Cells)
	l.wg.Add(size)
	for y := 0; y < size; y++ {
		go func(y int) {
			for x := 0; x < size; x++ {
				l.Sum[y][x] = l.sum(x, y)
			}
			l.wg.Done()
		}(y)
	}
	l.wg.Wait()

	l.wg.Add(size)
	for y := 0; y < size; y++ {
		go func(y int) {
			for x := 0; x < size; x++ {
				l.evolve(x, y)
			}
			l.wg.Done()
		}(y)
	}
	l.wg.Wait()
}

func (l *Life) sum(x, y int) int {
	var dir = [...][2]int{
		{-1, -1},
		{+0, -1},
		{+1, -1},
		{-1, +0},
		{+1, +0},
		{-1, +1},
		{+0, +1},
		{+1, +1},
	}

	sum := 0
	for _, p := range dir {
		sum += l.neighbor(x, y, p[0], p[1])
	}

	return sum
}

func (l *Life) evolve(x, y int) {
	sum := l.Sum[y][x]
	c := &l.Cells[y][x]

	if !l.Alive[sum] {
		*c = 0
	}

	if l.Born[sum] {
		*c = 1
	}
}

func (l *Life) neighbor(x, y, dx, dy int) int {
	size := len(l.Cells)
	px, py := x+dx, y+dy
	if px < 0 || py < 0 || px >= size || py >= size {
		return 0
	}

	return l.Cells[py][px]
}

func render(pix []uint32, l *Life) {
	var pixel uint32

	size := len(l.Cells)
	for y := 0; y < size; y++ {
		for x := 0; x < size; x++ {
			c := l.Cells[y][x]
			switch c {
			case 0: // dead
				pixel = 0xFF000000
			case 1: // alive
				cc := l.Sum[y][x]
				pixel = c32(colors[cc])
			default:
				panic("invalid state")
			}

			px, py := x, y
			pix[py*cellsize+px] = pixel
		}
	}
}

func c32(c color.RGBA) uint32 {
	return uint32(c.A)<<24 | uint32(c.R)<<16 | uint32(c.G)<<8 | uint32(c.B)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}
