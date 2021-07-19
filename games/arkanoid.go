// based on https://github.com/justinmeister/Arkanoid

package main

import (
	"flag"
	"os"
	"runtime"

	"github.com/qeedquan/go-media/math/f64"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlgfx"
)

var (
	Black  = sdl.Color{0, 0, 0, 255}
	White  = sdl.Color{255, 255, 255, 255}
	Cyan   = sdl.Color{0, 255, 255, 255}
	Yellow = sdl.Color{255, 255, 0, 255}
	Blue   = sdl.Color{0, 0, 255, 255}
	Gray   = sdl.Color{50, 50, 50, 255}
)

const (
	DisplayWidth  = 640
	DisplayHeight = 480

	BlockGap     = 2
	BlockWidth   = 62
	BlockHeight  = 25
	ArrayWidth   = 10
	ArrayHeight  = 5
	PaddleWidth  = 100
	PaddleHeight = 10
	BallRadius   = 10
	BallSpeed    = 4
)

var (
	BgColor     = Gray
	PaddleColor = White
	BallColor   = Blue
)

func main() {
	app := NewApp(NewConfig())
	app.Run()
}

func ck(err error) {
	if err != nil {
		sdl.LogCritical(sdl.LOG_CATEGORY_APPLICATION, "%v", err)
		sdl.ShowSimpleMessageBox(sdl.MESSAGEBOX_ERROR, "Error", err.Error(), nil)
		os.Exit(1)
	}
}

func NewConfig() Config {
	var conf Config
	flag.BoolVar(&conf.Fullscreen, "fullscreen", false, "fullscreen mode")
	flag.Parse()
	return conf
}

type Config struct {
	Fullscreen bool
}

type App struct {
	window   *sdl.Window
	renderer *sdl.Renderer
	blocks   []Block
	paddle   sdl.Rect
	ball     Ball
	run      bool
	fps      sdlgfx.FPSManager
}

func NewApp(conf Config) *App {
	runtime.LockOSThread()
	err := sdl.Init(sdl.INIT_EVERYTHING &^ sdl.INIT_AUDIO)
	ck(err)

	sdl.SetHint(sdl.HINT_RENDER_SCALE_QUALITY, "best")
	wflag := sdl.WINDOW_RESIZABLE
	if conf.Fullscreen {
		wflag |= sdl.WINDOW_FULLSCREEN_DESKTOP
	}
	window, renderer, err := sdl.CreateWindowAndRenderer(DisplayWidth, DisplayHeight, wflag)
	ck(err)

	window.SetTitle("Arkanoid")
	renderer.SetLogicalSize(DisplayWidth, DisplayHeight)

	return &App{
		window:   window,
		renderer: renderer,
	}
}

func (c *App) xmouse() int32 {
	mx, _, _ := sdl.GetMouseState()
	ow, _, _ := c.renderer.OutputSize()
	v := c.renderer.Viewport()
	return int32(f64.LinearRemap(float64(mx), float64(v.X), float64(ow)-float64(v.X), 0, DisplayWidth))
}

func (c *App) createBall(paddle sdl.Rect) Ball {
	return Ball{
		X:      paddle.X + paddle.W/2 - BallRadius/2,
		Y:      paddle.Y - BallRadius - 1,
		R:      BallRadius,
		Moving: false,
		Vel:    sdl.Point{BallSpeed, -BallSpeed},
		Speed:  1,
	}
}

func (c *App) createPaddle() sdl.Rect {
	return sdl.Rect{
		c.xmouse() - PaddleWidth/2,
		(DisplayHeight - PaddleHeight),
		PaddleWidth,
		PaddleHeight,
	}
}

func (c *App) createBlocks() []Block {
	var blocks []Block
	for y := 0; y < ArrayHeight; y++ {
		for x := 0; x < ArrayWidth; x++ {
			color := Cyan
			if y%2 == 0 {
				color = Yellow
			}
			blocks = append(blocks, Block{
				Rect: sdl.Rect{
					int32(x * (BlockWidth + BlockGap)),
					int32(y * (BlockHeight + BlockGap)),
					BlockWidth,
					BlockHeight,
				},
				Color: color,
			})
		}
	}
	return blocks
}

