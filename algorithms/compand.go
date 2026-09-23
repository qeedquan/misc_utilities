// https://en.wikipedia.org/wiki/Companding
// https://en.wikipedia.org/wiki/A-law_algorithm
// https://en.wikipedia.org/wiki/%CE%9C-law_algorithm
// http://digitalsoundandmusic.com/5-3-8-algorithms-for-audio-companding-and-compression/

package main

import (
	"fmt"
	"math"
)

func main() {
	for i := -32768; i <= 32767; i++ {
		fmt.Println(i, uu8t16(uq16t8(int16(i))), au8t16(aq16t8(int16(i))))
	}
}

func sgn(x float64) float64 {
	if x < 0 {
		return -1
	}
	return 1
}

func alaw(x, A float64) float64 {
	s := sgn(x)
	x = math.Abs(x)
	if x < 1/A {
		return s * A * x / (1 + math.Log(A))
	}
	return s * (1 + math.Log(A*x)) / (1 + math.Log(A))
}

func alawinv(y, A float64) float64 {
	s := sgn(y)
	y = math.Abs(y)
	if y < 1/(1+math.Log(A)) {
		return s * y * (1 + math.Log(A)) / A
	}
	return s * math.Exp(y*(1+math.Log(A))-1) / A
}

func ulaw(x, u float64) float64 {
	s := sgn(x)
	x = math.Abs(x)
	return s * math.Log(1+u*x) / math.Log(1+u)
}

func ulawinv(y, u float64) float64 {
	s := sgn(y)
	y = math.Abs(y)
	return s * (1 / u) * (math.Pow(1+u, y) - 1)
}

func ulaw8(x float64) float64 {
	return ulaw(x, 255)
}

func ulawinv8(x float64) float64 {
	return ulawinv(x, 255)
}

// to quantize using the companding algorithm
// remap the input range to [-1, 1], apply the function
// then rescale it to 0.5*2^(bit_depth), inverse is same
func uq16t8(x int16) int8 {
	return int8(math.Floor(128 * ulaw8(float64(x)/32768.0)))
}

func uu8t16(x int8) int16 {
	return int16(math.Ceil(ulawinv8(float64(x)/128) * 32768))
}

func aq16t8(x int16) int8 {
	return int8(math.Floor(128 * alaw(float64(x)/32768.0, 87.6)))
}

func au8t16(x int8) int16 {
	return int16(math.Ceil(alawinv(float64(x)/128, 87.6) * 32768))
}
