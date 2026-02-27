// ported from https://github.com/jcs/xbanish
package main

import (
	"flag"
	"fmt"
	"log"
	"strings"

	"github.com/qeedquan/go-media/x11/xlib"
	"github.com/qeedquan/go-media/x11/xlib/xfixes"
	"github.com/qeedquan/go-media/x11/xlib/xi"
)

var (
	button_press_type   = -1
	button_release_type = -1
	key_press_type      = -1
	key_release_type    = -1
	motion_type         = -1

	dpy     *xlib.Display
	debug   bool
	hiding  bool
	legacy  bool
	ignored uint
)

type MultiFlag []string

func (m *MultiFlag) String() string {
	return fmt.Sprint(*m)
}

func (m *MultiFlag) Set(s string) error {
	*m = append(*m, s)
	return nil
}

func main() {
	log.SetFlags(0)
	log.SetPrefix("")

	var mods MultiFlag
	flag.Var(&mods, "i", "set modifiers")
	flag.BoolVar(&debug, "d", false, "enable debug")
	flag.Parse()
	modLookups := map[string]uint{
		"shift":   xlib.ShiftMask,
		"lock":    xlib.LockMask,
		"control": xlib.ControlMask,
		"mod1":    xlib.Mod1Mask,
		"mod2":    xlib.Mod2Mask,
		"mod3":    xlib.Mod3Mask,
		"mod4":    xlib.Mod4Mask,
		"mod5":    xlib.Mod5Mask,
	}
	for _, m := range mods {
		if mask, found := modLookups[strings.ToLower(m)]; found {
			ignored |= mask
		}
	}

	dpy = xlib.OpenDisplay("")
	if dpy == nil {
		log.Fatal("can't open display", xlib.DisplayName(""))
	}

	xlib.SetErrorHandler(swallowError)

	if snoopXInput(xlib.DefaultRootWindow(dpy)) == 0 {
		debugf("no XInput devices found, using legacy snooping")
		legacy = true
		snoopLegacy(xlib.DefaultRootWindow(dpy))
	}

	var e xlib.Event
	for {
		cookie := e.Cookie()
		xlib.NextEvent(dpy, &e)
		etype := e.Type()
		switch e.Type() {
		case motion_type:
			etype = xlib.MotionNotify
		case key_press_type, key_release_type:
			etype = xlib.KeyRelease
		case button_press_type, button_release_type:
			etype = xlib.ButtonRelease
		}

		switch etype {
		case xlib.KeyRelease:
			if ignored != 0 {
				// masks are only set on key release, if
				// ignore is set we must throw out non-release
				// events here
				if e.Type() == key_press_type {
					break
				}

				// extract modifier state
				var state uint
				if e.Type() == key_release_type {
					// xinput device event
					state = e.Key().State()
				} else if e.Type() == xlib.KeyRelease {
					// legacy event
					state = e.Key().State()
				}
				if state&ignored != 0 {
					debugf("ignoring key %d", state)
					break
				}
			}
			hideCursor()

		case xlib.ButtonRelease, xlib.MotionNotify:
			showCursor()

		case xlib.CreateNotify:
			if legacy {
				debugf("created new window, snooping on it")

				// not sure why snooping directly on the window
				// doesn't work, so snoop on all windows from
				// its parent (probably root)
				snoopLegacy(e.CreateWindow().Parent())
			}

		case xlib.GenericEvent:
			// xi2 raw event
			xlib.GetEventData(dpy, cookie)
			xie := (*xi.DeviceEvent)(cookie.Data())
			switch xie.EvType() {
			case xi.RawMotion, xi.RawButtonPress:
				showCursor()
			case xi.RawButtonRelease:
			default:
				debugf("unknown XI event type %d", xie.EvType())
			}
			xlib.FreeEventData(dpy, cookie)

		default:
			debugf("unknown event type %d", e.Type())
		}
	}
}

func hideCursor() {
	if hiding {
		debugf("keystroke, already hiding cursor")
	} else {
		debugf("keystroke, hiding cursor")
	}

	if !hiding {
		xfixes.HideCursor(dpy, xlib.DefaultRootWindow(dpy))
		hiding = true
	}
}

