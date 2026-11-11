// ported from suckless lsw
package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"strconv"

	"github.com/qeedquan/go-media/x11/xlib"
)

var (
	dpy       *xlib.Display
	netwmname xlib.Atom
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("lsw: ")
	flag.Usage = usage
	flag.Parse()
	dpy = xlib.OpenDisplay("")
	if dpy == nil {
		log.Fatal("cannot open display")
	}
	netwmname = xlib.InternAtom(dpy, "_NET_WM_NAME", false)

	if flag.NArg() < 1 {
		lsw(xlib.DefaultRootWindow(dpy))
	} else {
		for _, arg := range flag.Args() {
			win, err := strconv.ParseInt(arg, 0, 64)
			if err != nil {
				fmt.Fprintln(os.Stderr, "lsw:", err)
				continue
			}
			lsw(xlib.Window(win))
		}
	}
	xlib.CloseDisplay(dpy)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: lsw [options]")
	flag.PrintDefaults()
	os.Exit(2)
}

func lsw(win xlib.Window) {
	_, _, wins, err := xlib.QueryTree(dpy, win)
	if err != nil {
		fmt.Fprintln(os.Stderr, "lsw:", err)
		return
	}

	var wa xlib.WindowAttributes
	for i := len(wins) - 1; i >= 0; i-- {
		w := wins[i]
		err := xlib.GetWindowAttributes(dpy, w, &wa)
		if err != nil {
			fmt.Fprintln(os.Stderr, "lsw:", err)
			continue
		}
		if !wa.OverrideRedirect() && wa.MapState() == xlib.IsViewable {
			fmt.Printf("0x%07x %s\n", w, getname(w))
		}
	}
}

func getname(win xlib.Window) string {
	var prop xlib.TextProperty
	err := xlib.GetTextProperty(dpy, win, &prop, netwmname)
	if err != nil || prop.NumItems() == 0 {
		err = xlib.GetWMName(dpy, win, &prop)
	}
	if err != nil || prop.NumItems() == 0 {
		return ""
	}

	defer prop.Free()
	list, err := xlib.XmbTextPropertyToTextList(dpy, &prop)
	if err == nil && len(list) > 0 {
		return list[0]
	}

	return ""
}
