// ported from suckless sselp
package main

import (
	"flag"
	"fmt"
	"log"
	"os"

	"github.com/qeedquan/go-media/x11/xlib"
)

func main() {
	flag.Usage = usage
	flag.Parse()

	dpy := xlib.OpenDisplay("")
	if dpy == nil {
		log.Fatal("can't open display")
	}
	defer xlib.CloseDisplay(dpy)

	utf8 := xlib.InternAtom(dpy, "UTF8_STRING", false)
	clip := xlib.InternAtom(dpy, "_SSELP_STRING", false)
	win := xlib.CreateSimpleWindow(dpy, xlib.DefaultRootWindow(dpy), 0, 0, 1, 1, 0, xlib.CopyFromParent, xlib.CopyFromParent)
	xlib.ConvertSelection(dpy, xlib.XA_PRIMARY, utf8, clip, win, xlib.CurrentTime)

	var ev xlib.Event
	xlib.NextEvent(dpy, &ev)
	if ev.Type() != xlib.SelectionNotify {
		return
	}

	sel := ev.Selection()
	if sel.Property() != xlib.None {
		off := 0
		for {
			_, _, length, more, data, _ := xlib.GetWindowProperty(dpy, win, sel.Property(), off, 8192, false, utf8)
			os.Stdout.Write(data)
			if more == 0 {
				break
			}
			off += length / 4
		}
		fmt.Println()
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: sselp [options]")
	flag.PrintDefaults()
	os.Exit(2)
}
