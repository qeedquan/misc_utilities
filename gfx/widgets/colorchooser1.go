package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	"log"
	"os"
	"runtime"

	"github.com/qeedquan/go-media/image/chroma"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
)

var (
	window   *sdl.Window
	renderer *sdl.Renderer
	ui       *UI
	clch     *ColorChooser
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

	window.SetTitle("Color Chooser")
	sdlgfx.SetFont(sdlgfx.Font10x20, 10, 20)

	sdl.StartTextInput()

	ui = NewUI()
	clch = NewColorChooser(ColorChooserOptions{
		Pos: image.Pt(100, 100),
	})
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
	renderer.SetDrawColor(color.RGBA{34, 43, 47, 255})
	renderer.Clear()
	clch.Blit()
	renderer.Present()
}

func fillRect(r image.Rectangle, c color.RGBA) {
	s := sdl.Recti(r)
	renderer.SetDrawColor(c)
	renderer.FillRect(&s)
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

type ColorChooser struct {
	ColorChooserOptions
	Cursor image.Point
	Fixed  int
	HSV    chroma.HSV
	RGB    color.RGBA
	Cw, Ch int
}

type ColorChooserOptions struct {
	Pos image.Point
}

func NewColorChooser(o ColorChooserOptions) *ColorChooser {
	rgb := color.RGBA{0, 0, 255, 255}
	hsv := chroma.RGB2HSV(rgb)
	return &ColorChooser{
		ColorChooserOptions: o,
		Fixed:               'v',
		RGB:                 rgb,
		HSV:                 hsv,
		Cursor:              image.Pt(int(hsv.H*360.0), 0),
		Cw:                  360,
		Ch:                  256,
	}
}

func (c *ColorChooser) Blit() {
	w, h := c.Cw, c.Ch
	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			_, rgb := c.sg(x, y)
			renderer.SetDrawColor(rgb)
			renderer.DrawPoint(c.Pos.X+x, c.Pos.Y+y)
		}
	}
	x := c.Pos.X + c.Cursor.X
	y := c.Pos.Y + c.Cursor.Y
	rad := 5
	blitCircle(x, y, rad, color.RGBA{255, 255, 255, 255})

	x = c.Pos.X + w + 64
	y = c.Pos.Y
	for i := 0; i < h; i++ {
		rgb := c.pg(i)
		r := image.Rect(x, y+i, x+100, y+i+1)
		fillRect(r, rgb)
	}
	r := image.Rect(x, y, x+100, y+h)
	if ui.mouse.hit == 1 && ui.mouse.In(r) {
		c.sy(ui.mouse.Y - y)
	}

	s := 30
	x = c.Pos.X + w + 64 - s/2
	y = c.Pos.Y
	blitLine(x, y+c.ly(), x+s/2, y+c.ly(), color.RGBA{34, 134, 185, 255})

	x = c.Pos.X
	y = c.Pos.Y + h + 32
	c.text(x, y, 'h')
	c.text(x, y, 's')
	c.text(x, y, 'v')
	c.text(x, y, 'r')
	c.text(x, y, 'g')
	c.text(x, y, 'b')

	r = image.Rect(c.Pos.X, c.Pos.Y, c.Pos.X+w, c.Pos.Y+h)
	if ui.mouse.hit == 1 && ui.mouse.In(r) {
		c.Cursor.X = ui.mouse.X - c.Pos.X
		c.Cursor.Y = ui.mouse.Y - c.Pos.Y

		c.HSV, c.RGB = c.sg(c.Cursor.X, c.Cursor.Y)
	}
}

func (c *ColorChooser) text(x, y, ref int) {
	hsv := c.HSV
	rgb := c.RGB
	_, fh, _ := sdlgfx.FontMetrics()
	tc := color.RGBA{200, 200, 200, 255}

	var text string
	switch ref {
	case 'h':
		text = fmt.Sprintf("Hue:        %.0f", hsv.H)
	case 's':
		y += fh * 1
		text = fmt.Sprintf("Saturation: %.0f", hsv.S*255)
	case 'v':
		y += fh * 2
		text = fmt.Sprintf("Value:      %.0f", hsv.V*255)
	case 'r':
		x += 250
		text = fmt.Sprintf("Red:    %v", rgb.R)
	case 'g':
		x += 250
		y += fh * 1
		text = fmt.Sprintf("Green:  %v", rgb.G)
	case 'b':
		x += 250
		y += fh * 2
		text = fmt.Sprintf("Blue:   %v", rgb.B)
	}
	blitText(x, y, tc, "%s", text)

	cx := x - 10
	cy := y + 10
	rad := 5

	in := false
	px := cx - ui.mouse.X
	py := cy - ui.mouse.Y
	if px*px+py*py <= rad*rad {
		in = true
	}

	if c.Fixed == ref {
		fillCircle(cx, cy, rad, color.RGBA{100, 20, 185, 255})
	} else if in {
		blitCircle(cx, cy, rad, color.RGBA{250, 255, 255, 255})
	} else {
		blitCircle(cx, cy, rad, color.RGBA{250, 30, 40, 255})
	}

	if ui.mouse.hit == 1 && in {
		c.Fixed = ref
	}
}

