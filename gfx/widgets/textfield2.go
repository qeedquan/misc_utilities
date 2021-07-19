package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	"log"
	"os"
	"runtime"
	"unicode"

	"github.com/go-gl/gl/v3.3-core/gl"
	"github.com/qeedquan/go-media/glew"
	"github.com/qeedquan/go-media/image/ttf"
	"github.com/qeedquan/go-media/math/f64"
	"github.com/qeedquan/go-media/math/mathutil"
	"github.com/qeedquan/go-media/nvg"
	"github.com/qeedquan/go-media/sdl"
)

var (
	window    *sdl.Window
	vg        *nvg.Context
	ui        *UI
	textField [8]*TextField
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("textfields: ")
	runtime.LockOSThread()
	flag.Parse()
	initGUI()
	for {
		ui.Begin()
		event()
		frame()
	}
}

func initGUI() {
	err := sdl.Init(sdl.INIT_VIDEO)
	ck(err)

	sdl.GLSetAttribute(sdl.GL_CONTEXT_MAJOR_VERSION, 3)
	sdl.GLSetAttribute(sdl.GL_CONTEXT_MINOR_VERSION, 3)

	window, err = sdl.CreateWindow("Text Fields",
		sdl.WINDOWPOS_UNDEFINED, sdl.WINDOWPOS_UNDEFINED,
		1024, 768,
		sdl.WINDOW_OPENGL|sdl.WINDOW_SHOWN|sdl.WINDOW_RESIZABLE,
	)
	ck(err)

	_, err = window.CreateContextGL()
	ck(err)

	err = gl.Init()
	ck(err)

	err = glew.Init()
	ck(err)

	vg, err = nvg.CreateGL3(nvg.ANTIALIAS | nvg.STENCIL_STROKES)
	ck(err)

	err = vg.CreateFontMem("sans", ttf.Roboto["default"])
	ck(err)

	ui = NewUI()
	for i := range textField {
		textField[i] = NewTextField()
		textField[i].SetText(i, "abcABC@BX!03932(#*_XZTUV")
	}

	window.SetMinimumSize(512, 512)
	sdl.StartTextInput()
	sdl.EventState(sdl.CLIPBOARDUPDATE, sdl.ENABLE)
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
				ui.Event(ev)
			}
		default:
			ui.Event(ev)
		}
	}
}

func frame() {
	w, h := window.Size()
	gl.ClearColor(.3, .4, .5, 1)
	gl.Clear(gl.COLOR_BUFFER_BIT | gl.STENCIL_BUFFER_BIT | gl.DEPTH_BUFFER_BIT)
	gl.Viewport(0, 0, int32(w), int32(h))
	vg.BeginFrame(float64(w), float64(h), 1)

	vp := f64.Rect(10, 10, 600, 50)
	for i := range textField {
		textField[i].Frame(vp)
		vp = vp.Add(f64.Vec2{0, vp.Dy() + 10})
		vp.Max.X += 50
		vp.Max.Y += 10
	}

	vg.EndFrame()
	window.SwapGL()
}

const (
	COLOR_BG = iota
	COLOR_HOVER_BG
	COLOR_SELECT_TEXT
	COLOR_TEXT
	COLOR_MAX
)

type UI struct {
	Mouse struct {
		f64.Vec2
		Button uint8
		Press  uint8
		Wheel  f64.Vec2
	}
	Key struct {
		Sym   sdl.Keycode
		Mod   sdl.Keymod
		Press uint8
	}
	Style Style
	Text  string
}

type Style struct {
	Font   string
	Colors [COLOR_MAX]color.RGBA
}

func NewUI() *UI {
	return &UI{
		Style: Style{
			Font: "sans",
			Colors: [COLOR_MAX]color.RGBA{
				COLOR_BG:          color.RGBA{10, 40, 60, 255},
				COLOR_HOVER_BG:    color.RGBA{100, 100, 200, 255},
				COLOR_SELECT_TEXT: color.RGBA{0, 100, 0, 255},
				COLOR_TEXT:        color.RGBA{200, 200, 200, 255},
			},
		},
	}
}

func (ui *UI) Begin() {
	k := &ui.Key
	k.Press = 0

	m := &ui.Mouse
	m.Wheel.X = 0
	m.Wheel.Y = 0
	if m.Press == 2 {
		m.Press = 0
	}

	ui.Text = ""
}

