// ported from 9front
package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	"image/draw"
	"image/png"
	"log"
	"math"
	"math/rand"
	"os"
	"runtime"
	"strconv"
	"strings"
	"text/scanner"

	"github.com/qeedquan/go-media/image/ttf"
	"github.com/qeedquan/go-media/math/ga"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlimage/sdlcolor"
	"github.com/qeedquan/go-media/stb/stbtt"
)

var (
	picx     = 640
	picy     = 480
	xmin     = -10.0
	xmax     = 10.0
	ymin     = -10.0
	ymax     = 10.0
	gymin    = math.Inf(-1)
	gymax    = math.Inf(1)
	stdout   bool
	noAxes   bool
	pixels   []byte
	zooms    []ga.Rect2d
	curvePos = image.Pt(-1, -1)
	grab     = image.Rect(-1, -1, -1, -1)

	funcs []Eval

	window   *sdl.Window
	renderer *sdl.Renderer
	texture  *sdl.Texture
	canvas   *image.RGBA
	font     *stbtt.Bitmap

	colors = []color.RGBA{
		{0x0, 0x0, 0x0, 0xff},
		{0xcc, 0x00, 0x00, 0xff},
		{0x00, 0xcc, 0x00, 0xff},
		{0x00, 0x00, 0xcc, 0xff},
		{0xff, 0x00, 0xcc, 0xff},
		{0xff, 0xaa, 0x00, 0xff},
		{0xcc, 0xcc, 0x00, 0xff},
	}
)

func main() {
	runtime.LockOSThread()
	log.SetFlags(0)
	log.SetPrefix("fplot: ")
	parseFlags()
	loadFonts()

	if stdout {
		resize(picx, picy)
		blitGraphs()
		ck(png.Encode(os.Stdout, canvas))
	} else {
		initSDL()
		for {
			event()
			blit()
		}
	}
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: fplot [options] expr ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func iabs(x int) int {
	if x < 0 {
		return -x
	}
	return x
}

func parseFlags() {
	var res, size string
	flag.BoolVar(&noAxes, "a", false, "don't show axes")
	flag.BoolVar(&stdout, "c", false, "write image to stdout as png")
	flag.StringVar(&res, "r", fmt.Sprintf("%v:%v %v:%v", xmin, xmax, ymin, ymax), "output range")
	flag.StringVar(&size, "s", fmt.Sprintf("%v %v", picx, picy), "window size")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}
	parseRange(res)
	parseSize(size)

	for _, expr := range flag.Args() {
		var fn Eval
		err := fn.Compile(expr)
		if err != nil {
			fmt.Fprintf(os.Stderr, "fplot: failed to compile expression %q: %v\n", expr, err)
		} else {
			funcs = append(funcs, fn)
		}
	}
}

func loadFonts() {
	font, _, _ = stbtt.BakeFontBitmap(ttf.VGA437["default"], 0, 16, 512, 512, 0, 128)
	font.FG = sdlcolor.Black
	font.BG = sdlcolor.White
}

func makeScanner(s string) *scanner.Scanner {
	var r scanner.Scanner
	r.Whitespace = scanner.GoWhitespace
	r.Mode = scanner.ScanFloats
	r.Init(strings.NewReader(s))
	return &r
}

func parseRange(s string) {
	r := makeScanner(s)

	if eof, val := scanNumber(r); !eof {
		xmin = val
	}
	if eof, val := scanNumber(r); !eof {
		xmax = val
	}
	if eof, val := scanNumber(r); !eof {
		ymin = val
	}
	if eof, val := scanNumber(r); !eof {
		ymax = val
	}

	if xmax < xmin {
		xmin, xmax = xmax, xmin
	}
	if ymax < ymin {
		ymin, ymax = ymax, ymin
	}
}

func parseSize(s string) {
	r := makeScanner(s)

	if eof, val := scanNumber(r); !eof {
		picx = int(val)
	}
	if eof, val := scanNumber(r); !eof {
		picy = int(val)
	}

	if picx <= 0 || picy <= 0 {
		log.Fatal("invalid window size")
	}
}

func scanNumber(r *scanner.Scanner) (eof bool, val float64) {
	sign := 1.0
	for {
		switch r.Scan() {
		case scanner.EOF:
			eof = true
			return
		case '+':
		case '-':
			sign *= -1
		case scanner.Int, scanner.Float:
			var err error
			val, err = strconv.ParseFloat(r.TokenText(), 64)
			if err != nil {
				eof = true
			}
			val *= sign
			return
		}
	}
	return
}

