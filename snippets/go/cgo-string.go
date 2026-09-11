package main

// #include <stdio.h>
import "C"
import (
	"fmt"
	"runtime"
	"unsafe"
)

func main() {
	for i := 0; i < 100; i++ {
		go func(n int) {
			b := []byte(fmt.Sprintf("hello%d\x00world", n))
			for {
				f(b)
			}
		}(i)
	}

	for {
		runtime.GC()
	}
}

func f(s []byte) {
	C.puts((*C.char)(unsafe.Pointer(&s[0])))
}