func showCursor() {
	if hiding {
		debugf("mouse moved, already unhiding cursor")
	} else {
		debugf("mouse moved, unhiding cursor")
	}

	if hiding {
		xfixes.ShowCursor(dpy, xlib.DefaultRootWindow(dpy))
		hiding = false
	}
}

func snoopXInput(win xlib.Window) int {
	supported, _, _, _ := xlib.QueryExtension(dpy, "XInputExtension")
	if !supported {
		warnf("XInput extension not available")
		return 0
	}

	rawmotion := false
	major, minor := 2, 2
	major, minor, err := xi.QueryVersion(dpy, major, minor)
	if err == nil {
		var mask [(xi.LASTEVENT + 7) / 8]uint8
		xi.SetMask(mask[:], xi.RawMotion)
		xi.SetMask(mask[:], xi.RawButtonPress)
		evmasks := [1]xi.EventMask{
			{xi.AllMasterDevices, mask[:]},
		}
		xi.SelectEvents(dpy, win, evmasks[:])
		xlib.Flush(dpy)

		rawmotion = true

		debugf("using xinput2 raw motion events")
	}

	devinfo := xi.ListInputDevices(dpy)
	event_list := make([]xi.EventClass, len(devinfo)*2)
	ev := 0
	for _, d := range devinfo {
		if d.Use() != xi.IsXExtensionKeyboard && d.Use() != xi.IsXExtensionPointer {
			continue
		}
		device := xi.OpenDevice(dpy, d.ID())
		if device == nil {
			break
		}

		icis := device.Classes()
		for _, ici := range icis {
			switch ici.InputClass() {
			case xi.KeyClass:
				debugf("attaching to keyboard device %s (use %d)", d.Name(), d.Use())

				key_press_type, event_list[ev] = xi.DeviceKeyPress(device, key_press_type, event_list[ev])
				ev++
				key_release_type, event_list[ev] = xi.DeviceKeyRelease(device, key_release_type, event_list[ev])
				ev++

			case xi.ButtonClass:
				if rawmotion {
					continue
				}

				debugf("attaching to buttoned device %s (use %d)", d.Name(), d.Use())

				button_press_type, event_list[ev] = xi.DeviceButtonPress(device, button_press_type, event_list[ev])
				ev++
				button_release_type, event_list[ev] = xi.DeviceButtonRelease(device, button_release_type, event_list[ev])
				ev++

			case xi.ValuatorClass:
				if rawmotion {
					continue
				}

				debugf("attaching to pointing device %s (use %d)", d.Name(), d.Use())

				motion_type, event_list[ev] = xi.DeviceMotionNotify(device, motion_type, event_list[ev])
				ev++
			}
		}
		err := xi.SelectExtensionEvent(dpy, win, event_list[:ev])
		if err != nil {
			warnf("error selecting extension events")
		}
	}

	return ev
}

func snoopLegacy(win xlib.Window) {
	var typ int64 = xlib.PointerMotionMask | xlib.KeyReleaseMask | xlib.Button1MotionMask |
		xlib.Button2MotionMask | xlib.Button3MotionMask | xlib.Button4MotionMask |
		xlib.Button5MotionMask | xlib.ButtonMotionMask

	root, _, kids, err := xlib.QueryTree(dpy, win)
	if err != nil {
		warnf("can't query window tree")
		return
	}

	xlib.SelectInput(dpy, root, typ)

	// listen for newly mapped windows
	var sattrs xlib.SetWindowAttributes
	sattrs.SetEventMask(xlib.SubstructureNotifyMask)
	xlib.ChangeWindowAttributes(dpy, root, xlib.CWEventMask, &sattrs)
	for i := range kids {
		xlib.SelectInput(dpy, kids[i], typ)
		snoopLegacy(kids[i])
	}
}

func debugf(format string, args ...interface{}) {
	if debug {
		fmt.Printf(format+"\n", args...)
	}
}

func warnf(format string, args ...interface{}) {
	fmt.Printf(format+"\n", args...)
}

func swallowError(d *xlib.Display, e *xlib.ErrorEvent) int {
	switch code := e.ErrorCode(); {
	case code == xlib.BadWindow:
		// no biggie
		return 0
	case code&xlib.FirstExtensionError != 0:
		return 0
	default:
		log.Fatal("got X error %d", code)
	}
	return 0
}
