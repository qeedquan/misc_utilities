package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	"image/draw"
	"log"
	"os"
	"runtime"
	"time"

	"github.com/qeedquan/go-media/image/ttf"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlimage/sdlcolor"
	"github.com/qeedquan/go-media/sdl/sdlttf"
)

var (
	conf struct {
		font       string
		fontsz     int
		width      int
		height     int
		fullscreen bool
		tick       time.Duration
	}

	font        *sdlttf.Font
	window      *sdl.Window
	renderer    *sdl.Renderer
	texture     *sdl.Texture
	canvas      *image.RGBA
	start, stop time.Time
	tick        time.Duration
	running     bool
)

func main() {
	runtime.LockOSThread()
	log.SetPrefix("stopwatch: ")
	log.SetFlags(0)
	parseFlags()
	initSDL()
	reset(false)
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

func parseFlags() {
	conf.width = 512
	conf.height = 180

	flag.Usage = usage
	flag.IntVar(&conf.width, "w", conf.width, "window width")
	flag.IntVar(&conf.height, "h", conf.height, "window height")
	flag.BoolVar(&conf.fullscreen, "fs", conf.fullscreen, "fullscreen mode")
	flag.StringVar(&conf.font, "f", conf.font, "use font file")
	flag.IntVar(&conf.fontsz, "s", conf.fontsz, "font size")
	flag.DurationVar(&conf.tick, "d", conf.tick, "start at duration")
	flag.Parse()
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options]")
	flag.PrintDefaults()
	os.Exit(2)
}

func initSDL() {
	err := sdl.Init(sdl.INIT_VIDEO)
	ck(err)

	err = sdlttf.Init()
	ck(err)

	w, h := conf.width, conf.height
	ptsz := conf.fontsz
	if ptsz <= 0 {
		switch {
		case w <= 100:
			ptsz = 8
		case w <= 256:
			ptsz = 16
		case w <= 512:
			ptsz = 20
		default:
			ptsz = 24
		}
	}
	if conf.font == "" {
		font, err = sdlttf.OpenFontMem(ttf.VGA437["default"], ptsz)
	} else {
		font, err = sdlttf.OpenFont(conf.font, ptsz)
	}
	ck(err)

	sdl.SetHint(sdl.HINT_RENDER_SCALE_QUALITY, "best")

	wflag := sdl.WINDOW_RESIZABLE
	if conf.fullscreen {
		wflag |= sdl.WINDOW_FULLSCREEN_DESKTOP
	}
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, wflag)
	ck(err)

	window.SetTitle("Stopwatch")
	renderer.SetLogicalSize(conf.width, conf.height)

	texture, err = renderer.CreateTexture(sdl.PIXELFORMAT_ABGR8888, sdl.TEXTUREACCESS_STREAMING, w, h)
	ck(err)

	canvas = image.NewRGBA(image.Rect(0, 0, w, h))
}

func reset(resetTick bool) {
	start = time.Now()
	stop = start
	if resetTick {
		tick = 0
	} else {
		tick = conf.tick
	}
	running = false
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
			case sdl.K_SPACE:
				running = !running
				if running {
					start = time.Now()
					stop = start
				} else {
					tick += stop.Sub(start)
				}
			case sdl.K_BACKSPACE:
				reset(false)
			case sdl.K_RETURN:
				reset(true)
			}
		}
	}
}

func update() {
	if !running {
		return
	}

	stop = time.Now()
}

func blit() {
	renderer.SetDrawColor(sdlcolor.Black)
	renderer.Clear()
	draw.Draw(canvas, canvas.Bounds(), image.NewUniform(color.Black), image.ZP, draw.Src)

	blitTime()

	texture.Update(nil, canvas.Pix, canvas.Stride)
	renderer.Copy(texture, nil, nil)
	renderer.Present()
}

func blitTime() {
	x, y := conf.width/2, conf.height/10

	sr := blitText(x, y, "Running: %v", running)
	y += sr.Dy()

	tm := start.Format(time.UnixDate)
	sr = blitText(x, y, "%-12s: %v", "Last Started", tm)

	y += sr.Dy()
	tm = stop.Format(time.UnixDate)
	blitText(x, y, "%-12s: %v", "Last Stopped", tm)

	elapsed := tick
	if running {
		elapsed += stop.Sub(start)
	}
	y += sr.Dy() * 3
	blitText(x, y, "%s: %v", "Elapsed", elapsed)
}

func blitText(x, y int, format string, args ...interface{}) image.Rectangle {
	text := fmt.Sprintf(format, args...)
	surface, err := font.RenderUTF8Blended(text, sdlcolor.White)
	ck(err)
	defer surface.Free()

	sr := surface.Bounds()
	x -= sr.Dx() / 2
	draw.Draw(canvas, image.Rect(x, y, x+sr.Dx(), y+sr.Dy()), surface, image.ZP, draw.Over)

	return sr
}
