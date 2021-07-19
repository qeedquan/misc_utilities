package main

import (
	"flag"
	"image/color"
	"log"
	"math"
	"os"
	"path/filepath"
	"runtime"
	"sort"
	"strings"

	"github.com/go-gl/gl/v3.3-core/gl"
	"github.com/qeedquan/go-media/glew"
	"github.com/qeedquan/go-media/image/ttf"
	"github.com/qeedquan/go-media/math/f64"
	"github.com/qeedquan/go-media/math/mathutil"
	"github.com/qeedquan/go-media/nvg"
	"github.com/qeedquan/go-media/sdl"
)

var (
	window     *sdl.Window
	vg         *nvg.Context
	ui         *UI
	fileViewer *FileViewer
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("fileviewer: ")
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

	window, err = sdl.CreateWindow("File Viewer",
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
	fileViewer = NewFileViewer()

	window.SetMinimumSize(512, 512)
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
	dc := f64.Rect(0, 0, float64(w)*5/6, float64(h)*5/6)
	fileViewer.Frame(dc)
	vg.EndFrame()
	window.SwapGL()
}

type UI struct {
	Mouse struct {
		f64.Vec2
		Button uint8
		Press  uint8
		Wheel  f64.Vec2
	}
	Key struct {
		Sym   sdl.Keycode
		Press uint8
	}
}

func NewUI() *UI {
	return &UI{}
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
	}
}

func (ui *UI) evKeyEvent(ev sdl.KeyboardEvent, press uint8) {
	k := &ui.Key
	k.Sym = ev.Sym
	k.Press = press
}

func (ui *UI) evMouseButton(ev sdl.MouseButtonEvent, press uint8) {
	m := &ui.Mouse
	m.X = float64(ev.X)
	m.Y = float64(ev.Y)
	m.Button = ev.Button
	m.Press = press
}

type FileViewer struct {
	cwd    string
	fontsz float64
	maxch  int
	fis    []os.FileInfo
	scr    f64.Vec2
	scrd   f64.Vec2
	drag   int
}

func NewFileViewer() *FileViewer {
	cwd, _ := os.Getwd()
	fv := &FileViewer{
		fontsz: 22,
	}
	fv.Chdir(cwd)
	return fv
}

func (fv *FileViewer) Chdir(dir string) error {
	if !filepath.IsAbs(dir) {
		dir = filepath.Join(fv.cwd, dir)
	}

	fis, err := readAllDir(dir)
	if err != nil {
		return err
	}
	fv.fis = fis
	fv.cwd = dir
	fv.maxch = -1
	n := 0
	for i, f := range fis {
		m := len(f.Name())
		if n < m {
			fv.maxch = i
			n = m
		}
	}
	fv.scr.X = 0
	fv.scr.Y = 0
	return nil
}

func (fv *FileViewer) Frame(dc f64.Rectangle) {
	fv.frameBG(dc)

	tp := dc.Inset(8)
	tp.Max.X -= 10
	fv.frameText(tp)

	vp := f64.Rect(dc.Min.X, dc.Max.Y-10, tp.Max.X, dc.Max.Y)
	fv.frameScroll(vp, tp, 0)

	vp = f64.Rect(vp.Max.X+5, dc.Min.Y, dc.Max.X, dc.Max.Y)
	fv.frameScroll(vp, tp, 1)

	m := &ui.Mouse
	k := &ui.Key
	in := m.In(dc)
	if !in {
		return
	}
	if (m.Button == 4 && m.Press == 2) || (k.Sym == sdl.K_BACKSPACE && k.Press == 2) {
		fv.Chdir("..")
	}
}

func (fv *FileViewer) frameBG(dc f64.Rectangle) {
	vg.Save()
	defer vg.Restore()

	x, y, w, h := dc.PosSize()
	vg.BeginPath()
	p := vg.BoxGradient(x, y, w, h, 5, 8, color.RGBA{0, 30, 100, 60}, color.RGBA{0, 0, 100, 192})
	vg.RoundedRect(x, y, w, h, 3)
	vg.FillPaint(p)
	vg.Fill()
}

func (fv *FileViewer) frameText(dc f64.Rectangle) {
	vg.Save()
	defer vg.Restore()

	x, y, w, h := dc.PosSize()
	vg.Scissor(x, y, w, h)
	vg.BeginPath()
	vg.FontSize(fv.fontsz)
	vg.FontFace("sans")
	vg.TextAlign(nvg.ALIGN_LEFT | nvg.ALIGN_TOP)

	var nf os.FileInfo
	m := &ui.Mouse

	td, lineh := fv.textDim()
	td = td.Add(dc.Min)

	py := int(dc.Dy() / lineh)
	ip := int(fv.scr.Y * td.Dy() / lineh)
	if len(fv.fis)-ip < py {
		ip -= py - (len(fv.fis) - ip)
		fv.scr.Y = float64(ip) * lineh / td.Dy()
		fv.scr.Y = f64.Clamp(fv.scr.Y, 0, 1)
	}
	ip = mathutil.Clamp(ip, 0, len(fv.fis))

	x, y = dc.Min.X, dc.Min.Y
	for _, f := range fv.fis[ip:] {
		r := f64.Rect(x, y, dc.Max.X, y+lineh)
		r.Max.Y = math.Min(r.Max.Y, dc.Max.Y)
		if ui.Mouse.In(r) && fv.drag == 0 && fv.scrd.Y == 0 {
			vg.FillColor(color.RGBA{0, 100, 192, 255})
			vg.Rect(r.Min.X, r.Min.Y, r.Dx(), r.Dy())
			vg.Fill()
			if m.Button == 1 && m.Press == 2 {
				nf = f
			}
		}

		text := f.Name()
		if f.IsDir() {
			text += "/"
		}

		sx := fv.scr.X * td.Dx()
		vg.FillColor(color.RGBA{255, 255, 255, 255})
		vg.Text(x-sx, y, text, -1)
		y += lineh
	}

	if nf != nil {
		fv.Chdir(nf.Name())
	}
}

