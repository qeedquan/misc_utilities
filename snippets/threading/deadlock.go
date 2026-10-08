// deadlock via bad lock ordering
package main

import (
	"flag"
	"fmt"
	"sync"
	"time"
)

var (
	nthreads = flag.Uint("t", 2, "number of threads")
	nlocks   = flag.Uint("l", 2, "number of locks")
)

func main() {
	flag.Parse()
	lk := make([]sync.Mutex, *nlocks)

	fmt.Printf("threads %d locks %d\n", *nthreads, *nlocks)
	var wg sync.WaitGroup
	for i := uint(0); i < *nthreads; i++ {
		wg.Add(1)
		go deadlocker(&wg, i, lk)
	}
	wg.Wait()
}

func deadlocker(wg *sync.WaitGroup, id uint, lk []sync.Mutex) {
	defer wg.Done()

	const N = 1000
	for n := 0; n < N; n++ {
		i := int(id) % len(lk)
		j := int(id+1) % len(lk)

		lk[i].Lock()
		lk[j].Lock()

		time.Sleep(100 * time.Millisecond)

		lk[j].Unlock()
		lk[i].Unlock()
	}
}