func initSDL() {
	err := sdl.Init(sdl.INIT_VIDEO)
	ck(err)

	window, renderer, err = sdl.CreateWindowAndRenderer(picx, picy, sdl.WINDOW_RESIZABLE)
	ck(err)

	window.SetTitle("Function Plotter")

	resize(picx, picy)
}

func resize(w, h int) {
	var err error

	if !stdout {
		if texture != nil {
			texture.Destroy()
		}

		texture, err = renderer.CreateTexture(sdl.PIXELFORMAT_ABGR8888, sdl.TEXTUREACCESS_STREAMING, w, h)
		ck(err)
	}

	canvas = image.NewRGBA(image.Rect(0, 0, w, h))
	pixels = make([]byte, w*h)
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
			case sdl.K_u:
				unzoom()
			}
		case sdl.WindowEvent:
			switch ev.Event {
			case sdl.WINDOWEVENT_RESIZED:
				picx = int(ev.Data[0])
				picy = int(ev.Data[1])
				resize(picx, picy)
				blitGraphs()
			}
		case sdl.MouseMotionEvent:
			mx, my, button := sdl.GetMouseState()
			if button&1 != 0 {
				if grab.Min.X < 0 || grab.Min.Y < 0 {
					grab.Min = image.Pt(mx, my)
					grab.Max = grab.Min
				} else {
					grab.Max = image.Pt(mx, my)
				}
			} else if button&2 != 0 {
				readout(image.Pt(int(ev.X), int(ev.Y)))
			}
		case sdl.MouseButtonDownEvent:
			if ev.Button == 3 {
				unzoom()
			}
		case sdl.MouseButtonUpEvent:
			if ev.Button == 1 {
				zoom()
				grab = image.Rect(-1, -1, -1, -1)
			} else if ev.Button == 2 {
				curvePos = image.Pt(-1, -1)
			}
		}
	}
}

func zoom() {
	grab = grab.Canon()
	if grab.Dx() <= 1 || grab.Dy() <= 1 {
		return
	}
	zooms = append(zooms, ga.Rect2d{ga.Vec2d{xmin, ymin}, ga.Vec2d{xmax, ymax}})
	r := canvas.Bounds()
	xmin = convx(r, grab.Min.X)
	xmax = convx(r, grab.Max.X)
	ymin = convy(r, grab.Min.Y)
	ymax = convy(r, grab.Max.Y)
}

func unzoom() {
	if l := len(zooms); l > 0 {
		r := zooms[l-1]
		xmin = r.Min.X
		ymin = r.Min.Y
		xmax = r.Max.X
		ymax = r.Max.Y
		zooms = zooms[:l-1]
	}
}

func blit() {
	renderer.SetDrawColor(sdlcolor.Black)
	renderer.Clear()
	blitGraphs()
	blitRect(grab, sdlcolor.Red)
	if curvePos.X >= 0 && curvePos.Y >= 0 {
		blitCircle(curvePos.X, curvePos.Y, 3, sdlcolor.Red)
		r := canvas.Bounds()
		x := convx(r, curvePos.X)
		y := convy(r, curvePos.Y)

		funcs[0].Variables["x"] = x
		besty, _ := funcs[0].Calculate()
		for i := 1; i < len(funcs); i++ {
			funcs[i].Variables["x"] = x
			ny, _ := funcs[i].Calculate()
			if math.Abs(ny-y) < math.Abs(besty-y) {
				besty = ny
			}
		}
		font.Printf(canvas, 0, 0, "%#.4g,%#.4g", x, besty)
	}
	texture.Update(nil, canvas.Pix, canvas.Stride)
	renderer.Copy(texture, nil, nil)
	renderer.Present()
}

func blitRect(r image.Rectangle, c color.Color) {
	blitLine(r.Min.X, r.Min.Y, r.Max.X, r.Min.Y, c)
	blitLine(r.Min.X, r.Min.Y, r.Min.X, r.Max.Y, c)
	blitLine(r.Min.X, r.Max.Y, r.Max.X, r.Max.Y, c)
	blitLine(r.Max.X, r.Min.Y, r.Max.X, r.Max.Y, c)
}

