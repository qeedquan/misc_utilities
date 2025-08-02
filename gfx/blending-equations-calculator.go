// http://nothings.org/gamedev/blend_equations_with_destination_alpha.txt
package main

import (
	"flag"
	"fmt"
	"math"
	"os"
	"strconv"
)

func main() {
	premul := flag.Bool("ps", true, "premultiplied source")
	flag.Parse()
	if flag.NArg() != 4 {
		usage()
	}
	sc, _ := strconv.ParseFloat(flag.Arg(0), 64)
	sa, _ := strconv.ParseFloat(flag.Arg(1), 64)
	dc, _ := strconv.ParseFloat(flag.Arg(2), 64)
	da, _ := strconv.ParseFloat(flag.Arg(3), 64)

	tab := []struct {
		name string
		typ  int
	}{
		{"layer/over", LAYER},
		{"multiply", MULTIPLY},
		{"screen", SCREEN},
		{"lighten", LIGHTEN},
		{"darken", DARKEN},
		{"add", ADD},
		{"subtract", SUBTRACT},
		{"difference", DIFFERENCE},
		{"invert", INVERT},
		{"overlay", OVERLAY},
		{"hardlight", HARDLIGHT},
	}
	fmt.Printf("sc %.6f sa %.6f dc %.6f da %.6f\n", sc, sa, dc, da)
	for _, t := range tab {
		oc, oa := mix(t.typ, sc, sa, dc, da, *premul)
		fmt.Printf("%-16s| % .6f % .6f\n", t.name, oc, oa)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] sc sa dc da")
	flag.PrintDefaults()
	os.Exit(2)
}

const (
	LAYER = iota + 1
	MULTIPLY
	SCREEN
	LIGHTEN
	DARKEN
	ADD
	SUBTRACT
	DIFFERENCE
	INVERT
	OVERLAY
	HARDLIGHT
)

func mix(typ int, sc, sa, dc, da float64, premul bool) (oc, oa float64) {
	if !premul {
		sc *= sa
	}

	oa = sa + da - sa*da
	switch typ {
	case LAYER:
		oc = sc + (1-sa)*dc
	case MULTIPLY:
		oc = sc * dc
	case SCREEN:
		oc = sa*da - (da-dc)*(sa-sc)
	case LIGHTEN:
		oc = math.Max(sa*dc, sc*da)
	case DARKEN:
		oc = math.Min(sa*dc, sc*da)
	case ADD:
		oc = math.Min(dc+sc, 1)
		oa = math.Min(sa+da, 1)
	case SUBTRACT:
		oc = math.Max(dc-sc, 0)
		oa = math.Min(sa+da, 1)
	case DIFFERENCE:
		oc = math.Abs(sa*dc - sc*da)
	case INVERT:
		oc = sa * (da - dc)
	case OVERLAY:
		if dc < da/2.0 {
			oc = 2.0 * sc * dc
		} else {
			oc = sa*da - 2.0*(da-dc)*(sa-sc)
		}
	case HARDLIGHT:
		if sc < sa/2.0 {
			oc = 2.0 * sc * dc
		} else {
			oc = sa*da - 2.0*(da-dc)*(sa-sc)
		}
	}
	return
}
