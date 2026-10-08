// ported from suckless swarp
package main

import (
	"flag"
	"fmt"
	"log"
	"os"

	"github.com/qeedquan/go-media/x11/xlib"
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("swarp:")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 2 {
		usage()
	}

	dpy := xlib.OpenDisplay("")
	if dpy == nil {
		log.Fatal("cannot open display")
	}
	defer xlib.CloseDisplay(dpy)

	var x, y int
	nx, _ := fmt.Sscanf(flag.Arg(0), "%d", &x)
	ny, _ := fmt.Sscanf(flag.Arg(1), "%d", &y)
	if nx != 1 || ny != 1 {
		usage()
	}
	xlib.WarpPointer(dpy, xlib.None, xlib.RootWindow(dpy, xlib.DefaultScreen(dpy)), 0, 0, 0, 0, x, y)
	xlib.SetInputFocus(dpy, xlib.PointerRoot, xlib.RevertToPointerRoot, xlib.CurrentTime)
}

func usage() {
	fmt.Fprintln(os.Stderr, "swarp: [options] <x> <y>")
	flag.PrintDefaults()
	os.Exit(2)
}
