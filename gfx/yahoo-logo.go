// ported from https://www.a1k0n.net/2011/06/26/obfuscated-c-yahoo-logo.html
package main

import (
	"fmt"
	"math"
	"time"
)

func main() {
	logo()
}

func logo() {
	const (
		S1 = " ''\".$u$"
		S2 = "<[\\]O=IKNAL;KNRbF8EbGEROQ@BSXXtG!#t3!^\x00"
		S3 = "<AFJPTX\x00"
		S4 = "O:85!fI,wfO8!yZfO8!f*hXK3&fO;:O;#hP;\"i[by asloane"
	)

	var (
		F = 40
		S = 0.0
		V = 0.0
	)
	for ; F > 0; F-- {
		V += (1-S)/10 - V/4
		S += V
		for j := 0; j < 72; j += 3 {
			for i := 0; i < 73; i++ {
				x := S * float64(i-27)
				c := 0
				for n := 2; n >= 0; n-- {
					y := S * float64(j+n-36)
					if (136*x*x + 84*y*y) < 92033 {
						c ^= 1 << uint(n)
					}
					p := 6
					k := 0
					for m := 0; m < 8; {
						a := (float64(S4[k]) - 79) / 14.64
						k++
						if float64(S2[k-1])/1.16-68 > x*math.Cos(a)+y*math.Sin(a) {
							k = p
							p = int(S3[m]) - 50
							m++
						} else if k == p {
							c ^= 1 << uint(n)
							m = 8
						}
					}
				}
				fmt.Printf("%c", S1[c])
			}
			fmt.Printf("\n")
		}
		fmt.Printf("\x1b[25A\n")
		time.Sleep(50000 * time.Microsecond)
	}
}
