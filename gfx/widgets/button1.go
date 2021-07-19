package main

import (
	"flag"
	"fmt"
	"image"
	"log"
	"os"
	"runtime"

	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
)

var (
	window   *sdl.Window
	renderer *sdl.Renderer
	ui       *UI
	buttons  []*Button
)

func main() {
	runtime.LockOSThread()
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

	window.SetTitle("Menu")
	sdlgfx.SetFont(sdlgfx.Font10x20, 10, 20)

	sdl.StartTextInput()
	renderer.SetBlendMode(sdl.BLENDMODE_BLEND)

	ui = NewUI()
	buttons = []*Button{
		NewButton(ButtonOptions{
			Pos:   image.Pt(100, 100),
			Text:  "Async Wd",
			Round: 30,
			Style: map[string]sdl.Color{
				"bg":    sdl.Color{40, 40, 95, 255},
				"hlbg":  sdl.Color{0, 0, 95, 255},
				"click": sdl.Color{45, 255, 255, 255},
				"text":  sdl.Color{205, 170, 125, 255},
			},
		}),

		NewButton(ButtonOptions{
			Pos:    image.Pt(300, 150),
			Text:   "~~Sonnets~~",
			Round:  0,
			Shadow: true,
			Style: map[string]sdl.Color{
				"bg":    sdl.Color{104, 56, 15, 255},
				"hlbg":  sdl.Color{20, 20, 95, 255},
				"click": sdl.Color{65, 55, 255, 255},
				"text":  sdl.Color{255, 255, 125, 255},
			},
		}),

		NewButton(ButtonOptions{
			Pos:    image.Pt(500, 160),
			Text:   "Question. Linker or Dimension?",
			Round:  10,
			Shadow: true,
			Style: map[string]sdl.Color{
				"bg":    sdl.Color{120, 30, 15, 255},
				"hlbg":  sdl.Color{20, 20, 95, 255},
				"click": sdl.Color{15, 35, 255, 255},
				"text":  sdl.Color{255, 34, 125, 255},
			},
		}),

		NewButton(ButtonOptions{
			Pos:   image.Pt(500, 300),
			Text:  "Sayonora to the ode field",
			Round: 5,
			Style: map[string]sdl.Color{
				"bg":    sdl.Color{10, 35, 15, 255},
				"hlbg":  sdl.Color{20, 20, 95, 255},
				"click": sdl.Color{105, 5, 25, 255},
				"text":  sdl.Color{24, 233, 11, 255},
			},
		}),

		NewButton(ButtonOptions{
			Pos:    image.Pt(850, 200),
			Text:   "Ipso Factor",
			Shadow: true,
			Round:  5,
			Style: map[string]sdl.Color{
				"bg":    sdl.Color{180, 180, 185, 255},
				"hlbg":  sdl.Color{253, 20, 95, 255},
				"click": sdl.Color{105, 5, 25, 255},
				"text":  sdl.Color{24, 233, 11, 255},
			},
		}),

		NewButton(ButtonOptions{
			Pos:   image.Pt(100, 600),
			Text:  "Maybe XL or DXXXL",
			Round: 5,
			Style: map[string]sdl.Color{
				"bg":    sdl.Color{104, 130, 185, 255},
				"hlbg":  sdl.Color{25, 25, 195, 255},
				"click": sdl.Color{255, 25, 105, 255},
				"text":  sdl.Color{100, 133, 41, 255},
			},
		}),

		NewButton(ButtonOptions{
			Pos:   image.Pt(640, 500),
			Text:  "Virtual Embeddings into Coordinate Free System",
			Round: 1,
			Style: map[string]sdl.Color{
				"bg":    sdl.Color{53, 134, 155, 255},
				"hlbg":  sdl.Color{225, 125, 105, 255},
				"click": sdl.Color{40, 30, 5, 255},
				"text":  sdl.Color{255, 255, 255, 255},
			},
		}),

		NewButton(ButtonOptions{
			Pos:   image.Pt(100, 800),
			Text:  "Press all the buttons",
			Round: 5,
			Style: map[string]sdl.Color{
				"bg":    sdl.Color{255, 34, 185, 255},
				"hlbg":  sdl.Color{225, 125, 105, 255},
				"click": sdl.Color{40, 30, 5, 255},
				"text":  sdl.Color{20, 20, 255, 255},
			},
		}),

		NewButton(ButtonOptions{
			Pos:   image.Pt(300, 550),
			Text:  "Timed Attack",
			Round: 15,
			Style: map[string]sdl.Color{
				"bg":    sdl.Color{24, 34, 185, 255},
				"hlbg":  sdl.Color{64, 125, 105, 255},
				"click": sdl.Color{40, 30, 5, 255},
				"text":  sdl.Color{56, 20, 255, 255},
			},
		}),

		NewButton(ButtonOptions{
			Pos:    image.Pt(600, 700),
			Text:   "HALT AND CATCH SURPLUS",
			Round:  13,
			Shadow: true,
			Style: map[string]sdl.Color{
				"bg":    sdl.Color{255, 100, 10, 255},
				"hlbg":  sdl.Color{45, 15, 105, 255},
				"click": sdl.Color{40, 30, 5, 255},
				"text":  sdl.Color{40, 50, 155, 255},
			},
		}),

		NewButton(ButtonOptions{
			Pos:   image.Pt(900, 800),
			Text:  "LUSR ERR",
			Round: 40,
			Style: map[string]sdl.Color{
				"bg":    sdl.Color{30, 255, 35, 255},
				"hlbg":  sdl.Color{225, 125, 105, 255},
				"click": sdl.Color{40, 30, 5, 255},
				"text":  sdl.Color{20, 20, 255, 255},
			},
		}),
	}
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
			ui.mouse.button = ev.Button
		case sdl.MouseButtonUpEvent:
			ui.mouse.Point = image.Pt(int(ev.X), int(ev.Y))
			ui.mouse.hit = 2
			ui.mouse.button = ev.Button
		case sdl.TextInputEvent:
			ui.text = ev.Text
		}
	}

	ui.win.width, ui.win.height, _ = renderer.OutputSize()
}