func (ui *UI) Event(ev sdl.Event) {
	switch ev := ev.(type) {
	case sdl.MouseButtonDownEvent:
		ui.evMouseButton(sdl.MouseButtonEvent(ev), 1)
	case sdl.MouseButtonUpEvent:
		ui.evMouseButton(sdl.MouseButtonEvent(ev), 2)
	case sdl.MouseMotionEvent:
		m := &ui.Mouse
		m.X = float64(ev.X)
		m.Y = float64(ev.Y)
	case sdl.MouseWheelEvent:
		w := &ui.Mouse.Wheel
		w.X = float64(ev.X)
		w.Y = float64(ev.Y)
		if ev.Direction == sdl.MOUSEWHEEL_FLIPPED {
			w.X *= -1
			w.Y *= -1
		}
	case sdl.KeyDownEvent:
		ui.evKeyEvent(sdl.KeyboardEvent(ev), 1)
	case sdl.KeyUpEvent:
		ui.evKeyEvent(sdl.KeyboardEvent(ev), 2)
	case sdl.TextInputEvent:
		ui.Text = ev.Text
	}
}

func (ui *UI) evKeyEvent(ev sdl.KeyboardEvent, press uint8) {
	k := &ui.Key
	k.Sym = ev.Sym
	k.Press = press
	k.Mod = sdl.GetModState()
}

func (ui *UI) evMouseButton(ev sdl.MouseButtonEvent, press uint8) {
	m := &ui.Mouse
	m.X = float64(ev.X)
	m.Y = float64(ev.Y)
	m.Button = ev.Button
	m.Press = press
}

type TextField struct {
	text     []rune
	editable bool
	selected bool
	sel      image.Point
	start    int
	cur      int
}

func NewTextField() *TextField {
	return &TextField{
		editable: true,
		sel:      image.Pt(-1, -1),
	}
}

func (tf *TextField) SetText(args ...interface{}) {
	tf.text = []rune(fmt.Sprint(args...))
	tf.start = 0
	tf.cur = 0
}

func (tf *TextField) SetEditable(editable bool) {
	tf.editable = editable
}

func (tf *TextField) Frame(dc f64.Rectangle) {
	tf.frameBG(dc)

	vp := dc.Inset(10)
	tf.frameText(dc, vp)
}

func (tf *TextField) frameBG(dc f64.Rectangle) {
	vg.Save()
	defer vg.Restore()

	st := &ui.Style
	m := &ui.Mouse
	x, y, w, h := dc.PosSize()
	vg.Scissor(x, y, w, h)

	c := st.Colors[COLOR_BG]
	if m.In(dc) {
		c = st.Colors[COLOR_HOVER_BG]
	}

	vg.BeginPath()
	vg.FillColor(c)
	vg.Rect(x, y, w, h)
	vg.Fill()
}

func (tf *TextField) frameText(dc, tc f64.Rectangle) {
	vg.Save()
	defer vg.Restore()

	st := &ui.Style
	const enlarge = 16
	x, y, w, h := tc.PosSize()
	y -= enlarge / 2
	h += enlarge
	vg.Scissor(x, y, w, h)

	vg.FontFace(st.Font)
	vg.FontSize(h)
	vg.TextAlign(nvg.ALIGN_LEFT | nvg.ALIGN_TOP)

	lw, _ := tf.lineWidth(tc)
	if tf.cur > lw {
		tf.start++
		tf.cur = lw
	}

	if tf.sel.X != -1 && tf.sel.Y != -1 {
		lx, ly := tf.getSel()

		s := tf.start
		e := tf.start + lw
		if s <= lx && lx <= e {
			s = lx
		}
		if s <= ly && ly <= e {
			e = ly
		}

		xadv, _, _, _, _ := vg.TextBounds(x, y, string(tf.text[s:e]), -1)
		oxadv, _, _, _, _ := vg.TextBounds(x, y, string(tf.text[tf.start:s]), -1)
		vg.BeginPath()
		vg.FillColor(st.Colors[COLOR_SELECT_TEXT])
		vg.Rect(x+oxadv, y, xadv, h)
		vg.Fill()
	}

	text := string(tf.text[tf.start : tf.start+lw])
	vg.BeginPath()
	vg.FillColor(st.Colors[COLOR_TEXT])
	vg.Text(x, y, text, -1)

	if tf.sel.X == tf.sel.Y {
		vg.BeginPath()
		text = string(tf.text[tf.start : tf.start+tf.cur])
		xadv, _, _, _, _ := vg.TextBounds(0, 0, text, -1)
		vg.FillColor(st.Colors[COLOR_TEXT])
		vg.Rect(x+xadv, y, 1, h)
		vg.Fill()
	}

	tf.frameEvent(dc, tc)
}

