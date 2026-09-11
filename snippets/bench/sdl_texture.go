package main

import (
	"flag"
	"fmt"
	"log"
	"runtime"

	"github.com/qeedquan/go-media/sdl"
)

var (
	conf struct {
		access     string
		format     string
		iterations uint64
		texw       int
		texh       int
	}
	window   *sdl.Window
	renderer *sdl.Renderer
)

func main() {
	runtime.LockOSThread()
	parseFlags()
	initSDL()
	bench()
}

func parseFlags() {
	flag.Uint64Var(&conf.iterations, "i", 1000, "number of iterations")
	flag.StringVar(&conf.format, "f", "argb8888", "texture format")
	flag.StringVar(&conf.access, "a", "target", "texture mode")
	flag.IntVar(&conf.texw, "w", 4096, "texture width")
	flag.IntVar(&conf.texh, "h", 4096, "texture height")
	flag.Parse()

	tf(conf.format)
	ta(conf.access)
}

func tf(format string) uint32 {
	switch format {
	case "argb8888":
		return sdl.PIXELFORMAT_ARGB8888
	}
	log.Fatalf("unknown texture format %q", format)
	return 0
}

func ta(access string) sdl.TextureAccess {
	switch access {
	case "streaming":
		return sdl.TEXTUREACCESS_STREAMING
	case "target":
		return sdl.TEXTUREACCESS_TARGET
	}
	log.Fatalf("unknown texture access %q", access)
	return 0
}

func initSDL() {
	err := sdl.Init(sdl.INIT_VIDEO)
	ck(err)

	dm, err := sdl.GetDisplayMode(0, 0)
	ck(err)

	w, h := dm.W/2, dm.H/2
	wflag := sdl.WINDOW_RESIZABLE
	window, renderer, err = sdl.CreateWindowAndRenderer(w, h, wflag)
	ck(err)

	window.SetTitle("Texture Creation Benchmark")
	renderer.Present()
}

func bench() {
	tw, th := conf.texw, conf.texh
	format := tf(conf.format)
	access := ta(conf.access)
	fmt.Printf("dimension  :  %dx%d\n", tw, th)
	fmt.Printf("iterations :  %d\n", conf.iterations)
	fmt.Printf("format     :  %s\n", conf.format)
	fmt.Printf("access     :  %s\n", conf.access)
	for n := uint64(0); n < conf.iterations; n++ {
		if n > 0 && n%100 == 0 {
			fmt.Printf("%d iterations\n", n)
		}
		_, err := renderer.CreateTexture(format, access, tw, th)
		if err != nil {
			fmt.Printf("err: %v\n", err)
			fmt.Printf("made %d textures\n", n)
			return
		}
	}
	fmt.Printf("finished %d iterations\n", conf.iterations)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}