func (c *App) Reset() {
	c.paddle = c.createPaddle()
	c.blocks = c.createBlocks()
	c.ball = c.createBall(c.paddle)
	c.fps.Init()
	c.fps.SetRate(60)
	c.run = true
}

func (c *App) Run() {
	c.Reset()
	for c.run {
		c.event()
		c.update()
		c.draw()
		c.fps.Delay()
	}
}

func (c *App) event() {
	for {
		ev := sdl.PollEvent()
		if ev == nil {
			break
		}

		switch ev := ev.(type) {
		case sdl.QuitEvent:
			c.run = false
		case sdl.KeyDownEvent:
			switch ev.Sym {
			case sdl.K_ESCAPE:
				c.run = false
			case sdl.K_SPACE:
				c.ball.Moving = true
			case sdl.K_RETURN:
				c.Reset()
			}
		case sdl.MouseButtonDownEvent:
			switch ev.Button {
			case sdl.BUTTON_LEFT:
				c.ball.Moving = true
			case sdl.BUTTON_RIGHT:
				c.Reset()
			}
		case sdl.MouseWheelEvent:
			switch b := &c.ball; {
			case ev.Y < 0:
				if b.Speed > 1 {
					b.Speed--
				}
			case ev.Y > 0:
				if b.Speed < 10 {
					b.Speed++
				}
			}
		}
	}
}

func (c *App) updatePaddle() {
	p := &c.paddle
	p.X = c.xmouse() - p.W/2
	if p.X < 0 {
		p.X = 0
	} else if p.X+p.W > DisplayWidth {
		p.X = DisplayWidth - p.W
	}
}

func (c *App) updateBall() {
	b := &c.ball
	p := &c.paddle
	if !b.Moving {
		b.X = p.X + p.W/2 - BallRadius/2
		return
	}
	b.X += b.Vel.X * b.Speed
	b.Y += b.Vel.Y * b.Speed

	hit := false
	for i := range c.blocks {
		v := &c.blocks[i]
		if !v.Dead && intersect(b, &v.Rect) {
			v.Dead = true
			hit = true
		}
	}
	if intersect(b, p) {
		hit = true
		b.Y = p.Y - BallRadius
	}
	if hit {
		b.Vel.Y *= -1
	}

	if b.X+b.R > DisplayWidth {
		b.Vel.X *= -1
		b.X = DisplayWidth - b.R
	} else if b.X-b.R < 0 {
		b.Vel.X *= -1
		b.X = b.R
	}

	if b.Y-b.R < 0 {
		b.Vel.Y *= -1
		b.Y = b.R
	}

	if b.Y > DisplayHeight || c.won() {
		c.Reset()
	}
}

func (c *App) won() bool {
	for _, b := range c.blocks {
		if !b.Dead {
			return false
		}
	}
	return true
}

func (c *App) update() {
	c.updatePaddle()
	c.updateBall()
}

func (c *App) draw() {
	re := c.renderer
	re.SetDrawColor(Black)
	re.Clear()

	re.SetDrawColor(BgColor)
	re.FillRect(&sdl.Rect{0, 0, DisplayWidth, DisplayHeight})

	re.SetDrawColor(PaddleColor)
	re.DrawRect(&c.paddle)

	for _, b := range c.blocks {
		if !b.Dead {
			re.SetDrawColor(b.Color)
			re.FillRect(&b.Rect)
		}
	}

	b := &c.ball
	sdlgfx.FilledCircle(re, int(b.X), int(b.Y), int(b.R), BallColor)

	re.Present()
}

type Block struct {
	Rect  sdl.Rect
	Color sdl.Color
	Dead  bool
}

type Ball struct {
	X, Y, R int32
	Moving  bool
	Vel     sdl.Point
	Speed   int32
}

func intersect(b *Ball, r *sdl.Rect) bool {
	x := clamp(b.X, r.X, r.X+r.W)
	y := clamp(b.Y, r.Y, r.Y+r.H)
	dx := b.X - x
	dy := b.Y - y
	return dx*dx+dy*dy < b.R*b.R
}

func clamp(x, s, e int32) int32 {
	if x < s {
		x = s
	}
	if x > e {
		x = e
	}
	return x
}