func (tf *TextField) moveLeft(n int) {
	for i := 0; i < n; i++ {
		if tf.cur--; tf.cur < 0 {
			tf.cur = 0
			tf.start = mathutil.Max(tf.start-1, 0)
		}
	}
}

func (tf *TextField) moveRight(lw int, n int) {
	for i := 0; i < n; i++ {
		if tf.cur++; tf.cur >= lw {
			tf.cur = lw
			if tf.start+tf.cur < len(tf.text) {
				tf.start = mathutil.Min(tf.start+1, len(tf.text)+1)
				tf.cur--
			}
		}
	}
}

func (tf *TextField) delete(n int) {
	for i := 0; i < n; i++ {
		idx := tf.start + tf.cur
		if idx > 0 {
			if idx < len(tf.text) {
				copy(tf.text[idx:], tf.text[idx+1:])
			}
			tf.text = tf.text[:len(tf.text)-1]
			if tf.cur > 0 {
				tf.cur--
			} else if tf.start > 0 {
				tf.start--
			}
		}
	}
}

func (tf *TextField) insert(tc f64.Rectangle, lw int, text string) {
	for _, ch := range text {
		idx := tf.start + tf.cur
		tf.text = append(tf.text[:idx], append([]rune{ch}, tf.text[idx:]...)...)
		xlw, xoob := tf.lineWidth(tc)
		if lw == xlw && xoob {
			if tf.cur < lw {
				tf.cur++
			} else {
				tf.start++
			}
		} else {
			tf.cur++
		}
		lw = xlw

		xlw, _ = tf.lineWidth(tc)
		if tf.cur > xlw {
			tf.start++
			tf.cur = xlw
		}
	}
}

func (tf *TextField) deleteSelection(tc f64.Rectangle) {
	if tf.sel.X == tf.sel.Y {
		return
	}

	lx, ly := tf.getSel()
	tf.start = 0
	for i := 0; i < ly-lx; i++ {
		tf.start = 0
		tf.cur = ly - i
		tf.delete(1)
	}

	tf.sel.X, tf.sel.Y = -1, -1
}

