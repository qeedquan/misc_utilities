// https://www.mscs.dal.ca/~selinger/random/

package main

/*
#include <stdlib.h>
*/
import "C"
import (
	"fmt"
	"math"
	"time"
)

func main() {
	var l LCG
	l.Seed(1)
	C.srand(1)

	t0 := time.Now()
	for i := 0; i != math.MaxInt; i++ {
		a := l.Int31()
		b := C.rand()
		if a != int32(b) {
			fmt.Println("MISMATCH", i, a, b)
		}
		if i > 0 && i%1e8 == 0 {
			t1 := time.Now()
			fmt.Println(t1.Sub(t0), i)
			t0 = t1
		}
	}
}

type LCG struct {
	r [34]int32
	i int
}

func (l *LCG) Seed(seed int32) {
	l.r[0] = seed
	for i := 1; i < 31; i++ {
		l.r[i] = int32(16807 * int64(l.r[i-1]) % 2147483647)
		if l.r[i] < 0 {
			l.r[i] += 2147483647
		}
	}

	for i := 31; i < 34; i++ {
		l.r[i] = l.r[i-31]
	}

	l.i = 34
	for i := 34; i < 344; i++ {
		l.Int31()
	}
}

func (l *LCG) Int31() int32 {
	n := len(l.r)
	i := mod(l.i-31, n)
	j := mod(l.i-3, n)
	p := mod(l.i, n)

	l.r[p] = l.r[i] + l.r[j]
	l.i = (l.i + 1) % n

	return int32(uint32(l.r[p]) >> 1)
}

func mod(x, m int) int {
	x %= m
	if x < 0 {
		x += m
	}
	return x
}
