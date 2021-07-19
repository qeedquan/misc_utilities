package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	"log"
	"math/rand"
	"os"
	"runtime"
	"time"

	"github.com/qeedquan/go-media/image/chroma"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
)

var (
	window   *sdl.Window
	renderer *sdl.Renderer
	ui       *UI
	layout   *Layout
)

func main() {
	runtime.LockOSThread()
	rand.Seed(time.Now().UnixNano())
	parseFlags()
	initSDL()
	for {
		ui.Begin()
		event()
		blit()
	}
}

func parseFlags() {
	flag.Parse()
}

func initSDL() {
	err := sdl.Init(sdl.INIT_VIDEO | sdl.INIT_TIMER)
	ck(err)

	w, h := 1440, 900
	wflag := sdl.WINDOW_RESIZABLE
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, wflag)
	ck(err)

	window.SetTitle("Frame")
	sdlgfx.SetFont(sdlgfx.Font10x20, 10, 20)

	sdl.StartTextInput()

	ui = NewUI()
	ui.bounds = image.Rect(0, 0, w, h)
	layout = NewLayout()
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
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
			default:
				ui.kbd.hit = 1
				ui.kbd.key = ev.Sym
				ui.kbd.mod = sdl.GetModState()
			}
		case sdl.KeyUpEvent:
			ui.kbd.hit = 2
			ui.kbd.key = ev.Sym
			ui.kbd.mod = sdl.GetModState()
		case sdl.MouseMotionEvent:
			ui.mouse.Point = image.Pt(int(ev.X), int(ev.Y))
		case sdl.MouseButtonDownEvent:
			ui.mouse.Point = image.Pt(int(ev.X), int(ev.Y))
			ui.mouse.hit = 1
			ui.mouse.clicks = ev.Clicks
			ui.mouse.button = ev.Button
		case sdl.MouseButtonUpEvent:
			ui.mouse.Point = image.Pt(int(ev.X), int(ev.Y))
			ui.mouse.hit = 2
			ui.mouse.clicks = ev.Clicks
			ui.mouse.button = ev.Button
		case sdl.TextInputEvent:
			ui.text = ev.Text
		}
	}
}

func blit() {
	renderer.SetDrawColor(color.RGBA{115, 140, 153, 255})
	renderer.Clear()
	layout.Blit()
	renderer.Present()
}

func fillRect(r image.Rectangle, c color.RGBA) {
	s := sdl.Recti(r)
	renderer.SetDrawColor(c)
	renderer.FillRect(&s)
}

func roundRect(r image.Rectangle, rad int, c sdl.Color) {
	renderer.SetDrawColor(c)
	sdlgfx.RoundedBox(renderer, r.Min.X, r.Min.Y, r.Max.X, r.Max.Y, rad, c)
}

func blitText(x, y int, c color.RGBA, format string, args ...interface{}) {
	text := fmt.Sprintf(format, args...)
	renderer.SetDrawColor(c)
	sdlgfx.String(renderer, x, y, c, text)
}

func blitLine(x1, y1, x2, y2 int, c color.RGBA) {
	renderer.SetDrawColor(c)
	renderer.DrawLine(x1, y1, x2, y2)
}

func blitCircle(x, y, r int, c color.RGBA) {
	sdlgfx.Circle(renderer, x, y, r, c)
}

func fillCircle(x, y, r int, c color.RGBA) {
	sdlgfx.FilledCircle(renderer, x, y, r, c)
}

type UI struct {
	mouse struct {
		image.Point
		button uint8
		clicks uint8
		hit    uint8
	}
	kbd struct {
		key sdl.Keycode
		mod sdl.Keymod
		hit int
	}
	bounds image.Rectangle
	text   string
}

func NewUI() *UI {
	return &UI{}
}

func (ui *UI) Begin() {
	if ui.mouse.hit == 2 {
		ui.mouse.hit = 0
	}
	if ui.kbd.hit == 2 {
		ui.kbd.hit = 0
	}
	ui.kbd.key = 0
	ui.text = ""
}

type Layout struct {
	Frames [][]*Frame
}

func NewLayout() *Layout {
	return &Layout{}
}

func (l *Layout) AddRow() {
	f, row, _, init := l.findOrInitFrame()
	if init {
		return
	}

	e := &Frame{
		Bounds: f.Bounds,
		Style: Style{
			BG: chroma.RandRGB(),
		},
	}
	dy := f.Bounds.Dy() / 2
	e.Bounds.Min.Y += dy
	f.Bounds.Max.Y -= dy
	l.Frames = append(l.Frames[:row], append([][]*Frame{{e}}, l.Frames[row:]...)...)
}

func (l *Layout) AddCol() {
	f, row, col, init := l.findOrInitFrame()
	if init {
		return
	}

	e := &Frame{
		Bounds: f.Bounds,
		Style: Style{
			BG: chroma.RandRGB(),
		},
	}
	dx := f.Bounds.Dx() / 2
	e.Bounds.Min.X += dx
	f.Bounds.Max.X -= dx
	l.Frames[row] = append(l.Frames[row][:col], append([]*Frame{e}, l.Frames[row][col:]...)...)
}

func (l *Layout) Blit() {
	if ui.kbd.hit == 2 {
		switch ui.kbd.key {
		case sdl.K_1:
			l.AddRow()
		case sdl.K_2:
			l.AddCol()
		case sdl.K_3:
			l.Frames = nil
		}
	}

	for r := range l.Frames {
		for c := range l.Frames[r] {
			l.Frames[r][c].Blit()
		}
	}
}

func (l *Layout) findOrInitFrame() (f *Frame, row, col int, init bool) {
	f, row, col = l.findFrame(ui.mouse.X, ui.mouse.Y)
	if f == nil {
		l.Frames = [][]*Frame{
			{
				{
					Bounds: ui.bounds,
					Style: Style{
						BG: chroma.RandRGB(),
					},
				},
			},
		}
		init = true
	}
	return
}

func (l *Layout) findFrame(x, y int) (*Frame, int, int) {
	p := image.Pt(x, y)
	for r := range l.Frames {
		for c := range l.Frames[r] {
			f := l.Frames[r][c]
			if p.In(f.Bounds) {
				return f, r, c
			}
		}
	}
	return nil, -1, -1
}

type Style struct {
	BG color.RGBA
}

type Frame struct {
	Bounds image.Rectangle
	View   image.Rectangle
	Obj    Obj
	Style  Style
}

type Obj interface {
	Blit()
}

const (
	BORDER_SIZE = 5
)

func (f *Frame) Blit() {
	white := color.RGBA{80, 80, 80, 255}
	black := color.RGBA{0, 0, 0, 255}
	r := f.Bounds
	fillRect(r, f.Style.BG)
	for i := 0; i < BORDER_SIZE; i++ {
		blitLine(r.Min.X+i, r.Min.Y, r.Min.X+i, r.Max.Y, black)
		blitLine(r.Min.X, r.Max.Y-i, r.Max.X, r.Max.Y-i, black)
		blitLine(r.Max.X-i, r.Min.Y, r.Max.X-i, r.Max.Y, white)
		blitLine(r.Min.X, r.Min.Y+i, r.Max.X, r.Min.Y+i, white)
	}
}