func blitLine(x0, y0, x1, y1 int, c color.Color) {
	dx := iabs(x1 - x0)
	dy := -iabs(y1 - y0)
	sx, sy := -1, -1
	if x0 < x1 {
		sx = 1
	}
	if y0 < y1 {
		sy = 1
	}
	err := dx + dy
	for {
		canvas.Set(x0, y0, c)
		if x0 == x1 && y0 == y1 {
			break
		}
		e2 := 2 * err
		if e2 >= dy {
			err += dy
			x0 += sx
		}
		if e2 <= dx {
			err += dx
			y0 += sy
		}
	}
}

func min(a, b int) int {
	if a < b {
		return a
	}
	return b
}

func max(a, b int) int {
	if a > b {
		return a
	}
	return b
}

func sign(a, b, c image.Point) int {
	return (a.X-c.X)*(b.Y-c.Y) - (b.X-c.X)*(a.Y-c.Y)
}

func insideTriangle(p, a, b, c image.Point) bool {
	b1 := sign(p, a, b) < 0
	b2 := sign(p, b, c) < 0
	b3 := sign(p, c, a) < 0
	return b1 == b2 && b2 == b3
}

func blitTriangle(a, b, c image.Point, col color.RGBA) {
	x0 := min(a.X, min(b.X, c.X))
	x1 := max(a.X, max(b.X, c.X))
	y0 := min(a.Y, min(b.Y, c.Y))
	y1 := max(a.Y, max(b.Y, c.Y))

	for y := y0; y <= y1; y++ {
		for x := x0; x <= x1; x++ {
			p := image.Pt(x, y)
			if insideTriangle(p, a, b, c) {
				canvas.Set(x, y, col)
			}
		}
	}
}

func blitLineArrow(x0, y0, x1, y1 int, c color.Color) {
	blitLine(x0, y0, x1, y1, c)

	size := 8
	var p0, p1, p2 image.Point
	if x0 == x1 {
		p0 = image.Pt(x0, y0)
		p1 = image.Pt(x0-size, y0+size)
		p2 = image.Pt(x0+size, y0+size)
	} else if y0 == y1 {
		p0 = image.Pt(x1, y1)
		p1 = image.Pt(x1-size, y1+size)
		p2 = image.Pt(x1-size, y1-size)
	}
	blitTriangle(p0, p1, p2, sdlcolor.Black)
}

func blitAxes() {
	r := canvas.Bounds()
	x := 0
	if xmin < 0 && xmax > 0 {
		x = deconvx(r, 0)
	} else {
		x = r.Min.X + 5
	}
	blitLineArrow(x, r.Min.Y, x, r.Max.Y, color.Black)

	y := 0
	if ymin < 0 && ymax > 0 {
		y = deconvy(r, 0)
	} else {
		y = r.Max.Y - 5
	}
	blitLineArrow(r.Min.X, y, r.Max.X, y, color.Black)

	nx, dx, mx := ticks(xmin, xmax)
	format := tickfmt(dx, mx, nx)
	for i := 0; i <= nx; i++ {
		p := deconvx(r, dx*float64(i)+mx)
		if xticklabel(format, dx*float64(i)+mx, p, x, y) {
			blitLine(p, y, p, y-5, color.Black)
		}
	}

	ny, dy, my := ticks(ymin, ymax)
	format = tickfmt(dy, my, ny)
	for i := 0; i <= ny; i++ {
		p := deconvy(r, dy*float64(i)+my)
		if yticklabel(format, dy*float64(i)+my, p, x, y) {
			blitLine(x, p, x+5, p, color.Black)
		}
	}
}

func blitGraphs() {
	draw.Draw(canvas, canvas.Bounds(), image.NewUniform(color.White), image.ZP, draw.Src)
	for i, f := range funcs {
		blitGraph(&f, canvas.Bounds(), colors[i%len(colors)])
	}
	if !noAxes {
		blitAxes()
	}
}

func blitGraph(f *Eval, r image.Rectangle, c color.Color) {
	for x := 0; x < picx; x++ {
		blitInter(f, r, convx(r, x), convx(r, x+1), 0, c)
	}
}

func pixel(x, y int, c color.Color) {
	canvas.Set(x, y, c)

	r := canvas.Bounds()
	p := image.Pt(x, y)
	if p.In(r) {
		pixels[picx*(y-r.Min.Y)+(x-r.Min.X)] = 1
	}
}

func blitCirclePoint(xc, yc, x, y int, c color.Color) {
	canvas.Set(xc+x, yc+y, c)
	canvas.Set(xc-x, yc+y, c)
	canvas.Set(xc+x, yc-y, c)
	canvas.Set(xc-x, yc-y, c)
	canvas.Set(xc+y, yc+x, c)
	canvas.Set(xc-y, yc+x, c)
	canvas.Set(xc+y, yc-x, c)
	canvas.Set(xc-y, yc-x, c)
}

