package main

import (
	"flag"
	"fmt"
	"io"
	"math"
	"os"
	"strconv"
)

func main() {
	flag.Parse()
	stats(os.Stdin)
}

func stats(r io.Reader) {
	min := math.MaxFloat64
	max := -math.MaxFloat64
	once := false
	for {
		var s string
		_, err := fmt.Fscanf(r, "%s", &s)
		if err != nil {
			break
		}

		v, err := strconv.ParseFloat(s, 64)
		if err != nil {
			continue
		}

		min = math.Min(min, v)
		max = math.Max(max, v)
		once = true
	}
	if !once {
		return
	}

	fmt.Printf("min: %v\n", min)
	fmt.Printf("max: %v\n", max)
}
