// https://en.wikipedia.org/wiki/Xorshift
package main

import (
	"fmt"
	"math/rand"
	"time"
)

type xorshift struct {
	a, b, c, d uint32
	counter    uint32
}

func (p *xorshift) Uint32() uint32 {
	x := p.a
	x ^= x << 13
	x ^= x >> 17
	x ^= x << 5
	p.a = x
	return x
}

func (p *xorshift) Uint64() uint64 {
	x := uint64(p.a) | uint64(p.b)<<32
	x ^= x << 13
	x ^= x >> 7
	x ^= x << 17
	p.a = uint32(x)
	p.b = uint32(x >> 32)
	return x
}

func (p *xorshift) Uint128() uint32 {
	t := p.d
	s := p.a

	p.d = p.c
	p.c = p.b
	p.b = s

	t ^= t << 11
	t ^= t >> 8
	p.a = t ^ s ^ (s >> 19)
	return p.a
}

func (p *xorshift) Uint64s() uint64 {
	x := uint64(p.a) | uint64(p.b)<<32
	x ^= x >> 12
	x ^= x << 25
	x ^= x >> 27
	p.a = uint32(x)
	p.b = uint32(x >> 32)
	return x * 0x2545F4914F6CDD1D
}

func (p *xorshift) Uint128p() uint64 {
	t := uint64(p.a) | uint64(p.b)<<32
	s := uint64(p.c) | uint64(p.d)<<32

	p.a = uint32(s)
	p.b = uint32(s >> 32)

	t ^= t << 23
	t ^= t >> 17
	t ^= s ^ (s >> 26)

	p.c = uint32(t)
	p.d = uint32(t >> 32)

	return t + s
}

func (p *xorshift) Wow() uint32 {
	t := p.d
	s := p.a

	p.d = p.c
	p.c = p.b
	p.b = s

	t ^= t >> 2
	t ^= t << 1
	t ^= s ^ (s << 4)
	p.a = t

	p.counter += 362437
	return t + p.counter
}

func (p *xorshift) Seed(seed int64) {
	rand.Seed(seed)
	p.a = rand.Uint32()
	p.b = rand.Uint32()
	p.c = rand.Uint32()
	p.d = rand.Uint32()
}

func main() {
	var x xorshift

	x.Seed(time.Now().UnixNano())
	for i := 0; i < 1e6; i++ {
		fmt.Println(x.Wow())
	}
}