func blitCircle(xc, yc, r int, c color.Color) {
	x, y := 0, r
	d := 3 - 2*r
	blitCirclePoint(xc, yc, x, y, c)
	for y >= x {
		x++
		if d > 0 {
			y--
			d = d + 4*(x-y) + 10
		} else {
			d = d + 4*x + 6
		}
		blitCirclePoint(xc, yc, x, y, c)
	}
}

func blitInter(f *Eval, r image.Rectangle, x1, x2 float64, n int, c color.Color) {
	ix1 := deconvx(r, x1)
	ix2 := deconvx(r, x2)
	iy1 := 0
	iy2 := 0

	f.Variables["x"] = x1
	y1, _ := f.Calculate()
	if !math.IsNaN(y1) {
		iy1 = deconvy(r, y1)
		pixel(ix1, iy1, c)
	}

	f.Variables["x"] = x2
	y2, _ := f.Calculate()
	if !math.IsNaN(y2) {
		iy2 = deconvy(r, y2)
		pixel(ix2, iy2, c)
	}

	if math.IsNaN(y1) || math.IsNaN(y2) {
		return
	}
	if n >= 10 {
		return
	}
	if iy2 >= iy1-1 && iy2 <= iy1+1 {
		return
	}
	if iy1 > r.Max.Y && iy2 > r.Max.Y {
		return
	}
	if iy1 < r.Min.Y && iy2 < r.Min.Y {
		return
	}
	blitInter(f, r, x1, (x1+x2)/2, n+1, c)
	blitInter(f, r, (x1+x2)/2, x2, n+1, c)
}

func readout(p image.Point) {
	best := math.MaxInt32
	bestp := image.Pt(0, 0)

	r := canvas.Bounds()
	for y := r.Min.Y; y < r.Max.Y; y++ {
		for x := r.Min.X; x < r.Max.X; x++ {
			if pixels[(y-r.Min.Y)*picx+(x-r.Min.X)] == 0 {
				continue
			}
			d := (x-p.X)*(x-p.X) + (y-p.Y)*(y-p.Y)
			if d < best {
				best = d
				bestp = image.Pt(x, y)
			}
		}
	}
	curvePos = bestp
}

func tickfmt(d, m float64, n int) string {
	e1 := math.Log10(math.Abs(m))
	e2 := math.Log10(math.Abs(m + float64(n)*d))
	if e2 > e1 {
		e1 = e2
	}
	if e2 >= 4 || e2 < -3 {
		x := int(math.Ceil(e1 - math.Log10(d) - 1))
		return fmt.Sprintf("%%.%de", x)
	} else {
		x := int(math.Ceil(-math.Log10(d)))
		return fmt.Sprintf("%%.%df", x)
	}
}

func xticklabel(format string, g float64, p, x, y int) bool {
	buf := fmt.Sprintf(format, g)
	lr := image.Rectangle{}
	lr.Min = image.Pt(p, y+2)
	fw, fh := font.StringSize(buf)
	lr.Max = lr.Min.Add(image.Pt(int(fw), int(fh)))
	lr = lr.Sub(image.Pt(lr.Dx()/2-1, 0))

	r := canvas.Bounds()
	if lr.Max.Y >= r.Max.Y {
		ny := y - 7 - lr.Dy()
		lr = lr.Sub(image.Pt(0, lr.Min.Y-ny))
	}
	if lr.In(r) && (lr.Min.X > x || lr.Max.X <= x) {
		return true
	}
	return false
}

func yticklabel(format string, g float64, p, x, y int) bool {
	buf := fmt.Sprintf(format, g)
	lr := image.Rectangle{}
	fw, fh := font.StringSize(buf)
	lr.Max = image.Pt(int(fw), int(fh))
	lr = lr.Add(image.Pt(x-lr.Dx()-2, p-lr.Dy()/2))
	r := canvas.Bounds()
	if lr.Min.X < r.Min.X {
		nx := x + 7
		lr = lr.Sub(image.Pt(lr.Min.X-nx, 0))
	}
	if lr.In(r) && (lr.Min.Y > y || lr.Max.Y <= y) {
		font.Print(canvas, lr.Min.X, lr.Min.Y, buf)
		return true
	}
	return false
}

