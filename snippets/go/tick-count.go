package main

import (
	"flag"
	"fmt"
	"time"
)

var (
	delay = flag.Duration("d", 5*time.Millisecond, "delay")
)

func main() {
	flag.Parse()

	t0 := time.Now()
	for ct := 0; ; ct++ {
		time.Sleep(*delay)
		t1 := time.Now()
		dt := t1.Sub(t0)
		if dt >= 1*time.Second {
			fmt.Println(t1, ct)
			t0 = t1
			ct = 0
		}
	}
}
