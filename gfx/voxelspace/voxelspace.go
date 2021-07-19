// ported from https://github.com/s-macke/VoxelSpace
package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	"image/draw"
	"log"
	"math"
	"os"
	"runtime"

	"github.com/qeedquan/go-media/image/imageutil"
	"github.com/qeedquan/go-media/math/f64"
	"github.com/qeedquan/go-media/sdl"
	"github.com/qeedquan/go-media/sdl/sdlimage/sdlcolor"
)

var (
	window      *sdl.Window
	renderer    *sdl.Renderer
	terrain     Terrain
	texture     *sdl.Texture
	canvas      *image.RGBA
	location    f64.Vec2
	orientation float64
	elevation   float64
	distanceFOV float64
)

func main() {
	runtime.LockOSThread()
	log.SetFlags(0)
	log.SetPrefix("voxelspace: ")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 2 {
		usage()
	}

	err := terrain.LoadFile(flag.Arg(0), flag.Arg(1))
	ck(err)

	initSDL()
	reset()
	for {
		event()
		blit()
	}
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] colormap heightmap")
	flag.PrintDefaults()
	os.Exit(2)
}

func initSDL() {
	err := sdl.Init(sdl.INIT_VIDEO | sdl.INIT_TIMER)
	ck(err)

	sdl.SetHint(sdl.HINT_RENDER_SCALE_QUALITY, "best")
	w, h := 700, 500
	wflag := sdl.WINDOW_RESIZABLE
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, wflag)
	ck(err)

	window.SetTitle("Voxel Space")

	resizeWindow(w, h)
}

func reset() {
	location = f64.Vec2{670, 500}
	orientation = 0
	elevation = 120
	distanceFOV = 800
}

func moveLocation(step f64.Vec2) f64.Vec2 {
	s, c := math.Sincos(orientation)
	M := f64.Mat2{
		{c, -s},
		{s, c},
	}
	dir := M.Transform(step)
	return location.Add(dir)
}

func event() {
	const step = 5
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
			case sdl.K_DOWN:
				location = moveLocation(f64.Vec2{0, step})
			case sdl.K_UP:
				location = moveLocation(f64.Vec2{0, -step})
			case sdl.K_LEFT:
				location = moveLocation(f64.Vec2{-step, 0})
			case sdl.K_RIGHT:
				location = moveLocation(f64.Vec2{step, 0})
			case sdl.K_q:
				orientation -= 0.1
			case sdl.K_w:
				orientation += 0.1
			case sdl.K_1:
				elevation += step
			case sdl.K_2:
				elevation -= step
			case sdl.K_a:
				distanceFOV += step
			case sdl.K_s:
				distanceFOV -= step
			case sdl.K_SPACE:
				reset()
			}
		case sdl.WindowEvent:
			switch ev.Event {
			case sdl.WINDOWEVENT_RESIZED:
				w, h := int(ev.Data[0]), int(ev.Data[1])
				resizeWindow(w, h)
			}
		}
	}
}

func resizeWindow(w, h int) {
	var err error
	texture, err = renderer.CreateTexture(sdl.PIXELFORMAT_ABGR8888, sdl.TEXTUREACCESS_STREAMING, w, h)
	ck(err)

	canvas = image.NewRGBA(image.Rect(0, 0, w, h))
}

func blit() {
	renderer.SetDrawColor(sdlcolor.Black)
	renderer.Clear()
	draw.Draw(canvas, canvas.Bounds(), image.NewUniform(color.RGBA{0x66, 0xa3, 0xff, 0xff}), image.ZP, draw.Over)
	blitTerrain()
	texture.Update(nil, canvas.Pix, canvas.Stride)
	renderer.Copy(texture, nil, nil)
	renderer.Present()
}

func blitTerrain() {
	terrain.DrawFrontToBack(canvas, location, orientation, elevation, distanceFOV, location)
}

type Terrain struct {
	colormap  *image.RGBA
	heightmap *image.RGBA
	palette   []color.RGBA
	hidden    []float64
}

func (c *Terrain) LoadFile(colormap, heightmap string) error {
	var err error
	c.colormap, err = imageutil.LoadRGBAFile(colormap)
	if err != nil {
		return err
	}

	c.heightmap, err = imageutil.LoadRGBAFile(heightmap)
	if err != nil {
		return err
	}

	return nil
}