func calcm(min, max float64, e int) (sp int, dp, mp float64) {
	d := math.Pow(10, float64(e>>1))
	if e&1 != 0 {
		d *= 5
	}
	m := min
	if min < 0 && max > 0 {
		m += math.Mod(-m, d)
	} else {
		r := math.Mod(m, d)
		if r < 0 {
			m -= r
		} else {
			m += d - r
		}
	}
	dp = d
	mp = m
	sp = int((max - m) * 0.999 / d)
	return
}

func ticks(min, max float64) (sp int, dp, mp float64) {
	n := 0
	m := 0.0
	e := int(2 * math.Ceil(math.Log10(max-min)))
	beste := 0
	bestm := math.Inf(1)
	for ; e > -100; e-- {
		n, _, _ = calcm(min, max, e)
		if n <= 0 {
			continue
		}
		if n < 10 {
			m = 10 / float64(n)
		} else {
			m = float64(n) / 10
		}
		if m < bestm {
			beste = e
			bestm = m
		}
		if n > 10 {
			break
		}
	}
	_, dp, mp = calcm(min, max, beste)
	sp = int((max - mp) / dp)
	return
}

func convx(r image.Rectangle, x int) float64 {
	return (xmax-xmin)*(float64(x-r.Min.X)/float64(r.Max.X-r.Min.X)) + xmin
}

func deconvx(r image.Rectangle, dx float64) int {
	return int((dx-xmin)*float64(r.Max.X-r.Min.X)/(xmax-xmin) + 0.5)
}

func convy(r image.Rectangle, y int) float64 {
	return (ymax-ymin)*(float64(r.Max.Y-y)/float64(r.Max.Y-r.Min.Y)) + ymin
}

func deconvy(r image.Rectangle, dy float64) int {
	return int((ymax-dy)*float64(r.Max.Y-r.Min.Y)/(ymax-ymin) + 0.5)
}

const (
	NUMBER = iota + 1
	VAR
	BINARY
	UNARY
)

type Eval struct {
	stack []float64

	code []Op
	ops  []Op

	scan scanner.Scanner
	err  error

	Operators map[string]Operator
	Variables map[string]float64
	Constants map[string]float64
}

type Operator struct {
	Type   int
	Rassoc bool
	Prec   int
	Func   func(*Eval)
}

type Op struct {
	Type  int
	Name  string
	Func  func(*Eval)
	Prec  int
	Value float64
}

func (op Op) String() string {
	var s string
	switch op.Type {
	case NUMBER:
		s = fmt.Sprintf("number %v", op.Value)
	case VAR:
		s = fmt.Sprintf("var %v", op.Value)
	case BINARY:
		s = fmt.Sprintf("binary %v", op.Func)
	case UNARY:
		s = fmt.Sprintf("unary %v", op.Func)
	}
	return s
}

type bailout struct{}

func (c *Eval) parse() {
	defer func() {
		if e := recover(); e != nil {
			if _, ok := e.(bailout); !ok {
				panic(e)
			}
		}
	}()

	for {
		ch := c.scan.Scan()
		if ch == scanner.EOF {
			break
		}

		text := c.scan.TokenText()
		switch ch {
		case scanner.Int, scanner.Float:
			n, _ := strconv.ParseFloat(text, 64)
			c.code = append(c.code, Op{
				Type:  NUMBER,
				Value: n,
			})
		case '(':
			c.ops = append(c.ops, Op{
				Type: '(',
			})
		case ')':
			for len(c.ops) > 0 &&
				(c.ops[len(c.ops)-1].Type == BINARY || c.ops[len(c.ops)-1].Type == UNARY) {
				c.popOperator()
			}
			if len(c.ops) == 0 {
				c.errf("mismatched parentheses")
			}
			c.ops = c.ops[:len(c.ops)-1]
		case scanner.Ident:
			if text == "x" {
				c.code = append(c.code, Op{
					Type: VAR,
					Name: text,
				})
				break
			}
			fallthrough
		default:
			if op, found := c.Operators[text]; found {
				if op.Type == BINARY {
					for len(c.ops) > 0 &&
						(c.ops[len(c.ops)-1].Type == BINARY || c.ops[len(c.ops)-1].Type == UNARY) &&
						(c.ops[len(c.ops)-1].Prec > op.Prec ||
							op.Rassoc && c.ops[len(c.ops)-1].Prec == op.Prec) {
						c.popOperator()
					}
				}
				c.ops = append(c.ops, Op{
					Type: op.Type,
					Func: op.Func,
					Prec: op.Prec,
				})
			} else if val, found := c.Constants[text]; found {
				c.code = append(c.code, Op{
					Type:  NUMBER,
					Value: val,
				})
			} else {
				if ch == scanner.Ident {
					c.errf("unknown identifier %q", text)
				} else {
					c.errf("unknown symbol %q", string(ch))
				}
			}
		}
	}

	for len(c.ops) > 0 {
		switch c.ops[len(c.ops)-1].Type {
		case BINARY, UNARY:
			c.popOperator()
		case '(':
			c.errf("mismatched parentheses")
		default:
			c.errf("syntax error")
		}
	}
}

