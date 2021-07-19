package main

import (
	"flag"
	"fmt"
	"image/color"
	"log"
	"math"
	"os"
	"runtime"

	"github.com/go-gl/gl/v3.3-core/gl"
	"github.com/qeedquan/go-media/glew"
	"github.com/qeedquan/go-media/image/ttf"
	"github.com/qeedquan/go-media/math/f64"
	"github.com/qeedquan/go-media/nvg"
	"github.com/qeedquan/go-media/sdl"
)

var (
	window      *sdl.Window
	vg          *nvg.Context
	ui          *UI
	imageViewer *ImageViewer
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("imageviewer: ")
	runtime.LockOSThread()
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}
	initGUI()
	for {
		ui.Begin()
		event()
		frame()
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: imageviewer file")
	flag.PrintDefaults()
	os.Exit(2)
}

func initGUI() {
	err := sdl.Init(sdl.INIT_VIDEO)
	ck(err)

	sdl.GLSetAttribute(sdl.GL_CONTEXT_MAJOR_VERSION, 3)
	sdl.GLSetAttribute(sdl.GL_CONTEXT_MINOR_VERSION, 3)

	window, err = sdl.CreateWindow("Image Viewer",
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
	imageViewer = NewImageViewer()
	ck(imageViewer.Load(flag.Arg(0)))

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
	gl.ClearColor(0.086275, 0.086275, 0.086275, 1)
	gl.Clear(gl.COLOR_BUFFER_BIT | gl.STENCIL_BUFFER_BIT | gl.DEPTH_BUFFER_BIT)
	gl.Viewport(0, 0, int32(w), int32(h))
	vg.BeginFrame(float64(w), float64(h), 1)
	vp := f64.Rect(0, 0, float64(w), float64(h))
	imageViewer.Frame(vp)
	vg.EndFrame()
	window.SwapGL()
}

const (
	COLOR_SCROLL_BG = iota
	COLOR_SCROLL
	COLOR_SCROLL_HOVER
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
				COLOR_SCROLL_BG:    color.RGBA{200, 222, 200, 255},
				COLOR_SCROLL:       color.RGBA{40, 80, 200, 255},
				COLOR_SCROLL_HOVER: color.RGBA{100, 100, 200, 255},
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

type ImageViewer struct {
	img  int
	scr  f64.Vec2
	mos  f64.Vec2
	zoom f64.Vec2
	drag bool
}

func NewImageViewer() *ImageViewer {
	return &ImageViewer{
		img:  -1,
		zoom: f64.Vec2{1, 1},
	}
}

func (iv *ImageViewer) Load(name string) error {
	img, err := vg.CreateImage(name, 0)
	if err != nil {
		return err
	}

	if iv.img != -1 {
		vg.DeleteImage(iv.img)
	}
	iv.img = img
	iv.scr = f64.Vec2{}
	return nil
}

func (iv *ImageViewer) Frame(dc f64.Rectangle) {
	vg.Save()
	defer vg.Restore()

	const scrollSize = 10.0

	st := &ui.Style
	viw, vih := vg.ImageSize(iv.img)
	iw := float64(viw) * iv.zoom.X
	ih := float64(vih) * iv.zoom.Y

	x, y, w, h := dc.PosSize()

	var rx, ry f64.Rectangle
	var rx2, ry2 float64
	if ih > h {
		c := st.Colors[COLOR_SCROLL_BG]
		xx := x + w - scrollSize
		yy := y
		ww := scrollSize
		hh := h
		ry = f64.Rect(xx, yy, xx+ww, yy+hh)

		vg.BeginPath()
		vg.FillColor(c)
		vg.RoundedRect(xx, yy, ww, hh, 8)
		vg.Fill()

		c = st.Colors[COLOR_SCROLL]
		yy = iv.scr.Y * h
		hh = (h / ih) * h
		if yy+hh > h {
			yy = h - hh
			iv.scr.Y = yy / h
		}
		vg.BeginPath()
		vg.FillColor(c)
		vg.RoundedRect(xx, yy, ww, hh, 8)
		vg.Fill()

		w -= ww
		ry2 = hh / 2
	}

	if iw > w {
		c := st.Colors[COLOR_SCROLL_BG]
		xx := x
		yy := y + h - scrollSize
		ww := w
		hh := scrollSize
		rx = f64.Rect(xx, yy, xx+ww, yy+hh)

		vg.BeginPath()
		vg.FillColor(c)
		vg.RoundedRect(xx, yy, ww, hh, 8)
		vg.Fill()

		c = st.Colors[COLOR_SCROLL]
		xx = iv.scr.X * w
		ww = (w / iw) * w
		if xx+ww > w {
			xx = w - ww
			iv.scr.X = xx / w
		}
		vg.BeginPath()
		vg.FillColor(c)
		vg.RoundedRect(xx, yy, ww, hh, 8)
		vg.Fill()

		h -= hh
		rx2 = ww / 2
	}

	x = x + math.Max((w-iw)/2, 0)
	y = y + math.Max((h-ih)/2, 0)
	w = math.Min(w, iw)
	h = math.Min(h, ih)
	r := f64.Rect(x, y, x+w, y+h)

	px := iv.scr.X * iw
	py := iv.scr.Y * ih
	if px+w > iw {
		px = iw - w
	}
	if py+h > ih {
		py = ih - h
	}
	vg.BeginPath()
	p := vg.ImagePattern(x-px, y-py, iw, ih, 0, iv.img, 1)
	vg.Rect(x, y, w, h)
	vg.FillPaint(p)
	vg.Fill()
	vg.Scale(iv.zoom.X, iv.zoom.Y)

	m := &ui.Mouse
	if m.Press == 1 && m.Button&1 != 0 {
		if m.In(rx) {
			iv.scr.X = (m.X - rx2) / rx.Dx()
		} else if m.In(ry) {
			iv.scr.Y = (m.Y - ry2) / ry.Dy()
		}
		iv.drag = true
	} else if m.Press == 2 {
		iv.drag = false
	}

	if m.Wheel.Y != 0 {
		step := 40.0 / h * m.Wheel.Y
		iv.scr.Y -= step
	}

	k := &ui.Key
	if k.Press == 1 {
		step := f64.Vec2{20.0 / w, 20.0 / h}
		switch k.Sym {
		case sdl.K_LEFT:
			iv.scr.X -= step.X
		case sdl.K_RIGHT:
			iv.scr.X += step.X
		case sdl.K_UP:
			iv.scr.Y -= step.Y
		case sdl.K_DOWN:
			iv.scr.Y += step.Y
		case sdl.K_1:
			iv.zoom.X += 0.05
			iv.zoom.Y += 0.05
		case sdl.K_2:
			iv.zoom.X -= 0.05
			iv.zoom.Y -= 0.05
		}
	}

	if iv.drag && m.In(r) {
		const threshold = 2.0
		step := f64.Vec2{4.0 / w, 4.0 / h}
		if math.Abs(iv.mos.X-m.X) >= threshold {
			if iv.mos.X < m.X {
				iv.scr.X -= step.X
			} else {
				iv.scr.X += step.X
			}
		}
		if math.Abs(iv.mos.Y-m.Y) >= threshold {
			if iv.mos.Y < m.Y {
				iv.scr.Y -= step.Y
			} else {
				iv.scr.Y += step.Y
			}
		}
	}

	iv.scr = iv.scr.Clamp(0, 1)
	iv.mos = m.Vec2
}