func (fv *FileViewer) frameScroll(dc, tc f64.Rectangle, axis int) {
	var a, b float64
	td, lineh := fv.textDim()
	if axis == 0 {
		a = tc.Dx()
		b = td.Dx()
		if b <= a {
			return
		}
	} else {
		a = tc.Dy()
		b = td.Dy()
	}
	rt := 1.0
	if b != 0 {
		rt = f64.Clamp(a/b, 0, 1)
	}

	vg.Save()
	defer vg.Restore()

	x, y, w, h := dc.PosSize()
	dx, _, dw, dh := x, y, w, h
	p := vg.BoxGradient(x, y, w, h, 8, 8, color.RGBA{0, 0, 0, 32}, color.RGBA{0, 0, 0, 150})
	vg.BeginPath()
	vg.RoundedRect(x, y, w, h, 5)
	vg.FillPaint(p)
	vg.Fill()

	const minsz = 10
	if axis == 0 {
		w = math.Max(rt*dw, minsz)
		x += fv.scr.X * dw
		if x+w > dw {
			x = dw - w
		}
	} else {
		h = math.Max(rt*dh, minsz)
		y += fv.scr.Y * dh
		if y+h > dh {
			y = dh - h
		}
	}

	k := &ui.Key
	m := &ui.Mouse
	r := f64.Rect(x, y, x+w, y+h)
	if (m.In(r) && fv.drag == 0) || fv.drag == axis+1 {
		p = vg.BoxGradient(x, y, w, h, 8, 8, color.RGBA{50, 254, 25, 50}, color.RGBA{100, 40, 250, 100})
	} else {
		p = vg.BoxGradient(x, y, w, h, 8, 8, color.RGBA{0, 0, 100, 255}, color.RGBA{0, 0, 200, 255})
	}
	vg.BeginPath()
	vg.RoundedRect(x, y, w, h, 5)
	vg.FillPaint(p)
	vg.Fill()

	if axis == 0 {
		if k.Press == 1 {
			sx := 1 / 5.0
			switch k.Sym {
			case sdl.K_LEFT:
				fv.scr.X -= sx
			case sdl.K_RIGHT:
				fv.scr.X += sx
			}
		}
	} else {
		sy := lineh * 5 / td.Dy()
		fv.scr.Y -= sy * m.Wheel.Y
		if m.Button == 3 {
			if m.Press != 1 {
				fv.scrd.Y = 0
			} else if m.Press == 1 || fv.scrd.Y != 0 {
				if fv.scrd.Y == 0 {
					if m.Y >= y+h/2 {
						fv.scrd.Y = -1
					} else {
						fv.scrd.Y = 1
					}
				}
				fv.scr.Y -= sy * fv.scrd.Y
			}
		}

		if k.Press == 1 {
			switch k.Sym {
			case sdl.K_UP:
				fv.scr.Y -= sy
			case sdl.K_DOWN:
				fv.scr.Y += sy
			case sdl.K_PAGEUP:
				fv.scr.Y -= sy * 5
			case sdl.K_PAGEDOWN:
				fv.scr.Y += sy * 5
			case sdl.K_HOME:
				fv.scr.Y = 0
			case sdl.K_END:
				fv.scr.Y = 1
			}
		}
	}

	if m.In(dc) || fv.drag == axis+1 {
		if (m.Button == 1 || m.Button == 2) && m.Press == 1 {
			if fv.drag == 0 {
				fv.drag = axis + 1
			}

			if !m.In(r) && (fv.drag == 0 || fv.drag == axis+1) {
				if axis == 0 {
					fv.scr.X = (m.X - w/2) / dw
				} else {
					fv.scr.Y = (m.Y - h/2) / dh
				}
			}
		}
	}

	if axis == 0 {
		dx += fv.scr.X * dw
		if dx+w > dw {
			fv.scr.X = (dw - w) / dw
		}
	}
	fv.scr.X = f64.Clamp(fv.scr.X, 0, 1)
	fv.scr.Y = f64.Clamp(fv.scr.Y, 0, 1)

	if m.Press != 1 {
		fv.drag = 0
	}
}

func (fv *FileViewer) textDim() (td f64.Rectangle, lineh float64) {
	if fv.maxch < 0 {
		return
	}

	vg.BeginPath()
	vg.FontSize(fv.fontsz)
	vg.FontFace("sans")

	_, _, lineh = vg.TextMetrics()
	text := fv.fis[fv.maxch].Name()
	w, _, _, _, _ := vg.TextBounds(0, 0, text, -1)
	h := lineh * float64(len(fv.fis))

	return f64.Rect(0, 0, w, h), lineh
}

func readAllDir(name string) ([]os.FileInfo, error) {
	f, err := os.Open(name)
	if err != nil {
		return nil, err
	}
	defer f.Close()

	fis, err := f.Readdir(-1)
	if err != nil {
		return nil, err
	}

	sort.Slice(fis, func(i, j int) bool {
		return strings.ToLower(fis[i].Name()) < strings.ToLower(fis[j].Name())
	})

	return fis, nil
}