func (c *Eval) popOperator() {
	if len(c.ops) == 0 {
		c.errf("operator stack is empty")
	}
	t := c.ops[len(c.ops)-1]
	c.ops = c.ops[:len(c.ops)-1]
	if t.Type != BINARY && t.Type != UNARY {
		c.errf("non-operator pop, got %q", t.Type)
	}
	c.code = append(c.code, Op{
		Type: t.Type,
		Func: t.Func,
	})
}

func (c *Eval) errf(format string, args ...interface{}) {
	text := fmt.Sprintf(format, args...)
	c.err = fmt.Errorf("%v: %v", c.scan.Position, text)
	panic(bailout{})
}

func (c *Eval) Compile(expr string) error {
	r := strings.NewReader(expr)
	c.scan.Init(r)
	c.scan.Whitespace = scanner.GoWhitespace
	c.scan.Mode = scanner.ScanIdents | scanner.ScanFloats
	if c.Operators == nil {
		c.Operators = operators
	}
	if c.Constants == nil {
		c.Constants = constants
	}
	if c.Variables == nil {
		c.Variables = make(map[string]float64)
	}
	c.parse()
	return c.err
}

func (c *Eval) Push(v float64) {
	c.stack = append(c.stack, v)
}

func (c *Eval) Pop() float64 {
	if len(c.stack) == 0 {
		c.err = fmt.Errorf("stack underflow")
		return 0
	}
	v := c.stack[len(c.stack)-1]
	c.stack = c.stack[:len(c.stack)-1]
	return v
}

func (c *Eval) Peek() float64 {
	if len(c.stack) == 0 {
		return 0
	}
	return c.stack[len(c.stack)-1]
}

func (c *Eval) Calculate() (float64, error) {
	c.stack = c.stack[:0]
	c.err = nil
	for _, o := range c.code {
		switch o.Type {
		case NUMBER:
			c.stack = append(c.stack, o.Value)
		case VAR:
			c.stack = append(c.stack, c.Variables[o.Name])
		case UNARY, BINARY:
			o.Func(c)
		}
	}

	v := c.Peek()
	if v < gymin {
		gymin = v
	} else if v > gymax {
		gymax = v
	}

	if c.err != nil {
		return 0, c.err
	}
	return v, nil
}

func (c *Eval) Eval(expr string) (float64, error) {
	err := c.Compile(expr)
	if err != nil {
		return 0, err
	}
	return c.Calculate()
}