func (c *Terrain) clear(m *image.RGBA) {
	r := m.Bounds()
	for y := r.Min.Y; y < r.Max.Y; y++ {
		for x := r.Min.X; x < r.Max.X; x++ {
			m.Set(x, y, color.RGBA{0x66, 0xa3, 0xff, 0xff})
		}
	}

	w := r.Dx()
	h := r.Dy()
	if len(c.hidden) < w {
		c.hidden = make([]float64, w)
	}
	for i := range c.hidden {
		c.hidden[i] = float64(h)
	}
}

func (c *Terrain) vline(m *image.RGBA, x, y1, y2 int, col color.RGBA) {
	if y1 >= y2 {
		return
	}
	if y1 < 0 {
		y1 = 0
	}

	for y := y1; y < y2; y++ {
		m.SetRGBA(x, y, col)
	}
	return
}

func (c *Terrain) hline(m *image.RGBA, p1, p2 f64.Vec2, offset, scale, horizon float64, pmap f64.Vec2) {
	w, _, _ := renderer.OutputSize()
	n := w

	// we always draw the size of the screen width, as in every pixel on a scanline gets a color
	// but we step along the colormap/heightmap based on the bounds p1 and p2, and we also wrap around
	// if we are out of bounds

	// from point p1 to p2, determine the slope for stepping size, we step 1/width_of_screen to touch all pixel
	// of the scanline
	dx := (p2.X - p1.X) / float64(n)
	dy := (p2.Y - p1.Y) / float64(n)
	for i := 0; i < n; i++ {
		// xi and yi are the coordinates inside the heightmap/colormap
		// the heightmap/colormap have to be the same image size
		xi := int(math.Floor(p1.X)) & 1023
		yi := int(math.Floor(p1.Y)) & 1023
		hc := c.heightmap.RGBAAt(xi, yi)
		col := c.colormap.RGBAAt(xi, yi)

		// now we calculate height based on the heightmap, we translate it a little with the
		// height offset, if the height is higher we get a more eagle view of the scene
		// the scale is for scaling the height distance and horizon is just a translation vector
		// this correspond to the starting y value
		// the color value in the heightmap corresponds to the elevation value
		// we also do perspective divide here (in scale)
		height := (float64(hc.R)+offset)*scale + horizon

		// now we sampled the point from height/colormap for this scanline, draw a vertical line
		// from starting height to the hidden buffer
		// the hidden buffer kinds of acts like a zbuffer but for the y axis, so we don't have to draw
		// back to front (painter algorithm)

		// instead of drawing from back to the front we can draw from front to back.
		// The advantage is, the we don't have to draw lines to the bottom of the screen every time because of occlusion.
		// However, to guarantee occlusion we need an additional y-buffer.
		// For every column, the highest y position is stored.
		// Because we are drawing from the front to back, the visible part of the next line can
		// only be larger then the highest line previously drawn.

		c.vline(m, i, int(height), int(c.hidden[i]), col)
		if height < c.hidden[i] {
			c.hidden[i] = height
		}
		p1.X += dx
		p1.Y += dy
	}
}

func (c *Terrain) DrawFrontToBack(m *image.RGBA, p f64.Vec2, phi, height, distance float64, pmap f64.Vec2) {
	c.clear(m)

	// algorithm works starting at the z location and looking forward "distance" ahead,
	// the more distance is, the more one see on the horizon, then for each z, draw
	// a horizontal line that is increasing size (this makes a view frustum) and anything
	// in the view frustum is drawn
	dz := 1.0
	z := 5.0
	for z < distance {
		// the view frustum we see is a triangle, our orientation/current z value determines where to sample
		// in the color and height map and how much
		pl := f64.Vec2{-z, -z}
		pr := f64.Vec2{z, -z}
		pl = pl.Rotate(phi)
		pr = pr.Rotate(phi)

		// now we have the left and right bounds of the horizontal line, draw it
		p1 := f64.Vec2{p.X + pl.X, p.Y + pl.Y}
		p2 := f64.Vec2{p.X + pr.X, p.Y + pr.Y}

		c.hline(m, p1, p2, -height, -1/z*240, 100, pmap)
		z += dz
	}
}
