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
	tooltips []*Tooltip
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

	window.SetTitle("Tooltip")
	sdlgfx.SetFont(sdlgfx.Font10x20, 10, 20)

	sdl.StartTextInput()
	renderer.SetBlendMode(sdl.BLENDMODE_BLEND)

	ui = NewUI()
	tooltips = []*Tooltip{
		NewTooltip(TooltipOptions{
			Pos:   image.Pt(100, 100),
			Pad:   20,
			Round: 20,
			Arrow: 0,
			Text:  "Baxter Solvent",
			Style: map[string]sdl.Color{
				"fg": sdl.Color{180, 180, 180, 200},
				"bg": sdl.Color{20, 20, 20, 150},
			},
		}),

		NewTooltip(TooltipOptions{
			Pos:   image.Pt(400, 100),
			Pad:   20,
			Round: 30,
			Arrow: 1,
			Text:  "Ways about things",
			Style: map[string]sdl.Color{
				"fg": sdl.Color{200, 255, 180, 200},
				"bg": sdl.Color{40, 60, 20, 250},
			},
		}),

		NewTooltip(TooltipOptions{
			Pos:   image.Pt(200, 300),
			Pad:   20,
			Round: 0,
			Arrow: 2,
			Text:  "Axle Gimbal Space Lock",
			Style: map[string]sdl.Color{
				"fg": sdl.Color{0, 0, 255, 200},
				"bg": sdl.Color{255, 0, 20, 250},
			},
		}),

		NewTooltip(TooltipOptions{
			Pos:   image.Pt(500, 350),
			Pad:   20,
			Round: 70,
			Arrow: 1,
			Text:  "Ys Chronicles",
			Style: map[string]sdl.Color{
				"fg": sdl.Color{255, 255, 255, 255},
				"bg": sdl.Color{40, 35, 70, 250},
			},
		}),

		NewTooltip(TooltipOptions{
			Pos:   image.Pt(600, 550),
			Pad:   30,
			Round: 30,
			Arrow: 2,
			Text:  "Blexomfmt Bussole",
			Style: map[string]sdl.Color{
				"fg": sdl.Color{25, 255, 255, 255},
				"bg": sdl.Color{40, 135, 70, 250},
			},
		}),

		NewTooltip(TooltipOptions{
			Pos:   image.Pt(900, 550),
			Pad:   30,
			Round: 40,
			Arrow: 1,
			Text:  "Quantized Spacetime",
			Style: map[string]sdl.Color{
				"fg": sdl.Color{250, 10, 25, 255},
				"bg": sdl.Color{140, 135, 70, 250},
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
}

func blit() {
	renderer.SetDrawColor(sdl.Color{115, 140, 153, 255})
	renderer.Clear()
	for _, tp := range tooltips {
		tp.Blit()
	}
	renderer.Present()
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
		hit    int
		button uint8
	}
	kbd struct {
		key sdl.Keycode
		mod sdl.Keymod
		hit int
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

type Tooltip struct {
	TooltipOptions
}

type TooltipOptions struct {
	Pos   image.Point
	Round int
	Pad   int
	Text  string
	Style map[string]sdl.Color
	Arrow int
}

func NewTooltip(opt TooltipOptions) *Tooltip {
	return &Tooltip{
		TooltipOptions: opt,
	}
}

func (tp *Tooltip) Blit() {
	fontWidth, fontHeight := sdlgfx.FontSize(tp.Text)
	frame := image.Rect(
		tp.Pos.X,
		tp.Pos.Y,
		tp.Pos.X+fontWidth+tp.Pad,
		tp.Pos.Y+fontHeight+tp.Pad,
	)
	roundRect(frame, tp.Round, tp.Style["bg"])

	textPos := image.Pt(
		tp.Pos.X+tp.Pad/2,
		tp.Pos.Y+tp.Pad/2,
	)
	blitText(textPos.X, textPos.Y, tp.Style["fg"], tp.Text)

	switch tp.Arrow {
	case 1:
		p1 := image.Pt(
			int(float64(frame.Min.X)+.2*float64(frame.Dx())),
			frame.Max.Y,
		)
		p2 := image.Pt(
			int(float64(frame.Min.X)+.4*float64(frame.Dx())),
			frame.Max.Y,
		)
		p3 := image.Pt(
			int(float64(frame.Min.X)+.22*float64(frame.Dx())),
			frame.Max.Y+30,
		)
		filledTrigon(p1.X, p1.Y, p2.X, p2.Y, p3.X, p3.Y, tp.Style["bg"])
	case 2:
		p1 := image.Pt(
			int(float64(frame.Min.X)+.2*float64(frame.Dx())),
			frame.Min.Y,
		)
		p2 := image.Pt(
			int(float64(frame.Min.X)+.4*float64(frame.Dx())),
			frame.Min.Y,
		)
		p3 := image.Pt(
			int(float64(frame.Min.X)+.22*float64(frame.Dx())),
			frame.Min.Y-20,
		)
		filledTrigon(p1.X, p1.Y, p2.X, p2.Y, p3.X, p3.Y, tp.Style["bg"])
	}
}