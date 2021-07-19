// http://paulbourke.net/fractals/lsys/
// parses lsys format described by link above
package main

import (
	"bufio"
	"flag"
	"fmt"
	"image/color"
	"io"
	"log"
	"math"
	"os"
	"runtime"
	"strconv"
	"strings"

	"github.com/qeedquan/go-media/image/chroma"
	"github.com/qeedquan/go-media/math/f64"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
)

var (
	window   *sdl.Window
	renderer *sdl.Renderer
	texture  *sdl.Texture
	lsys     L
	iters    = 5
)

func main() {
	runtime.LockOSThread()
	log.SetFlags(0)
	log.SetPrefix("lsys: ")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}
	initSDL()
	run(flag.Arg(0))
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] file")
	flag.PrintDefaults()
	os.Exit(2)
}

func initSDL() {
	err := sdl.Init(sdl.INIT_VIDEO)
	ck(err)

	w, h := 1280, 800
	wflag := sdl.WINDOW_RESIZABLE
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, wflag)
	ck(err)

	window.SetTitle("L-System")

	texture, err = renderer.CreateTexture(sdl.PIXELFORMAT_ABGR8888, sdl.TEXTUREACCESS_TARGET, w, h)
	ck(err)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func run(name string) {
	f, err := os.Open(name)
	ck(err)

	err = lsys.Parse(f)
	ck(err)

	render()
	for {
		event()
		blit()
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
			case sdl.K_LEFT:
				if iters > 0 {
					iters--
				}
				render()
			case sdl.K_RIGHT:
				iters++
				render()
			}
		case sdl.WindowEvent:
			switch ev.Event {
			case sdl.WINDOWEVENT_RESIZED:
				var err error
				texture, err = renderer.CreateTexture(sdl.PIXELFORMAT_ABGR8888, sdl.TEXTUREACCESS_TARGET,
					nextPow2(int(ev.Data[0])), nextPow2(int(ev.Data[1])))
				ck(err)
				render()
			}
		}
	}
}

func nextPow2(n int) int {
	v := 1
	for v < n {
		v <<= 1
	}
	return v
}

func render() {
	err := renderer.SetTarget(texture)
	ck(err)
	renderer.SetDrawColor(color.RGBA{100, 120, 130, 255})
	renderer.Clear()
	lsys.Reset()
	lsys.Rewrite(iters)
	fmt.Println(lsys.Axiom, len(lsys.Axiom))
	lsys.Draw(lsys.Axiom)
	renderer.SetTarget(nil)
}

func blit() {
	w, h, _ := renderer.OutputSize()
	renderer.Clear()
	renderer.Copy(texture, &sdl.Rect{0, 0, int32(w), int32(h)}, nil)
	renderer.Present()
}

type L struct {
	S
	O       f64.Vec2
	Plr     bool
	Pl      []sdl.Point
	History []string
	Axiom   string
	Rules   map[string]string
	Seed    struct {
		Axiom string
		State S
	}
	Stack []S
}

type S struct {
	P   f64.Vec2   // position
	A   float64    // angle
	At  float64    // angle turn
	Ai  float64    // angle turn increment
	Ad  float64    // angle +- sign
	Ln  float64    // line length
	Lns float64    // line length scale factor
	Lw  float64    // line width
	Lwi float64    // line width increment
	C   color.RGBA // color
}

func (l *L) Reset() {
	l.Axiom = l.Seed.Axiom
	l.S = l.Seed.State
}

func (l *L) Parse(r io.Reader) error {
	l.Rules = make(map[string]string)
	l.A = f64.Deg2Rad(-90)
	l.At = f64.Deg2Rad(90)
	l.Ai = f64.Deg2Rad(1)
	l.Ad = 1
	l.Ln = 5
	l.Lns = 1.25
	l.Lw = 1
	l.Lwi = 1
	l.C = color.RGBA{255, 255, 255, 255}

	s := bufio.NewScanner(r)
	for s.Scan() {
		ln := s.Text()

		var t [4][2]string
		n1, _ := fmt.Sscanf(ln, "%s = %s", &t[0][0], &t[0][1])
		n2, _ := fmt.Sscanf(ln, "%s %s", &t[1][0], &t[1][1])
		n3, _ := fmt.Sscanf(ln, "%s --> %s", &t[2][0], &t[2][1])
		n4, _ := fmt.Sscanf(ln, "%s -> %s", &t[3][0], &t[3][1])

		var k, v string
		op := -1
		switch {
		case n1 == 2:
			k, v = t[0][0], t[0][1]
			op = 0
		case n3 == 2:
			k, v = t[2][0], t[2][1]
			op = 1
		case n4 == 2:
			k, v = t[3][0], t[3][1]
			op = 1
		case n2 == 2:
			k, v = t[1][0], t[1][1]
			op = 0
		}

		switch op {
		case 0:
			switch strings.ToLower(k) {
			case "axiom":
				l.Axiom = v
			case "a", "initial_angle":
				d, _ := strconv.ParseFloat(v, 64)
				l.A = f64.Deg2Rad(d)
			case "ø", "angle":
				d, _ := strconv.ParseFloat(v, 64)
				l.At = f64.Deg2Rad(d)
			case "ln", "line_length":
				l.Ln, _ = strconv.ParseFloat(v, 64)
			case "lw", "line_width":
				l.Lw, _ = strconv.ParseFloat(v, 64)
			case "lns", "line_scale_factor":
				l.Lns, _ = strconv.ParseFloat(v, 64)
			case "x", "off_x":
				l.O.X, _ = strconv.ParseFloat(v, 64)
			case "y", "off_y":
				l.O.Y, _ = strconv.ParseFloat(v, 64)
			case "col", "color":
				l.C, _ = chroma.ParseRGBA(v)
			}
		case 1:
			l.Rules[k] = v
		}
	}
	l.Seed.Axiom = l.Axiom
	l.Seed.State = l.S
	return nil
}

