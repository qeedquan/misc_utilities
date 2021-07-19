// based on imgui vslider
package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	"log"
	"math"
	"os"
	"runtime"

	"github.com/qeedquan/go-media/image/chroma"
	"github.com/qeedquan/go-media/math/f64"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
)

var (
	window   *sdl.Window
	renderer *sdl.Renderer
	ui       *UI
	vsliders []*VSlider
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

	const N = 18
	for y := 0; y < 4; y++ {
		for x := 0; x < N; x++ {
			values := []float64{0.0, 0.6, 0.35, 0.9, 0.7, 0.2, 0.0}
			hue := float64(x) / N
			style := map[string]color.RGBA{
				"frame_bg":           chroma.HSV2RGB(chroma.HSV{hue, 0.5, 0.5}),
				"frame_bg_hovered":   chroma.HSV2RGB(chroma.HSV{hue, 0.6, 0.5}),
				"frame_bg_active":    chroma.HSV2RGB(chroma.HSV{hue, 0.7, 0.5}),
				"slider_grab":        chroma.HSV2RGB(chroma.HSV{hue, 0.9, 0.9}),
				"slider_grab_active": chroma.VEC42RGBA(f64.Vec4{0.24, 0.52, 0.88, 1.00}),
				"text":               chroma.HSV2RGB(chroma.HSV{hue, 1, 1}),
			}
			vs := &VSlider{
				Style:     style,
				Pos:       image.Pt(100+75*x, 100+y*200),
				Size:      image.Pt(18, 160),
				Round:     x % 4,
				Value:     values[(y*N+x)%len(values)],
				Min:       0,
				Max:       1,
				Power:     1,
				Precision: 2,
			}
			vs.Geom.GrabMinSize = 10
			vsliders = append(vsliders, vs)
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
	renderer.SetDrawColor(color.RGBA{115, 140, 153, 255})
	renderer.Clear()
	for _, vs := range vsliders {
		vs.Blit()
	}
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

type VSlider struct {
	Label     string
	Pos       image.Point
	Size      image.Point
	Round     int
	Precision int
	Value     float64
	Min, Max  float64
	Power     float64
	Style     map[string]color.RGBA
	Geom      struct {
		GrabMinSize float64
	}
}

func (vs *VSlider) Blit() {
	frame := image.Rectangle{
		vs.Pos,
		vs.Pos.Add(vs.Size),
	}
	if ui.mouse.In(frame) {
		roundRect(frame, vs.Round, vs.Style["frame_bg_active"])
	} else {
		roundRect(frame, vs.Round, vs.Style["frame_bg"])
	}

	grabPad := 2.0
	nonLinear := vs.Power < 1-1e-5 || vs.Power > 1+1e-5
	horizontal := false
	sliderSize := 0.0
	if horizontal {
		sliderSize = float64(frame.Dx()) - grabPad*2
	} else {
		sliderSize = float64(frame.Dy()) - grabPad*2
	}
	var grabSize float64
	if vs.Precision != 0 {
		// float slider
		grabSize = math.Min(vs.Geom.GrabMinSize, sliderSize)
	} else {
		// integer slider
		grabSize = sliderSize/math.Abs(vs.Max-vs.Min) + 1
		grabSize = math.Max(grabSize, vs.Geom.GrabMinSize)
		grabSize = math.Min(grabSize, sliderSize)
	}
	sliderUsableSize := sliderSize - grabSize
	var sliderUsablePosMin, sliderUsablePosMax float64
	if horizontal {
		sliderUsablePosMin = float64(frame.Min.X) + grabPad + grabSize*0.5
		sliderUsablePosMax = float64(frame.Max.X) - grabPad - grabSize*0.5
	} else {
		sliderUsablePosMin = float64(frame.Min.Y) + grabPad + grabSize*0.5
		sliderUsablePosMax = float64(frame.Max.Y) - grabPad - grabSize*0.5
	}

	// log sliders that cross over sign boundary, want exponential increase
	// to be symmetric around 0
	linearZeroPos := 0.0
	if vs.Min*vs.Max < 0 {
		// different sign
		linearDistMinToZero := math.Pow(math.Abs(vs.Min), 1/vs.Power)
		linearDistMaxToZero := math.Pow(math.Abs(vs.Max), 1/vs.Power)
		linearZeroPos = linearDistMinToZero / (linearDistMinToZero + linearDistMaxToZero)
	} else {
		// same sign
		if vs.Min > 0 {
			linearZeroPos = 1
		}
	}

	// click on the slider
	if ui.mouse.In(frame) && ui.mouse.hit == 1 {
		var mousePos float64
		if horizontal {
			mousePos = float64(ui.mouse.X)
		} else {
			mousePos = float64(ui.mouse.Y)
		}
		var click float64
		if sliderUsableSize > 0 {
			click = f64.Clamp((mousePos-sliderUsablePosMin)/sliderUsableSize, 0, 1)
		}
		if !horizontal {
			click = 1 - click
		}

		var newValue float64
		if nonLinear {
			// account for log scale on both sides of zero
			if click < linearZeroPos {
				// negative
				a := 1 - (click / linearZeroPos)
				a = math.Pow(a, vs.Power)
				newValue = f64.Lerp(a, math.Min(vs.Max, 0), vs.Min)
			} else {
				a := click
				if math.Abs(linearZeroPos-1) > 1e-6 {
					a = (click - linearZeroPos) / (1 - linearZeroPos)
				}
				a = math.Pow(a, vs.Power)
				newValue = f64.Lerp(a, math.Max(vs.Min, 0), vs.Max)
			}
		} else {
			// linear
			newValue = f64.Lerp(click, vs.Min, vs.Max)
		}
		fmt.Println(newValue)
		newValue = f64.RoundPrec(newValue, vs.Precision)
		if vs.Value != newValue {
			vs.Value = newValue
		}
	}

	grab := vs.calcRatioFromValue(vs.Value, vs.Min, vs.Max, vs.Power, linearZeroPos)
	if !horizontal {
		grab = 1 - grab
	}
	grabPos := f64.Lerp(grab, sliderUsablePosMin, sliderUsablePosMax)
	var grabBB image.Rectangle
	if horizontal {
		grabBB = image.Rect(
			int(grabPos-grabSize*0.5),
			frame.Min.Y+int(grabPad),
			int(grabPos+grabSize*0.5),
			frame.Max.Y-int(grabPad),
		)
	} else {
		grabBB = image.Rect(
			frame.Min.X+int(grabPad),
			int(grabPos-grabSize*0.5),
			frame.Max.X-int(grabPad),
			int(grabPos+grabSize*0.5),
		)
	}
	if ui.mouse.In(frame) && ui.mouse.hit == 1 {
		roundRect(grabBB, vs.Round, vs.Style["slider_grab_active"])
	} else {
		roundRect(grabBB, vs.Round, vs.Style["slider_grab"])
	}

	if ui.mouse.In(frame) {
		format := fmt.Sprintf("%%1.%df", vs.Precision)
		text := fmt.Sprintf(format, vs.Value)
		blitText(ui.mouse.X+10, ui.mouse.Y, vs.Style["text"], "%s", text)
	}
}

func (vs *VSlider) calcRatioFromValue(v, min, max, power, linearZeroPos float64) float64 {
	if min == max {
		return 0
	}
	if min < max {
		v = f64.Clamp(v, min, max)
	} else {
		v = f64.Clamp(v, max, min)
	}

	nonLinear := power < 1-0.0001 || power > 1+0.00001
	if nonLinear {
		if v < 0 {
			f := 1 - (v-min)/(math.Min(0, max)-min)
			return 1 - math.Pow(f, 1/power)*linearZeroPos
		} else {
			f := (v - math.Max(0, min)) / (max - math.Max(0, min))
			return linearZeroPos + math.Pow(f, 1/power)*(1-linearZeroPos)
		}
	}
	return (v - min) / (max - min)
}
