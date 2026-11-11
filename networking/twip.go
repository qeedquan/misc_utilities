// ported from http://dunkels.com/adam/twip.html
package main

import "os"

func main() {
	var b [4096]byte
	for {
		n, err := os.Stdin.Read(b[:])
		if err != nil {
			break
		}

		if n < 24 {
			continue
		}

		if b[9] == 1 && b[20] == 8 && b[21] == 0 {
			b[20] = 0
			put2(b[22:], get2(b[22:])+8)

			s := get4(b[12:])
			d := get4(b[16:])
			put4(b[12:], d)
			put4(b[16:], s)

			os.Stdout.Write(b[:n])
		}
	}
}

func get2(b []byte) uint16 {
	return uint16(b[0]) | uint16(b[1])<<8
}

func put2(b []byte, v uint16) {
	b[0] = byte(v)
	b[1] = byte(v >> 8)
}

func get4(b []byte) uint32 {
	return uint32(b[0]) | uint32(b[1])<<8 | uint32(b[2])<<16 | uint32(b[3])<<24
}

func put4(b []byte, v uint32) {
	b[0] = byte(v)
	b[1] = byte(v >> 8)
	b[2] = byte(v >> 16)
	b[3] = byte(v >> 24)
}
