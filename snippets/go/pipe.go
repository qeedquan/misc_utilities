package main

import (
	"flag"
	"log"
	"os"
	"time"
)

var (
	pairs = flag.Int("n", 1, "number of reader/writer pairs")
	size  = flag.Int("b", 8192, "buffer size")
)

func main() {
	flag.Parse()
	for i := 0; i < *pairs; i++ {
		r, w, err := os.Pipe()
		if err != nil {
			log.Fatal(err)
		}
		go reader(i, r, *size)
		go writer(i, w, *size)
	}
	select {}
}

func reader(id int, r *os.File, size int) {
	buf := make([]byte, size)
	tot := 0
	t0 := time.Now()
	for {
		nr, _ := r.Read(buf)
		tot += nr
		t1 := time.Now()
		dt := t1.Sub(t0)
		if dt >= 1*time.Second {
			t0 = t1
			log.Printf("%d: %d (%d gb) bytes read", id, tot, tot/(1024*1024))
		}
	}
}

func writer(id int, w *os.File, size int) {
	buf := make([]byte, size)
	for {
		w.Write(buf)
	}
}
