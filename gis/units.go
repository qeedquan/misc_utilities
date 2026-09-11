package main

import (
	"flag"
	"fmt"
	"log"
	"math"
	"os"
	"strconv"
)

func main() {
	flag.Parse()
	if flag.NArg() != 3 {
		usage()
	}

	val, _ := strconv.ParseFloat(flag.Arg(0), 64)
	from := flag.Arg(1)
	to := flag.Arg(2)
	val, err := conv(val, from, to)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Println(val)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: value from to")
	flag.PrintDefaults()
	fmt.Fprintln(os.Stderr, "")
	fmt.Fprintln(os.Stderr, "available conversions:")
	fmt.Fprintln(os.Stderr, "arcsec deg rad")
	fmt.Fprintln(os.Stderr, "nautical_mile meter mile")
	os.Exit(2)
}

func unitcode(unit string) int {
	switch unit {
	case "arcsec", "deg", "rad":
		return 1
	case "nautical_mile", "meter", "mile":
		return 2
	}
	return 0
}

func conv(val float64, from, to string) (float64, error) {
	if unitcode(from) != unitcode(to) {
		return 0, fmt.Errorf("conversion %q to %q not supported", from, to)
	}

	var x, y float64
	switch from {
	case "rad":
		x = val
	case "deg":
		x = deg2rad(val)
	case "arcsec":
		x = arcsec2rad(val)

	case "nautical_mile":
		x = nautical2meter(val)
	case "mile":
		x = mile2meter(val)
	case "meter":
		x = val

	default:
		return 0, fmt.Errorf("unsupported type: %v", from)
	}

	switch to {
	case "rad":
		y = x
	case "deg":
		y = rad2deg(x)
	case "arcsec":
		y = rad2arcsec(x)

	case "nautical_mile":
		y = meter2nautical(x)
	case "mile":
		y = meter2mile(x)
	case "meter":
		y = x

	default:
		return 0, fmt.Errorf("unsupported type: %v", to)
	}

	return y, nil
}

// https://en.wikipedia.org/wiki/Minute_and_second_of_arc
func arcsec2deg(a float64) float64 { return a / 3600 }
func deg2arcsec(a float64) float64 { return a * 3600 }

func arcsec2rad(a float64) float64 { return a * math.Pi / 648000 }
func rad2arcsec(r float64) float64 { return r * 648000 / math.Pi }

func deg2rad(d float64) float64 { return d * math.Pi / 180 }
func rad2deg(r float64) float64 { return r * 180 / math.Pi }

// https://en.wikipedia.org/wiki/Nautical_mile
func nautical2meter(n float64) float64 { return 1852 * n }
func meter2nautical(m float64) float64 { return m / 1852 }

func meter2mile(m float64) float64 { return m / 1609.344 }
func mile2meter(m float64) float64 { return m * 1609.344 }
