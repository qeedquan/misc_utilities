// ported from suckless xsstate
package main

import (
	"flag"
	"fmt"
	"log"
	"os"

	"github.com/qeedquan/go-media/x11/xlib"
)

var (
	showidle  = flag.Bool("i", false, "show idle")
	showstate = flag.Bool("s", false, "show state")
	showtill  = flag.Bool("t", false, "show till")
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("xsstate: ")
	flag.Usage = usage
	flag.Parse()

	if !*showstate && !*showtill && !*showidle {
		usage()
	}

	dpy := xlib.OpenDisplay("")
	if dpy == nil {
		log.Fatal("cannot open display")
	}
	defer xlib.CloseDisplay(dpy)

	supported, _, _ := xlib.ScreenSaverQueryExtension(dpy)
	if !supported {
		log.Fatal("screensaver extension not activated")
	}

	info := xlib.ScreenSaverAllocInfo()
	err := xlib.ScreenSaverQueryInfo(dpy, xlib.Drawable(xlib.DefaultRootWindow(dpy)), info)
	if err != nil {
		log.Fatal(err)
	}

	switch {
	case *showstate:
		switch info.State() {
		case xlib.ScreenSaverOn:
			fmt.Println("on")
		case xlib.ScreenSaverOff:
			fmt.Println("off")
		case xlib.ScreenSaverDisabled:
			fmt.Println("disabled")
		}
	case *showtill:
		switch info.State() {
		case xlib.ScreenSaverOn:
			fmt.Println("0")
		case xlib.ScreenSaverOff:
			fmt.Println(info.TilOrSince())
		case xlib.ScreenSaverDisabled:
			fmt.Println("-1")
		}
	case *showidle:
		fmt.Println(info.Idle())
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options]")
	flag.PrintDefaults()
	os.Exit(2)
}
