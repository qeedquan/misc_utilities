package main

import (
	"fmt"
	"log"

	"github.com/qeedquan/go-media/sdl"
)

func main() {
	err := sdl.Init(sdl.INIT_TIMER)
	if err != nil {
		log.Fatal(err)
	}

	t0 := sdl.GetPerformanceCounter()
	for {
		t1 := sdl.GetPerformanceCounter()
		dt := float64(t1-t0) / float64(sdl.GetPerformanceFrequency())
		fmt.Printf("%.6fs\n", dt)
	}
}