func (l *L) Step() {
	l.History = append(l.History, l.Axiom)

	var s strings.Builder
	for _, ch := range l.Axiom {
		if v, e := l.Rules[string(ch)]; e {
			s.WriteString(v)
		} else {
			s.WriteString(string(ch))
		}
	}
	l.Axiom = s.String()
}

func (l *L) Rewrite(n int) {
	for i := 0; i < n; i++ {
		l.Step()
	}
}

func (l *L) Undo() {
	l.Axiom = ""
	if len(l.History) > 0 {
		l.Axiom = l.History[len(l.History)-1]
	}
}

func (l *L) Draw(s string) {
	w, h, _ := renderer.OutputSize()
	l.P = f64.Vec2{float64(w / 2), float64(h)}
	l.P = l.P.Add(l.O)
	l.Pl = l.Pl[:0]
	l.Plr = false
	for i, op := range s {
		if i > 0 && i%100000 == 0 {
			fmt.Printf("%d iterations\n", i)
		}
		switch op {
		case 'F': // Move forward by line length drawing a line
			p0, p1 := l.move(l.Ln, 0)
			sdlgfx.ThickLine(renderer, int(p0.X), int(p0.Y), int(p1.X), int(p1.Y), int(l.Lw), l.C)
		case 'f': // Move forward by line length without drawing a line
			l.move(l.Ln, 0)
		case '+': // Turn left by turning angle
			l.A += l.At * l.Ad
		case '-': // Turn right by turning angle
			l.A -= l.At * l.Ad
		case '|': // Reverse direction (ie: turn by 180 degrees)
			l.A += -f64.Sign(l.A) * f64.Deg2Rad(180)
		case '[': // Push current drawing state onto stack
			l.Stack = append(l.Stack, l.S)
		case ']': // Pop current drawing state from the stack
			if len(l.Stack) > 0 {
				l.S = l.Stack[len(l.Stack)-1]
				l.Stack = l.Stack[:len(l.Stack)-1]
			}
		case '#': // Increment the line width by line width increment
			l.Lw += l.Lwi
		case '!': // Decrement the line width by line width increment
			l.Lw -= l.Lwi
		case '@': // Draw a dot with line width radius
			sdlgfx.FilledCircle(renderer, int(l.P.X), int(l.P.Y), int(l.Lw), l.C)
		case '{': // Open a polygon
			l.Pl = l.Pl[:0]
			l.Plr = true
		case '}': // Close a polygon and fill it with fill colour
			sdlgfx.FilledPolygon(renderer, l.Pl, l.C)
			l.Plr = false
		case '<': // Multiply the line length by the line length scale factor
			l.Ln *= l.Lns
		case '>': // Divide the line length by the line length scale factor
			if l.Lns != 0 {
				l.Ln /= l.Lns
			}
		case '&': // Swap the meaning of + and -
			l.Ad = -l.Ad
		case '(': // Decrement turning angle by turning angle increment
			l.At -= l.Ai
		case ')': // Increment turning angle by turning angle increment
			l.At += l.Ai
		}
	}
	fmt.Println("done iterating")
}

func (l *L) move(dx, dy float64) (p0, p1 f64.Vec2) {
	p := l.P
	s, c := math.Sincos(l.A)
	m := f64.Mat2{
		{c, -s},
		{s, c},
	}
	l.P = l.P.Add(m.Transform(f64.Vec2{dx, dy}))
	if l.Plr {
		l.Pl = append(l.Pl, sdl.Point{int32(p.X), int32(p.Y)})
	}
	return p, l.P
}