func blit() {
	renderer.SetDrawColor(sdl.Color{115, 117, 115, 255})
	renderer.Clear()
	for _, b := range buttons {
		b.Blit()
	}
	renderer.Present()
}

func blitRect(r image.Rectangle, c sdl.Color) {
	s := sdl.Recti(r)
	renderer.SetDrawColor(c)
	renderer.DrawRect(&s)
}

func fillRect(r image.Rectangle, c sdl.Color) {
	s := sdl.Recti(r)
	renderer.SetDrawColor(c)
	renderer.FillRect(&s)
}

func roundRect(r image.Rectangle, rad int, c sdl.Color) {
	renderer.SetDrawColor(c)
	sdlgfx.RoundedBox(renderer, r.Min.X, r.Min.Y, r.Max.X, r.Max.Y, rad, c)
}

func filledTrigon(x1, y1, x2, y2, x3, y3 int, c sdl.Color) {
	sdlgfx.FilledTrigon(renderer, x1, y1, x2, y2, x3, y3, c)
}

func fillArrow(x, y, s int, c sdl.Color) {
	x1 := x
	y1 := y
	x2 := x1
	y2 := y1 + s
	x3 := x1 + s/2
	y3 := y1 + s/2
	filledTrigon(x1, y1, x2, y2, x3, y3, c)
}

func blitText(x, y int, c sdl.Color, format string, args ...interface{}) {
	text := fmt.Sprintf(format, args...)
	renderer.SetDrawColor(c)
	sdlgfx.String(renderer, x, y, c, text)
}

func blitLine(x1, y1, x2, y2 int, c sdl.Color) {
	renderer.SetDrawColor(c)
	renderer.DrawLine(x1, y1, x2, y2)
}

func min(a ...int) int {
	x := a[0]
	for _, y := range a[1:] {
		if y < x {
			x = y
		}
	}
	return x
}

func max(a ...int) int {
	x := a[0]
	for _, y := range a[1:] {
		if y > x {
			x = y
		}
	}
	return x
}

func clamp(x, a, b int) int {
	if x < a {
		x = a
	} else if x > b {
		x = b
	}
	return x
}

func pointCanon(p image.Point) image.Point {
	if p.Y < p.X {
		p.X, p.Y = p.Y, p.X
	}
	return p
}

type UI struct {
	mouse struct {
		image.Point
		timestamp uint32
		hit       int
		button    uint8
	}
	kbd struct {
		key sdl.Keycode
		mod sdl.Keymod
		hit int
	}
	win struct {
		width  int
		height int
	}
	text string
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

type Button struct {
	ButtonOptions
	State int
}

type ButtonOptions struct {
	Pos     image.Point
	Text    string
	Handler func()
	Round   int
	Shadow  bool
	Style   map[string]sdl.Color
}

func NewButton(o ButtonOptions) *Button {
	return &Button{
		ButtonOptions: o,
	}
}

func (b *Button) Blit() {
	const (
		pad    = 10
		shadow = 10
	)
	fw, fh := sdlgfx.FontSize(b.Text)
	frame := image.Rect(b.Pos.X, b.Pos.Y, b.Pos.X+fw+pad, b.Pos.Y+fh+pad)
	shadowFrame := frame.Add(image.Pt(shadow, shadow))
	if b.Shadow {
		shadowFrame := frame.Add(image.Pt(shadow, shadow))
		roundRect(shadowFrame, b.Round, sdl.Color{0, 0, 0, 255})

	}
	if ui.mouse.In(frame) {
		if b.State == 1 {
			if b.Shadow {
				frame = shadowFrame
			}
			roundRect(frame, b.Round, b.Style["click"])
		} else {
			roundRect(frame, b.Round, b.Style["hlbg"])
		}
	} else {
		roundRect(frame, b.Round, b.Style["bg"])
	}

	blitText(frame.Min.X+pad/2, frame.Min.Y+pad/2, b.Style["text"], b.Text)

	b.State = ui.mouse.hit
	if !ui.mouse.In(frame) {
		return
	}

	if ui.mouse.hit == 1 && b.State != 1 {
		if b.Handler == nil {
			b.defaultHandler()
		} else {
			b.Handler()
		}
	}
}

func (b *Button) defaultHandler() {
	fmt.Printf("Button: %q\n", b.Text)
}