func (tf *TextField) frameEvent(dc, tc f64.Rectangle) {
	m := &ui.Mouse
	if !m.In(dc) {
		return
	}

	lw, _ := tf.lineWidth(tc)
	if m.Press == 1 && m.Button == 1 {
		tf.cur = tf.linePos(m.X)
		tf.cur = mathutil.Clamp(tf.cur, 0, lw)

		r := tc
		r.Max.X = m.X
		slw, oob := tf.lineWidth(r)

		switch {
		case tf.selected:
			tf.sel.X, tf.sel.Y = -1, -1
			tf.selected = false
		case tf.sel.X == -1:
			tf.sel.X = tf.start + slw
		case tf.sel.X != slw:
			tf.sel.Y = tf.start + slw
		}

		if tf.sel.X != -1 && tf.sel.Y != -1 {
			if m.X <= tc.Min.X+10 {
				tf.moveLeft(5)
			} else if oob {
				tf.moveRight(lw, 5)
			}
		}
	} else if m.Press == 2 {
		if tf.sel.Y == -1 {
			tf.sel.X, tf.sel.Y = -1, -1
			tf.selected = false
		} else {
			tf.selected = true
		}
	}

	k := &ui.Key
	if k.Press == 1 {
		clearSel := true
		switch k.Sym {
		case sdl.K_LEFT:
			n := 1
			if k.Mod&sdl.KMOD_CTRL != 0 {
				for i := tf.start + tf.cur - 1; i > 0; i-- {
					ch := tf.text[i]
					if !(unicode.IsLetter(ch) || unicode.IsNumber(ch)) {
						break
					}
					n++
				}
			}
			tf.moveLeft(n)
		case sdl.K_RIGHT:
			n := 1
			if k.Mod&sdl.KMOD_CTRL != 0 {
				for i := tf.start + tf.cur; i < len(tf.text); i++ {
					ch := tf.text[i]
					if !(unicode.IsLetter(ch) || unicode.IsNumber(ch)) {
						break
					}
					n++
				}
			}
			tf.moveRight(lw, n)
		case sdl.K_BACKSPACE:
			if tf.sel.X == tf.sel.Y {
				tf.delete(1)
			} else {
				tf.deleteSelection(tc)
			}
		case sdl.K_a:
			if k.Mod&sdl.KMOD_CTRL != 0 {
				tf.start = 0
				tf.cur = 0
			} else if k.Mod&sdl.KMOD_GUI != 0 {
				tf.sel.X = 0
				tf.sel.Y = len(tf.text)
				clearSel = false
				ui.Text = ""
			}
		case sdl.K_e:
			if k.Mod&sdl.KMOD_CTRL != 0 {
				tf.start = 0
				tf.cur = lw
				if lw < len(tf.text) {
					tf.start = len(tf.text) - lw
				}
				lw, _ = tf.lineWidth(tc)
			}
		case sdl.K_l:
			if k.Mod&sdl.KMOD_CTRL != 0 {
				tf.start = 0
				tf.cur = 0
				tf.text = tf.text[:0]
			}
		case sdl.K_u:
			if k.Mod&sdl.KMOD_CTRL != 0 {
				tf.text = tf.text[tf.cur:]
				tf.start = 0
				tf.cur = 0
			}
		case sdl.K_w:
			n := 1
			if k.Mod&sdl.KMOD_CTRL != 0 {
				for i := tf.start + tf.cur - 1; i > 0; i-- {
					ch := tf.text[i]
					if unicode.IsSpace(ch) {
						break
					}
					n++
				}
				tf.delete(n)
			}
		case sdl.K_x:
			if k.Mod&sdl.KMOD_CTRL != 0 {
				lx, ly := tf.getSel()
				if lx != -1 && ly != -1 {
					text := string(tf.text[lx:ly])
					sdl.SetClipboardText(text)
					tf.deleteSelection(tc)
				}
			}
		case sdl.K_c:
			if k.Mod&sdl.KMOD_CTRL != 0 {
				lx, ly := tf.getSel()
				if lx != -1 && ly != -1 {
					text := string(tf.text[lx:ly])
					sdl.SetClipboardText(text)
					clearSel = false
				}
			}
		case sdl.K_v:
			if tf.editable && k.Mod&sdl.KMOD_CTRL != 0 {
				if sdl.HasClipboardText() {
					text := sdl.GetClipboardText()
					tf.insert(tc, lw, text)
				}
			}
		default:
			clearSel = false
		}
		if ui.Text != "" {
			clearSel = false
		}
		if clearSel {
			tf.sel.X, tf.sel.Y = -1, -1
		}
	}

	if tf.editable {
		if ui.Text != "" {
			tf.deleteSelection(tc)
			tf.insert(tc, lw, ui.Text)
		}
	}
}

func (tf *TextField) lineWidth(tc f64.Rectangle) (lw int, oob bool) {
	dx := tc.Dx()
	lpw := 0.0
	for i := tf.start; i < len(tf.text); i++ {
		xadv, _, _, _, _ := vg.TextBounds(lpw, 0, string(tf.text[i]), -1)
		if lpw+xadv >= dx {
			oob = true
			break
		}
		lw++
		lpw += xadv
	}
	return
}

func (tf *TextField) linePos(x float64) int {
	ox := 0.0
	for i := tf.start; i < len(tf.text); i++ {
		xadv, _, _, xmax, _ := vg.TextBounds(ox, 0, string(tf.text[i]), -1)
		if x <= xmax {
			return i - tf.start
		}
		ox += xadv
	}
	return len(tf.text) - tf.start
}

func (tf *TextField) getSel() (lx, ly int) {
	lx, ly = tf.sel.X, tf.sel.Y
	if ly < lx {
		return ly, lx
	}
	return lx, ly
}
