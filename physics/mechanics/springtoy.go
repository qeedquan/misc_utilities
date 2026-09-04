// https://sol.gfxile.net/springs/
package main

import (
	"image/color"
	"log"
	"math"
	"os"
	"runtime"

	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlimage/sdlcolor"
)

const (
	TICK_TIMEWARP = 100
	PHYSICS_FPS   = 100
	WINDOW_WIDTH  = 800
	WINDOW_HEIGHT = 600
)

var (
	window    *sdl.Window
	renderer  *sdl.Renderer
	mp        []*masspoint
	sp        []*spring
	last_tick uint32
)

func main() {
	runtime.LockOSThread()
	init_sdl()
	init_spring()
	loop()
}

func init_sdl() {
	err := sdl.Init(sdl.INIT_VIDEO | sdl.INIT_TIMER)
	ck(err)

	w, h := WINDOW_WIDTH, WINDOW_HEIGHT
	wflags := sdl.WINDOW_RESIZABLE
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, wflags)
	ck(err)

	window.SetTitle("Spring Toy")
	renderer.SetLogicalSize(w, h)
}

func init_spring() {
	p1 := define_masspoint(100, 100)
	p2 := define_masspoint(100, 200)
	p3 := define_masspoint(200, 100)
	p4 := define_masspoint(200, 200)

	p5 := define_masspoint(100, 150)
	p6 := define_masspoint(150, 100)
	p7 := define_masspoint(200, 150)
	p8 := define_masspoint(150, 200)

	// around
	define_spring(p1, p2, 0.05)
	define_spring(p2, p4, 0.05)
	define_spring(p3, p4, 0.05)
	define_spring(p3, p1, 0.05)

	// cross
	define_spring(p1, p4, 0.05)
	define_spring(p2, p3, 0.05)

	// midpoints to corners
	define_spring(p1, p5, 0.05)
	define_spring(p1, p6, 0.05)
	define_spring(p2, p5, 0.05)
	define_spring(p2, p8, 0.05)
	define_spring(p3, p7, 0.05)
	define_spring(p3, p6, 0.05)
	define_spring(p4, p7, 0.05)
	define_spring(p4, p8, 0.05)

	// midpoints across
	define_spring(p5, p7, 0.05)
	define_spring(p6, p8, 0.05)

	// midpoints diamond
	define_spring(p5, p6, 0.05)
	define_spring(p6, p7, 0.05)
	define_spring(p7, p8, 0.05)
	define_spring(p8, p5, 0.05)

	// corners to midpoints
	define_spring(p1, p7, 0.05)
	define_spring(p1, p8, 0.05)
	define_spring(p2, p6, 0.05)
	define_spring(p2, p7, 0.05)
	define_spring(p3, p5, 0.05)
	define_spring(p3, p8, 0.05)
	define_spring(p4, p5, 0.05)
	define_spring(p4, p5, 0.05)
}

func loop() {
	for {
		event()
		update()
		render()
		sdl.Delay(10)
	}
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
			}
		}
	}
}

func update() {
	tick := sdl.GetTicks()
	if tick-last_tick > TICK_TIMEWARP {
		last_tick = tick
	}
	if last_tick >= tick {
		sdl.Delay(1)
		return
	}

	for last_tick < tick {
		physics_tick()
		last_tick += 1000 / PHYSICS_FPS
	}
}

func render() {
	renderer.SetDrawColor(color.RGBA{25, 25, 51, 255})
	renderer.Clear()
	for i := range mp {
		r := sdl.Rect{
			int32(mp[i].x - 2),
			int32(mp[i].y - 2),
			5, 5,
		}
		renderer.SetDrawColor(sdlcolor.White)
		renderer.DrawRect(&r)
	}
	for i := range sp {
		renderer.SetDrawColor(sdlcolor.Green)
		renderer.DrawLine(
			int(sp[i].p1.x), int(sp[i].p1.y),
			int(sp[i].p2.x), int(sp[i].p2.y),
		)
	}
	renderer.Present()
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

type masspoint struct {
	x, y   float64 // position
	xi, yi float64 // velocity
}

type spring struct {
	p1, p2    *masspoint // one spring links 2 masses together
	desired_d float64    // distance between 2 masses
	fx, fy    float64    // force
	k         float64    // spring constant
}

func mp_distance(mp1, mp2 *masspoint) float64 {
	dx := mp1.x - mp2.x
	dy := mp1.y - mp2.y
	return math.Hypot(dx, dy)
}

func define_spring(mp1, mp2 *masspoint, k float64) {
	sp = append(sp, &spring{
		p1:        mp1,
		p2:        mp2,
		desired_d: mp_distance(mp1, mp2),
		k:         k,
	})
}

func define_masspoint(x, y float64) *masspoint {
	mp = append(mp, &masspoint{
		x: x,
		y: y,
	})
	return mp[len(mp)-1]
}

func physics_tick() {
	for i := range sp {
		dx := sp[i].p1.x - sp[i].p2.x
		dy := sp[i].p1.y - sp[i].p2.y
		d := math.Hypot(dx, dy)
		if d > 0.000001 {
			// F = -ky

			// how much of the force goes to horizontal and vertical vectors
			fx := dx / d
			fy := dy / d

			intensity := (d - sp[i].desired_d) * sp[i].k

			// apply
			sp[i].fx = fx * intensity
			sp[i].fy = fy * intensity
			sp[i].p1.xi -= sp[i].fx
			sp[i].p1.yi -= sp[i].fy
			sp[i].p2.xi += sp[i].fx
			sp[i].p2.yi += sp[i].fy
		}
	}

	for i := range mp {
		mp[i].yi += 0.2
		mp[i].x += mp[i].xi
		mp[i].y += mp[i].yi
		if mp[i].y > WINDOW_HEIGHT {
			mp[i].y = WINDOW_HEIGHT
		}
		if mp[i].x < 0 {
			mp[i].x = 0
		}
		if mp[i].x > WINDOW_WIDTH {
			mp[i].x = WINDOW_WIDTH
		}
		mp[i].xi *= 0.95
		mp[i].yi *= 0.95
	}
}
