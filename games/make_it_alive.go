// ported from http://joel-murielle.perso.sfr.fr/make%20it%20alive.py
package main

import (
	"log"
	"os"
	"runtime"

	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlimage/sdlcolor"
)

type Display struct {
	*sdl.Window
	*sdl.Renderer
}

func NewDisplay(w, h int, flags sdl.WindowFlags) *Display {
	window, renderer, err := sdl.CreateWindowAndRenderer(w, h, flags)
	ck(err)
	return &Display{window, renderer}
}

func ck(err error) {
	if err != nil {
		log.Fatal("sdl:", err)
	}
}

func main() {
	runtime.LockOSThread()
	log.SetFlags(0)
	err := sdl.Init(sdl.INIT_VIDEO)
	ck(err)

	screen := NewDisplay(500, 500, sdl.WINDOW_RESIZABLE)
	screen.SetLogicalSize(500, 500)
	screen.SetTitle("Make It Alive!")

	foo := 3
	bar := 2

	unit := []sdl.Point{
		{-1, -1}, {0, -1}, {1, -1}, {-1, 0},
		{1, 0}, {-1, 1}, {0, 1}, {1, 1},
	}

	var p, z map[sdl.Point]bool
	var q map[sdl.Point]int

	for {
		screen.SetDrawColor(sdlcolor.Black)
		screen.Clear()

		z = make(map[sdl.Point]bool)
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
				}
			case sdl.MouseButtonDownEvent:
				for _, u := range unit {
					n := sdl.Point{ev.X + u.X, ev.Y + u.Y}
					z[n] = true
				}
			}
		}

		for i := range q {
			if q[i] == foo {
				z[i] = true
				if p[i] {
					screen.SetDrawColor(sdl.Color{250, 150, 150, 255})
				} else {
					screen.SetDrawColor(sdl.Color{150, 150, 250, 255})
				}
				screen.DrawPoint(int(i.X), int(i.Y))
			} else if q[i] == bar && p[i] {
				z[i] = true
				screen.SetDrawColor(sdl.Color{250, 150, 150, 255})
				screen.DrawPoint(int(i.X), int(i.Y))
			}
		}

		p = z
		q = make(map[sdl.Point]int)
		for v := range p {
			for _, u := range unit {
				n := sdl.Point{v.X + u.X, v.Y + u.Y}
				q[n] = q[n] + 1
			}
		}

		screen.Present()
	}
}
