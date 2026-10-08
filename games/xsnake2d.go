// ported from https://sourceforge.net/projects/xsnake-2d/
package main

import (
	"flag"
	"fmt"
	"log"
	"math/rand"
	"os"
	"runtime"
	"time"

	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlimage/sdlcolor"
)

const (
	N = 30
	M = 20
	S = 25

	W = N * S
	H = M * S
)

var (
	fullscreen = flag.Bool("fullscreen", false, "fullscreen")

	screen   *Display
	mx, mymx int
	fruit    sdl.Point
	s        [100]sdl.Point
	step     = uint32(300)
	dir      = 1
	sh       = 5
)

type Display struct {
	*sdl.Window
	*sdl.Renderer
}

func newDisplay(w, h int, flags sdl.WindowFlags) *Display {
	window, renderer, err := sdl.CreateWindowAndRenderer(w, h, flags)
	ck(err)
	return &Display{window, renderer}
}

func initGame() {
	for i := 0; i < sh; i++ {
		s[i] = sdl.Point{
			N / 2,
			int32((M+sh)/2 - i),
		}
	}
	genFruit()
}

func main() {
	runtime.LockOSThread()
	log.SetFlags(0)
	rand.Seed(time.Now().UnixNano())
	flag.Parse()
	initSDL()
	initGame()

	for {
		event()
		draw()
		stroke()
		sdl.Delay(step)
	}
}

func ck(err error) {
	if err != nil {
		log.Fatal("xsnake2d: ", err)
	}
}

func initSDL() {
	err := sdl.Init(sdl.INIT_VIDEO)
	ck(err)

	wflag := sdl.WINDOW_RESIZABLE
	if *fullscreen {
		wflag |= sdl.WINDOW_FULLSCREEN_DESKTOP
	}
	screen = newDisplay(W, H, wflag)
	screen.SetTitle("XSnake 2D")
	screen.SetLogicalSize(W, H)
	sdl.ShowCursor(0)
}

func event() {
	for {
		ev := sdl.PollEvent()
		if ev == nil {
			break
		}
		switch ev := ev.(type) {
		case sdl.QuitEvent:
			os.Exit(0)
		case sdl.KeyDownEvent:
			switch ev.Sym {
			case sdl.K_ESCAPE:
				os.Exit(0)
			case sdl.K_UP:
				if dir != 1 {
					dir = 3
				}
			case sdl.K_DOWN:
				if dir != 3 {
					dir = 1
				}
			case sdl.K_LEFT:
				if dir != 2 {
					dir = 4
				}
			case sdl.K_RIGHT:
				if dir != 4 {
					dir = 2
				}
			}
		}
	}
}

func draw() {
	screen.SetDrawColor(sdl.Color{150, 150, 150, 255})
	screen.Clear()
	drawField()
	drawSnake()
	drawFruit()
	screen.Present()
}

func drawField() {
	screen.SetDrawColor(sdl.Color{0, 0, 50, 255})
	for i := 0; i < N; i++ {
		screen.DrawLine(i*S, 0, i*S, H)
	}
	for i := 0; i < M; i++ {
		screen.DrawLine(0, i*S, W, i*S)
	}
}

func drawSnake() {
	screen.SetDrawColor(sdl.Color{255, 255, 0, 255})
	screen.FillRect(&sdl.Rect{
		s[0].X*S - 1,
		s[0].Y * S,
		S + 1,
		S + 1,
	})

	screen.SetDrawColor(sdl.Color{255, 0, 255, 255})
	for i := 1; i < sh; i++ {
		screen.FillRect(&sdl.Rect{
			s[i].X*S - 1,
			s[i].Y * S,
			S + 1,
			S + 1,
		})
	}
}

func drawFruit() {
	x, y := fruit.X, fruit.Y
	screen.SetDrawColor(sdlcolor.Red)
	screen.FillRect(&sdl.Rect{
		x*S - 1,
		y * S,
		S + 1,
		S + 1,
	})
}

func genFruit() {
	fruit = sdl.Point{int32(rand.Intn(N)), int32(rand.Intn(M))}
	for i := 0; i < sh; i++ {
		if s[i] == fruit {
			genFruit()
		}
	}
}

func stroke() {
	for i := sh; i > 0; i-- {
		s[i] = s[i-1]
	}

	switch dir {
	case 1:
		s[0].Y++
	case 2:
		s[0].X++
	case 3:
		s[0].Y--
	case 4:
		s[0].X--
	}

	if fruit == s[0] {
		sh++
		genFruit()

		if sh > mx {
			mymx = sh
		}

		if step > 1 {
			step -= step / 20
		}
	}

	if s[0].X < 0 || s[0].X >= N || s[0].Y < 0 || s[0].Y >= M {
		over()
		return
	}

	for i := 1; i < sh; i++ {
		if s[0] == s[i] {
			sh = i + 1
		}
	}
}

func over() {
	fmt.Println("GAME OVER!")
	initGame()
}
