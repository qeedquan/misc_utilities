package main

import (
	"fmt"
	"log"

	"github.com/qeedquan/go-media/x11/xlib"
)

func main() {
	dpy := xlib.OpenDisplay("")
	if dpy == nil {
		log.Fatal("failed to open display")
	}
	exts := xlib.ListExtensions(dpy)
	fmt.Println("Extensions: ")
	for _, ext := range exts {
		fmt.Println(ext)
	}
}
