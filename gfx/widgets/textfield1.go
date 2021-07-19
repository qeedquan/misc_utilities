package main

import (
	"flag"
	"fmt"
	"image"
	"io"
	"log"
	"os"
	"runtime"

	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
)

const (
	LINEWIDTH = 16
	MAX_CHARS = 256
)

var (
	window    *sdl.Window
	renderer  *sdl.Renderer
	ui        *UI
	textField []*TextField
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

	window.SetTitle("Text Field")
	sdlgfx.SetFont(sdlgfx.Font10x20, 10, 20)

	sdl.StartTextInput()

	ui = NewUI()
	for y := 0; y < 768; y += 32 {
		for x := 0; x <= 1280; x += LINEWIDTH * 10 * 2 {
			textField = append(textField, NewTextField(x, y))
		}
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
	renderer.SetDrawColor(sdl.Color{34, 43, 47, 255})
	renderer.Clear()
	for _, tf := range textField {
		tf.Blit()
	}
	renderer.Present()
}

func fillRect(r image.Rectangle, c sdl.Color) {
	s := sdl.Recti(r)
	renderer.SetDrawColor(c)
	renderer.FillRect(&s)
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

type TextField struct {
	// location of the textfield in pixels for drawing
	Pos image.Point

	// boundary for text selection
	// 0 <= select <= len(text)
	Select image.Point

	// select hilighting boundary
	// 0 <= hilight <= linewidth
	Hilight image.Point

	// did we already hilight
	Hilighted bool

	// maximum viewable characters in a textfield
	Linewidth int

	// maximum character limit
	MaxChars int

	// cursor position in the text
	// 0 <= cursor <= linewidth
	Cursor int

	// the text string
	// len(text) <= maxchars
	Text []rune

	// clipboard
	Clipboard string

	// location of the viewable text portion
	// 0 <= loc <= len(text)
	// 0 <= loc.y-loc.x <= linewidth
	Loc image.Point

	// editable or readonly
	Editable bool
}

func NewTextField(x, y int) *TextField {
	text := fmt.Sprintf("%d,%d", x, y)
	return &TextField{
		Pos:       image.Pt(x, y),
		Linewidth: LINEWIDTH,
		MaxChars:  MAX_CHARS,
		Select:    image.Pt(-1, -1),
		Hilight:   image.Pt(-1, -1),
		Text:      []rune(text),
		Loc:       image.Pt(0, len(text)),
		Cursor:    len(text),
		Editable:  true,
	}
}

func (tf *TextField) selectOp(reset, delete bool) string {
	var s string

	if tf.Select.X != -1 && tf.Select.Y != -1 {
		p := pointCanon(tf.Select)
		s = string(tf.Text[p.X:p.Y])
		if delete {
			tf.MoveCursor(p.Y, io.SeekStart)
			tf.Delete(p.Y - p.X)
		}
	}
	if reset {
		tf.Select = image.Pt(-1, -1)
		tf.Hilight = image.Pt(-1, -1)
		tf.Hilighted = false
	}
	return s
}

func (tf *TextField) Blit() {
	fontWidth, fontHeight, _ := sdlgfx.FontMetrics()
	framePad := fontHeight / 2
	frame := image.Rect(
		tf.Pos.X,
		tf.Pos.Y,
		tf.Pos.X+tf.Linewidth*fontWidth+framePad,
		tf.Pos.Y+fontHeight+framePad,
	)

	// textfield frame
	if ui.mouse.In(frame) {
		fillRect(frame, sdl.Color{120, 120, 120, 255})
	} else {
		fillRect(frame, sdl.Color{85, 91, 94, 255})
	}

	textFrame := image.Pt(tf.Pos.X+framePad/2, tf.Pos.Y+framePad/2)

	// hilight and cursor
	cursorPos := textFrame
	if tf.Hilight.X == -1 || tf.Hilight.Y == -1 {
		cursorPos.X += tf.Cursor * fontWidth
		blitLine(cursorPos.X, cursorPos.Y, cursorPos.X, cursorPos.Y+fontHeight, sdl.Color{255, 255, 255, 255})
	} else {
		hilight := image.Rect(
			cursorPos.X+tf.Hilight.X*fontWidth,
			cursorPos.Y,
			cursorPos.X+tf.Hilight.Y*fontWidth,
			cursorPos.Y+fontHeight,
		)
		fillRect(hilight, sdl.Color{42, 44, 135, 255})
	}

	// text
	blitText(textFrame.X, textFrame.Y, sdl.Color{229, 229, 229, 255}, "%s", string(tf.Text[tf.Loc.X:tf.Loc.Y]))

	// events
	if !ui.mouse.In(frame) {
		return
	}

	if ui.kbd.hit == 1 {
		switch ui.kbd.key {
		case sdl.K_a:
			if ui.kbd.mod&sdl.KMOD_CTRL != 0 {
				tf.MoveCursor(0, io.SeekStart)
			} else if ui.kbd.mod&sdl.KMOD_GUI != 0 {
				tf.Select.X = 0
				tf.Select.Y = len(tf.Text)
				tf.Hilight.X = 0
				tf.Hilight.Y = min(len(tf.Text), tf.Linewidth)
				tf.Hilighted = true
			}

		case sdl.K_u:
			if ui.kbd.mod&sdl.KMOD_CTRL != 0 {
				tf.MoveCursor(0, io.SeekEnd)
				tf.Delete(len(tf.Text))
				tf.selectOp(true, false)
			}

		case sdl.K_e:
			if ui.kbd.mod&sdl.KMOD_CTRL != 0 {
				tf.MoveCursor(0, io.SeekEnd)
			}

		case sdl.K_x, sdl.K_c:
			if ui.kbd.mod&sdl.KMOD_CTRL != 0 && tf.Select.X != -1 && tf.Select.Y != -1 {
				tf.Clipboard = tf.selectOp(ui.kbd.key == sdl.K_x, ui.kbd.key == sdl.K_x)
			}

		case sdl.K_v:
			if ui.kbd.mod&sdl.KMOD_CTRL != 0 && tf.Clipboard != "" {
				tf.selectOp(true, true)
				tf.Insert(tf.Clipboard)
			}

		case sdl.K_BACKSPACE:
			if tf.Select.X == -1 || tf.Select.Y == -1 {
				tf.Delete(1)
			} else {
				tf.selectOp(true, true)
			}

		case sdl.K_LEFT:
			tf.selectOp(true, false)
			if ui.kbd.mod&sdl.KMOD_CTRL != 0 {
				tf.MoveCursor(tf.Loc.X, io.SeekStart)
			} else {
				tf.MoveCursor(-1, io.SeekCurrent)
			}

		case sdl.K_RIGHT:
			tf.selectOp(true, false)
			if ui.kbd.mod&sdl.KMOD_CTRL != 0 {
				tf.MoveCursor(tf.Loc.X+min(tf.Loc.Y-tf.Loc.X, tf.Linewidth), io.SeekStart)
			} else {
				tf.MoveCursor(1, io.SeekCurrent)
			}
		}
	}

	if ui.text != "" && ui.kbd.mod&sdl.KMOD_GUI == 0 {
		if tf.Select.X != -1 && tf.Select.Y != -1 {
			tf.selectOp(true, true)
		}
		tf.Insert(ui.text)
	}

	switch {
	case ui.mouse.hit == 1 && ui.mouse.button == 1:
		pos := ui.mouse.Sub(textFrame)
		cursor := min(pos.X/fontWidth, tf.Loc.Y, tf.Linewidth)
		tf.MoveCursor(cursor-tf.Cursor, io.SeekCurrent)

		switch {
		case tf.Hilighted:
			tf.selectOp(true, false)

		case tf.Select.X == -1:
			tf.Select.X = tf.Loc.X + cursor
			tf.Hilight.X = cursor

		case tf.Hilight.X != cursor:
			switch {
			case tf.Hilight.Y == 0:
				tf.MoveCursor(-1, io.SeekCurrent)
				tf.Select.Y = tf.Loc.X

				if tf.Loc.Y-tf.Loc.X >= tf.Linewidth {
					tf.Hilight.X = clamp(tf.Hilight.X+1, 0, min(tf.Linewidth, len(tf.Text)))
				}

			case tf.Hilight.Y == tf.Linewidth:
				tf.MoveCursor(1, io.SeekCurrent)
				tf.Select.Y = tf.Loc.Y
				tf.Hilight.X = clamp(tf.Hilight.X-1, 0, tf.Linewidth)
				tf.Hilight.Y = tf.Cursor

			default:
				tf.Select.Y = tf.Loc.X + cursor
				tf.Hilight.Y = cursor
			}
		}

	case ui.mouse.hit == 2:
		if tf.Hilight.Y == -1 {
			tf.selectOp(true, false)
		} else {
			tf.Hilighted = true
		}
	}
}

func (tf *TextField) Insert(text string) {
	if !tf.Editable {
		return
	}

	for _, ch := range text {
		if len(tf.Text) >= tf.MaxChars {
			break
		}

		loc := tf.Loc.X + tf.Cursor
		tf.Text = append(tf.Text[:loc], append([]rune{ch}, tf.Text[loc:]...)...)

		switch {
		case tf.Loc.Y >= tf.Linewidth && tf.Cursor == tf.Linewidth:
			tf.Loc.X++
			tf.Loc.Y++

		case (tf.Loc.Y >= tf.Linewidth && tf.Cursor < tf.Linewidth) || tf.Loc.Y < tf.Linewidth:
			if tf.Loc.Y-tf.Loc.X >= tf.Linewidth {
				tf.Loc.X = tf.Loc.Y - tf.Linewidth
				tf.Cursor++
			} else {
				tf.Loc.Y++
				tf.Cursor++
				tf.Cursor = clamp(tf.Cursor, 0, tf.Linewidth)
			}

		case tf.Cursor < tf.Linewidth:
			tf.Cursor++
		}

		tf.Loc.X = clamp(tf.Loc.X, 0, len(tf.Text))
		tf.Loc.Y = clamp(tf.Loc.Y, 0, len(tf.Text))
	}
}

func (tf *TextField) Delete(n int) {
	if !tf.Editable {
		return
	}

	from := tf.Loc.X + tf.Cursor
	to := clamp(from-n, 0, len(tf.Text))
	if to < from {
		from, to = to, from
	}
	copy(tf.Text[from:], tf.Text[to:])
	tf.Text = tf.Text[:len(tf.Text)-(to-from)]

	tf.Cursor -= (to - from)
	if tf.Cursor < 0 {
		tf.Cursor = min(tf.Loc.X-1, len(tf.Text))
		tf.Cursor = clamp(tf.Cursor, 0, tf.Linewidth)
		tf.Loc.X = clamp(tf.Loc.X-tf.Linewidth, 0, len(tf.Text))
		tf.Loc.Y = clamp(tf.Loc.X+min(tf.Linewidth, len(tf.Text)), 0, len(tf.Text))
	} else if tf.Loc.Y > len(tf.Text) {
		tf.Loc.Y = len(tf.Text)
	}
}

func (tf *TextField) MoveCursor(offset, whence int) {
	loc := tf.Loc.X + tf.Cursor
	switch whence {
	case io.SeekStart:
		loc = offset
	case io.SeekCurrent:
		loc += offset
	case io.SeekEnd:
		loc = len(tf.Text) + offset
	default:
		panic(fmt.Errorf("textfield: unknown seek type %d", whence))
	}
	loc = clamp(loc, 0, len(tf.Text))

	switch {
	case loc < tf.Loc.X:
		tf.Loc.X = loc
		if tf.Loc.Y-tf.Loc.X >= tf.Linewidth {
			tf.Loc.Y = tf.Loc.X + min(tf.Linewidth, len(tf.Text))
		} else {
			tf.Loc.Y = tf.Loc.Y - loc
		}
		tf.Cursor = 0

	case loc > tf.Loc.Y:
		tf.Loc.X += loc - tf.Loc.Y
		tf.Loc.Y = loc
		tf.Cursor = tf.Linewidth

	default:
		tf.Cursor = loc - tf.Loc.X
	}
}
