// http://jwilson.coe.uga.edu/EMT668/EMT668.Folders.F97/Anderson/writeup11/writeup11.html
package main

import (
	"fmt"
	"image/color"
	"log"
	"math"
	"math/rand"
	"os"
	"runtime"

	"github.com/qeedquan/go-media/image/chroma"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
)

var (
	window       *sdl.Window
	renderer     *sdl.Renderer
	radius       float64
	theta        float64
	colormode    int
	spiralmode   int
	noisy        float64
	spiralradius int
)

func main() {
	runtime.LockOSThread()
	initSDL()
	for {
		event()
		update()
		blit()
	}
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func initSDL() {
	err := sdl.Init(sdl.INIT_VIDEO | sdl.INIT_TIMER)
	ck(err)

	sdl.SetHint(sdl.HINT_RENDER_SCALE_QUALITY, "best")
	w, h := 1280, 800
	wflag := sdl.WINDOW_RESIZABLE
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, wflag)
	ck(err)

	window.SetTitle("Spiral")

	spiralradius = 1
	reset()
}

func reset() {
	radius, theta = 0, 0
	blitBG()
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
			case sdl.K_LEFT:
				if colormode > 0 {
					colormode--
				}
			case sdl.K_RIGHT:
				if colormode < 1 {
					colormode++
				}
			case sdl.K_1:
				noisy--
			case sdl.K_2:
				noisy++
			case sdl.K_q:
				if spiralmode > 0 {
					spiralmode--
				}
			case sdl.K_w:
				if spiralmode < 11 {
					spiralmode++
				}
			case sdl.K_e:
				spiralradius--
			case sdl.K_r:
				spiralradius++
			default:
				continue
			}
		case sdl.WindowEvent:
		default:
			continue
		}
		fmt.Printf("noisy: %v\n", noisy)
		fmt.Printf("color mode: %v\n", colormode)
		fmt.Printf("spiral mode: %v\n", spiralmode)
		fmt.Printf("spiral radius: %v\n", spiralradius)
		fmt.Println()
		reset()
	}
}

func update() {
	radius += 0.05
	if theta += 0.01; theta >= 2*math.Pi {
		theta -= 2 * math.Pi
	}
}

func blitBG() {
	renderer.SetDrawColor(color.RGBA{150, 150, 150, 255})
	renderer.Clear()
	renderer.Present()
}

func blit() {
	blitSpiral(radius, theta)
	renderer.Present()
}

// convert polar coordinate to rectangular coordinate and draw at each of those
// coordinates, increasing radius as we move over time makes a spiral
func blitSpiral(r, t float64) {
	w, h, _ := renderer.OutputSize()

	var x, y float64
	rx, ry := r, r
	tx, ty := t, t
	switch spiralmode {
	case 0:
	case 1:
		rx *= math.Sqrt(t)
	case 2:
		ty = t * t * t
	case 3:
		tx = math.Sqrt(r)
		ty = tx
	case 4:
		tx = t * t
		ty = tx
	case 5:
		rx = r * 2 * math.Sin(4*t)
		ry = rx
	case 6: // limacon
		rx = r * (0.5 + math.Cos(t))
		ry = rx
	case 7: // dimpled limacons
		rx = r * (3/2.0 + math.Cos(t))
		ry = rx
	case 8: // cardoid
		rx = r * (1 + math.Cos(t))
		ry = rx
	case 9:
		rx = 3 * math.Pow(math.Cos(t)*math.Sin(t), 2)
		rx /= math.Pow(math.Cos(t), 5) + math.Pow(math.Sin(t), 5)
		rx *= r
		ry = r
	case 10: // hyperbolic spiral
		rx = r * 0.5 / t
		ry = rx
	case 11:
		rx = r * 4 * math.Cos(7*t)
		ry = rx
	}

	x = rx*math.Cos(tx) + float64(w)/2
	y = ry*math.Sin(ty) + float64(h)/2

	x += noisy * rand.Float64()
	y += noisy * rand.Float64()

	switch colormode {
	case 0:
		sdlgfx.FilledCircle(renderer, int(x), int(y), spiralradius, color.RGBA{0, 0, 0, 255})
	case 1:
		sdlgfx.FilledCircle(renderer, int(x), int(y), spiralradius, chroma.RandRGBA())
	}
}
