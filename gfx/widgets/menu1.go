package main

import (
	"flag"
	"fmt"
	"image"
	"log"
	"os"
	"runtime"
	"strings"

	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
)

var (
	window   *sdl.Window
	renderer *sdl.Renderer
	ui       *UI
	menus    []*Menu
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
	menus = []*Menu{
		&Menu{
			Align: ALIGN_CENTER,
			Entry: []*Entry{
				{Text: "Open Document\nNein\nChordal Transform"},
				{
					Text: "Crawl Shell",
					Link: []*Entry{
						{Text: "Crab"},
						{
							Text: "Squid",
							Link: []*Entry{
								{Text: "XZ Decomp"},
								{Text: "Study for Test"},
								{Text: "Bengu Tribe"},
							},
						},
						{Text: "Fishxu"},
					},
				},
				{Text: "Magnolia Forms"},
				{Text: "XBED Test"},
				{
					Text: "Constant Curvature\nEigen Transfer",
					Link: []*Entry{
						{Text: "K1 Sphere"},
						{Text: "Twistor Invariant"},
						{Text: "SuperN Yumpert"},
					},
				},
				{Text: "Dyslx 1"},
				{Text: "XxX Quotient"},
				{Text: "Kedstral Delight"},
			},
			Style: map[string]sdl.Color{
				"bg":      sdl.Color{0xef, 0xff, 0xef, 0xff},
				"border":  sdl.Color{0x8c, 0xcf, 0x8c, 0xff},
				"text":    sdl.Color{0x0, 0x0, 0x0, 0xff},
				"arrow":   sdl.Color{0x8c, 0xcf, 0x8c, 0xff},
				"hilight": sdl.Color{66, 138, 66, 255},
			},
		},
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
			ui.mouse.timestamp = ev.Timestamp
		case sdl.MouseButtonDownEvent:
			ui.mouse.Point = image.Pt(int(ev.X), int(ev.Y))
			ui.mouse.hit = 1
			ui.mouse.button = ev.Button
			ui.mouse.timestamp = ev.Timestamp
		case sdl.MouseButtonUpEvent:
			ui.mouse.Point = image.Pt(int(ev.X), int(ev.Y))
			ui.mouse.hit = 2
			ui.mouse.button = ev.Button
			ui.mouse.timestamp = ev.Timestamp
		case sdl.TextInputEvent:
			ui.text = ev.Text
		}
	}

	ui.win.width, ui.win.height, _ = renderer.OutputSize()
}

func blit() {
	renderer.SetDrawColor(sdl.Color{115, 117, 115, 255})
	renderer.Clear()
	for _, m := range menus {
		m.Blit()
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

const (
	ALIGN_LEFT = iota
	ALIGN_CENTER
	ALIGN_RIGHT
)

type Menu struct {
	Pos    image.Point
	Pad    int
	Align  int
	State  int
	Entry  []*Entry
	Select []int
	Style  map[string]sdl.Color
	Mouse  struct {
		Timestamp uint32
	}
}

type Entry struct {
	Text    string
	Handler func()
	Link    []*Entry
}

func (m *Menu) Blit() {
	m.Pad = 15
	if ui.mouse.hit == 1 && ui.mouse.button == 3 && m.State == 0 {
		m.State = 1
		m.Pos = ui.mouse.Point
		m.Mouse.Timestamp = ui.mouse.timestamp
	}

	if m.State == 0 {
		return
	}

	_, fh, _ := sdlgfx.FontMetrics()
	p := m.Pos
	e := m.Entry
	in := false
	sd := []int{}
	lv := 0
	llr := -1
	for {
		r, s, lr := m.bounds(p, e)
		if llr == -1 {
			llr = lr
		}

		fillRect(r, m.Style["bg"])
		for i := 0; i < 3; i++ {
			blitRect(r.Inset(i), m.Style["border"])
		}
		d := r.Inset(3)
		q := image.Pt(s.Min.X, s.Min.Y)

		if ui.mouse.In(r) {
			in = true
		}

		for i, n := range e {
			_, bh := m.strSize(n.Text, n)
			b := image.Rect(d.Min.X, q.Y, d.Max.X, q.Y+bh)
			if i == 0 {
				b.Min.Y = d.Min.Y
			} else if i == len(e)-1 {
				b.Max.Y = d.Max.Y
			}

			if ui.mouse.In(b) {
				fillRect(b, m.Style["hilight"])
				if n.Link != nil {
					sd = append(sd, i)
				}
			}
			if n.Link != nil {
				fillArrow(r.Max.X-m.Pad, q.Y+3, fh*4/5, m.Style["arrow"])
			}

			t := strings.Split(n.Text, "\n")
			for _, t := range t {
				v := q
				w, _ := sdlgfx.FontSize(t)
				switch m.Align {
				case ALIGN_CENTER:
					v.X = s.Min.X + (s.Dx()-w)/2
				case ALIGN_RIGHT:
					v.X = s.Max.X - w
				}
				sdlgfx.String(renderer, v.X, v.Y, m.Style["text"], t)
				q.Y += fh
			}
		}
		if lv < len(sd) {
			e = e[sd[lv]].Link
		} else if lv < len(m.Select) {
			sd = append(sd, m.Select[lv])
			e = e[sd[lv]].Link
		} else {
			break
		}

		if llr == 0 {
			p.X = r.Max.X
		} else {
			p.X = r.Min.X
		}
		lv++
	}

	if len(sd) > 0 {
		m.Select = sd
	}
	if !in && ui.mouse.hit != 0 && ui.mouse.timestamp-m.Mouse.Timestamp >= 200 {
		m.State = 0
		m.Select = m.Select[:0]
	}
}

func (m *Menu) bounds(p image.Point, e []*Entry) (r, s image.Rectangle, lr int) {
	w, h := 0, 0
	aw := 0
	for _, e := range e {
		t := strings.Split(e.Text, "\n")
		for _, t := range t {
			fw, fh := sdlgfx.FontSize(t)
			if e.Link != nil {
				aw = m.Pad
			}
			w = max(w, fw)
			h += fh
		}
	}
	lr = 0
	r = image.Rect(p.X, p.Y, p.X+w+m.Pad, p.Y+h+m.Pad)
	if r.Min.X >= ui.win.width || r.Max.X+aw > ui.win.width {
		dx := r.Dx()
		r.Min.X = p.X - dx
		r.Max.X = p.X
		lr = 1
	}
	if r.Min.Y >= ui.win.height || r.Max.Y > ui.win.height {
		dy := r.Dy()
		r.Min.Y = p.Y - dy
		r.Max.Y = p.Y
	}
	s = image.Rect(
		r.Min.X+m.Pad/2,
		r.Min.Y+m.Pad/2,
		r.Max.X-m.Pad/2,
		r.Max.Y-m.Pad/2,
	)
	r.Max.X += aw
	return
}

func (m *Menu) strSize(s string, e *Entry) (w, h int) {
	fw, fh := 0, 0
	t := strings.Split(s, "\n")
	for _, t := range t {
		fw, fh = sdlgfx.FontSize(t)
		w = max(w, fw)
		h += fh
	}
	if e.Link != nil {
		w += m.Pad * 2
	}
	return
}