func (c *ColorChooser) sg(x, y int) (hsv chroma.HSV, rgb color.RGBA) {
	hsv = c.HSV
	rgb = c.RGB
	switch c.Fixed {
	case 'h':
		hsv.S = float64(x) / float64(c.Ch)
		hsv.V = float64(c.Ch-y) / float64(c.Ch)
		rgb = chroma.HSV2RGB(hsv)
	case 's':
		hsv.H = float64(x)
		hsv.V = float64(c.Ch-y) / float64(c.Ch)
		rgb = chroma.HSV2RGB(hsv)
	case 'v':
		hsv.H = float64(x)
		hsv.S = float64(c.Ch-y) / float64(c.Ch)
		rgb = chroma.HSV2RGB(hsv)
	case 'r':
		rgb.G = uint8(float64(x) / float64(c.Cw) * 255)
		rgb.B = uint8(float64(c.Ch-y) / float64(c.Ch) * 255)
		hsv = chroma.RGB2HSV(rgb)
	case 'g':
		rgb.R = uint8(float64(x) / float64(c.Cw) * 255)
		rgb.B = uint8(float64(c.Ch-y) / float64(c.Ch) * 255)
		hsv = chroma.RGB2HSV(rgb)
	case 'b':
		rgb.R = uint8(float64(x) / float64(c.Cw) * 255)
		rgb.G = uint8(float64(c.Ch-y) / float64(c.Ch) * 255)
		hsv = chroma.RGB2HSV(rgb)
	}
	return
}

func (c *ColorChooser) pg(y int) color.RGBA {
	hsv := c.HSV
	rgb := c.RGB
	switch c.Fixed {
	case 'h':
		hsv.H = float64(y) / float64(c.Ch)
		rgb = chroma.HSV2RGB(hsv)
	case 's':
		hsv.S = 1 - (float64(y) / float64(c.Ch))
		rgb = chroma.HSV2RGB(hsv)
	case 'v':
		hsv.V = 1 - (float64(y) / float64(c.Ch))
		rgb = chroma.HSV2RGB(hsv)
	case 'r':
		rgb.R = uint8(clamp(y, 0, 255))
	case 'g':
		rgb.G = uint8(clamp(y, 0, 255))
	case 'b':
		rgb.B = uint8(clamp(y, 0, 255))
	}
	return rgb
}

func (c *ColorChooser) ly() int {
	hsv := c.HSV
	rgb := c.RGB

	var y int
	switch c.Fixed {
	case 'h':
		y = int(float64(hsv.H) * float64(c.Ch))
	case 's':
		y = int((1 - hsv.S) * float64(c.Ch))
	case 'v':
		y = int((1 - hsv.V) * float64(c.Ch))
	case 'r':
		y = int(float64(rgb.R) / 255.0 * float64(c.Ch))
	case 'g':
		y = int(float64(rgb.G) / 255.0 * float64(c.Ch))
	case 'b':
		y = int(float64(rgb.B) / 255.0 * float64(c.Ch))
	}
	return y
}

func (c *ColorChooser) sy(y int) {
	hsv := c.HSV
	rgb := c.RGB

	switch c.Fixed {
	case 'h':
		hsv.H = float64(y) / float64(c.Ch)
	case 's':
		hsv.S = 1 - (float64(y) / float64(c.Ch))
	case 'v':
		hsv.V = 1 - (float64(y) / float64(c.Ch))
	case 'r':
		rgb.R = uint8(float64(y) / float64(c.Ch) * 255)
	case 'g':
		rgb.G = uint8(float64(y) / float64(c.Ch) * 255)
	case 'b':
		rgb.B = uint8(float64(y) / float64(c.Ch) * 255)
	}

	c.HSV = hsv
	c.RGB = rgb
}