var operators = map[string]Operator{
	"+":           {BINARY, false, 0, add},
	"-":           {BINARY, false, 0, sub},
	"*":           {BINARY, false, 100, mul},
	"/":           {BINARY, false, 100, div},
	"%":           {BINARY, false, 100, mod},
	"^":           {BINARY, true, 200, pow},
	"min":         {BINARY, false, 0, fmin},
	"max":         {BINARY, false, 0, fmax},
	"dim":         {BINARY, false, 0, dim},
	"neg":         {UNARY, false, 50, neg},
	"abs":         {UNARY, false, 50, abs},
	"trunc":       {UNARY, false, 50, trunc},
	"erf":         {UNARY, false, 50, erf},
	"erfc":        {UNARY, false, 50, erfc},
	"erfinv":      {UNARY, false, 50, erfinv},
	"erfcinv":     {UNARY, false, 50, erfcinv},
	"ilogb":       {UNARY, false, 50, ilogb},
	"sin":         {UNARY, false, 50, sin},
	"sinh":        {UNARY, false, 50, sinh},
	"cos":         {UNARY, false, 50, cos},
	"cosh":        {UNARY, false, 50, cosh},
	"tan":         {UNARY, false, 50, tan},
	"tanh":        {UNARY, false, 50, tanh},
	"asin":        {UNARY, false, 50, asin},
	"asinh":       {UNARY, false, 50, asinh},
	"acos":        {UNARY, false, 50, acos},
	"acosh":       {UNARY, false, 50, acosh},
	"atan":        {UNARY, false, 50, atan},
	"atanh":       {UNARY, false, 50, atanh},
	"sqrt":        {UNARY, false, 50, sqrt},
	"cbrt":        {UNARY, false, 50, cbrt},
	"exp":         {UNARY, false, 50, exp},
	"exp2":        {UNARY, false, 50, exp2},
	"expm1":       {UNARY, false, 50, expm1},
	"log":         {UNARY, false, 50, log10},
	"ln":          {UNARY, false, 50, ln},
	"log1p":       {UNARY, false, 50, log1p},
	"logb":        {UNARY, false, 50, logb},
	"floor":       {UNARY, false, 50, floor},
	"ceil":        {UNARY, false, 50, ceil},
	"gamma":       {UNARY, false, 50, gamma},
	"y0":          {UNARY, false, 50, y0},
	"y1":          {UNARY, false, 50, y1},
	"j0":          {UNARY, false, 50, j0},
	"j1":          {UNARY, false, 50, j1},
	"sinc":        {UNARY, false, 50, sinc},
	"fact":        {UNARY, false, 50, fact},
	"fib":         {UNARY, false, 50, fib},
	"sgn":         {UNARY, false, 50, sgn},
	"cot":         {UNARY, false, 50, cot},
	"sec":         {UNARY, false, 50, sec},
	"csc":         {UNARY, false, 50, csc},
	"gd":          {UNARY, false, 50, gd},
	"round":       {UNARY, false, 50, round},
	"roundtoeven": {UNARY, false, 50, roundtoeven},
	"urand":       {UNARY, false, 50, urand},
	"erand":       {UNARY, false, 50, erand},
	"nrand":       {UNARY, false, 50, nrand},
}

var constants = map[string]float64{
	"pi":  math.Pi,
	"e":   math.E,
	"phi": math.Phi,
	"tau": 2 * math.Pi,
	"τ":   2 * math.Pi,
	"π":   math.Pi,
	"γ":   0.57721566490153286060651209008240243104215933593992,
}

func add(c *Eval) {
	y := c.Pop()
	x := c.Pop()
	c.Push(x + y)
}

func sub(c *Eval) {
	y := c.Pop()
	x := c.Pop()
	c.Push(x - y)
}

func mul(c *Eval) {
	y := c.Pop()
	x := c.Pop()
	c.Push(x * y)
}

func div(c *Eval) {
	y := c.Pop()
	x := c.Pop()
	if y == 0 {
		c.Push(math.MaxFloat64)
		return
	}
	c.Push(x / y)
}

func mod(c *Eval) {
	y := c.Pop()
	x := c.Pop()
	if y == 0 {
		c.Push(0)
		return
	}
	c.Push(math.Mod(x, y))
}

func pow(c *Eval) {
	y := c.Pop()
	x := c.Pop()
	c.Push(math.Pow(x, y))
}

func neg(c *Eval) {
	x := c.Pop()
	c.Push(-x)
}

func abs(c *Eval) {
	x := c.Pop()
	c.Push(math.Abs(x))
}

func trunc(c *Eval) {
	x := c.Pop()
	c.Push(math.Trunc(x))
}

func erf(c *Eval) {
	x := c.Pop()
	c.Push(math.Erf(x))
}

func erfc(c *Eval) {
	x := c.Pop()
	c.Push(math.Erfc(x))
}

func ilogb(c *Eval) {
	x := c.Pop()
	c.Push(float64(math.Ilogb(x)))
}

func sin(c *Eval) {
	x := c.Pop()
	c.Push(math.Sin(x))
}

func sinh(c *Eval) {
	x := c.Pop()
	c.Push(math.Sinh(x))
}

func cos(c *Eval) {
	x := c.Pop()
	c.Push(math.Cos(x))
}

func cosh(c *Eval) {
	x := c.Pop()
	c.Push(math.Cosh(x))
}

func tan(c *Eval) {
	x := c.Pop()
	c.Push(math.Tan(x))
}

func tanh(c *Eval) {
	x := c.Pop()
	c.Push(math.Tanh(x))
}

func asin(c *Eval) {
	x := c.Pop()
	c.Push(math.Asin(x))
}

func asinh(c *Eval) {
	x := c.Pop()
	c.Push(math.Asinh(x))
}

func acos(c *Eval) {
	x := c.Pop()
	c.Push(math.Acos(x))
}

