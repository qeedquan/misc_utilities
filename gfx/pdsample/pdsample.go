package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"strconv"
)

var (
	maximize     = flag.Bool("m", false, "maximize point set after sampling")
	isTiled      = flag.Bool("t", false, "use tiled domain")
	multiplier   = flag.Int("M", 1, "set multiplier for DartThrowing and BestCandidate samplers")
	minMaxThrows = flag.Int("N", 1000, "set minimum of maximum throws for DartThrowing sampler")
	method       string
	radius       float64
)

func main() {
	log.SetPrefix("pdsample: ")
	log.SetFlags(0)
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 2 {
		usage()
	}

	method = flag.Arg(0)
	radius, _ = strconv.ParseFloat(flag.Arg(1), 64)

	var sampler Interface
	switch method {
	case "DartThrowing":
		sampler = NewDartThrowing(radius, *isTiled, *minMaxThrows, *multiplier)
	case "BestCandidate":
		sampler = NewBestCandidate(radius, *isTiled, *multiplier)
	case "Boundary":
		sampler = NewBoundarySampler(radius, *isTiled)
	case "Pure":
		if !*isTiled {
			log.Fatalf("Pure sampler does not support untiled domain")
		}
		sampler = NewPureSampler(radius)
	case "LinearPure":
		if !*isTiled {
			log.Fatalf("LinearPure sampler does not support untiled domain")
		}
		sampler = NewLinearPureSampler(radius)
	case "Penrose":
		sampler = NewPenroseSampler(radius)
	case "Uniform":
		sampler = NewUniformSampler(radius)
	default:
		log.Fatalf("Unrecognized sampling method (%s)", method)
	}

	sampler.Complete()
	if *maximize {
		sampler.Maximize()
	}
	pts := sampler.Points()
	for _, p := range pts {
		fmt.Printf("%v %v\n", p.X, p.Y)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] <method> <radius>")
	flag.PrintDefaults()
	fmt.Fprintln(os.Stderr)
	fmt.Fprintln(os.Stderr, "available methods:")
	fmt.Fprintln(os.Stderr, "\tDartThrowing (uses multiplier and minMaxThrows)")
	fmt.Fprintln(os.Stderr, "\tBestCandidate (uses multiplier)")
	fmt.Fprintln(os.Stderr, "\tBoundary")
	fmt.Fprintln(os.Stderr, "\tPure")
	fmt.Fprintln(os.Stderr, "\tLinearPure")
	fmt.Fprintln(os.Stderr, "\tPenrose")
	fmt.Fprintln(os.Stderr, "\tUniform")
	os.Exit(2)
}