func acosh(c *Eval) {
	x := c.Pop()
	c.Push(math.Acosh(x))
}

func atan(c *Eval) {
	x := c.Pop()
	c.Push(math.Atan(x))
}

func atanh(c *Eval) {
	x := c.Pop()
	c.Push(math.Atanh(x))
}

func sqrt(c *Eval) {
	x := c.Pop()
	c.Push(math.Sqrt(x))
}

func cbrt(c *Eval) {
	x := c.Pop()
	c.Push(math.Cbrt(x))
}

func exp(c *Eval) {
	x := c.Pop()
	c.Push(math.Exp(x))
}

func exp2(c *Eval) {
	x := c.Pop()
	c.Push(math.Exp2(x))
}

func expm1(c *Eval) {
	x := c.Pop()
	c.Push(math.Expm1(x))
}

func log10(c *Eval) {
	x := c.Pop()
	c.Push(math.Log(x))
}

func ln(c *Eval) {
	x := c.Pop()
	c.Push(math.Log2(x))
}

func log1p(c *Eval) {
	x := c.Pop()
	c.Push(math.Log1p(x))
}

func logb(c *Eval) {
	x := c.Pop()
	c.Push(math.Logb(x))
}

func floor(c *Eval) {
	x := c.Pop()
	c.Push(math.Floor(x))
}

func ceil(c *Eval) {
	x := c.Pop()
	c.Push(math.Ceil(x))
}

func gamma(c *Eval) {
	x := c.Pop()
	c.Push(math.Gamma(x))
}

func y0(c *Eval) {
	x := c.Pop()
	c.Push(math.Y0(x))
}

func y1(c *Eval) {
	x := c.Pop()
	c.Push(math.Y1(x))
}

func j0(c *Eval) {
	x := c.Pop()
	c.Push(math.J0(x))
}

func j1(c *Eval) {
	x := c.Pop()
	c.Push(math.J1(x))
}

func sinc(c *Eval) {
	x := c.Pop()
	if x == 0 {
		c.Push(1)
	} else {
		c.Push(math.Sin(x) / x)
	}
}

func fact(c *Eval) {
	x := int64(c.Pop())
	if x <= 1 {
		c.Push(1)
		return
	}

	z := int64(1)
	for y := int64(2); y <= x; y++ {
		z *= y
	}
	c.Push(float64(z))
}

func fib(c *Eval) {
	x := int64(c.Pop())
	a, b := 0, 1
	for y := int64(0); y < x; y++ {
		a, b = b, a+b
	}
	c.Push(float64(a))
}

func sgn(c *Eval) {
	x := c.Pop()
	switch {
	case x < 0:
		c.Push(-1)
	case x == 0:
		c.Push(0)
	case x > 0:
		c.Push(1)
	}
}

func cot(c *Eval) {
	x := c.Pop()
	c.Push(1 / math.Tan(x))
}

func sec(c *Eval) {
	x := c.Pop()
	c.Push(1 / math.Cos(x))
}

func csc(c *Eval) {
	x := c.Pop()
	c.Push(1 / math.Sin(x))
}

func gd(c *Eval) {
	x := c.Pop()
	c.Push(math.Asin(math.Tanh(x)))
}

func round(c *Eval) {
	x := c.Pop()
	c.Push(math.Round(x))
}

func roundtoeven(c *Eval) {
	x := c.Pop()
	c.Push(math.RoundToEven(x))
}

func erfinv(c *Eval) {
	x := c.Pop()
	c.Push(math.Erfinv(x))
}

func erfcinv(c *Eval) {
	x := c.Pop()
	c.Push(math.Erfcinv(x))
}

func fmin(c *Eval) {
	x := c.Pop()
	y := c.Pop()
	if y < x {
		x = y
	}
	c.Push(x)
}

func fmax(c *Eval) {
	x := c.Pop()
	y := c.Pop()
	if y > x {
		x = y
	}
	c.Push(x)
}

func dim(c *Eval) {
	x := c.Pop()
	y := c.Pop()
	c.Push(math.Dim(x, y))
}

func urand(c *Eval) {
	x := c.Pop()
	x = (2*rand.Float64() - 1) * x
	c.Push(x)
}

func erand(c *Eval) {
	x := c.Pop()
	c.Push(rand.ExpFloat64() * x)
}

func nrand(c *Eval) {
	x := c.Pop()
	c.Push(rand.NormFloat64() * x)
}
