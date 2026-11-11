/*

https://frederic-wang.fr/2013/12/01/decomposition-of-2d-transform-matrices/
The derivation in the link above assumes counter-clockwise rotation
For the skew matrices in the link it uses atan() for the skew matrices to compute the values,
but that assumes the skew (shear) matrices accept angles, for the code implemented, it just takes raw values
so we don't need an atan()

The two main ways of decomposing a affine 2D matrix is LDU/QR decomposition
The affine 2D matrix is represented as:
[a c e]
[b d f]
[0 0 1]

*/

package main

import (
	"fmt"
	"math"
	"math/rand"
)

type Mat3 [3][3]float64

func main() {
	A := affine2(100)
	LDU(A)
	QR(A)
}

/*

Δ = det(A)
The transform matrix(a,b,c,d,e,f) can be written as:

a != 0
translate(e, f) * skewY(b/a) * scale(a, Δ/a) * skewX(c/a)

a = 0 and b != 0
translate(e,f) * rotate(90°) * scale(b, Δ/b) * skewX(d/b)

a = 0 and b = 0
translate(e,f) * scale(c, d) * skewX(45°) * scale(0, 1)

*/

func LDU(A Mat3) {
	var B Mat3
	a, b, c, d, e, f := coeff(A)
	det := det2(A)
	if det == 0 {
		goto out
	}
	if a != 0 {
		KX := skewX(c / a)
		S := scale(a, det/a)
		KY := skewY(b / a)
		T := translate(e, f)
		B = mul(T, mul(KY, mul(S, KX)))
	} else if a == 0 && b != 0 {
		T := translate(e, f)
		R := rotate(math.Pi / 2)
		S := scale(b, det/b)
		KX := skewX(d / b)
		B = mul(T, mul(R, mul(S, KX)))
	} else if a == 0 && b == 0 {
		T := translate(e, f)
		S1 := scale(c, d)
		KX := skewX(math.Pi / 4)
		S2 := scale(0, 1)
		B = mul(T, mul(S1, mul(KX, S2)))
	}

out:
	fmt.Println("LDU")
	fmt.Println(A)
	fmt.Println(B)
}

/*

Δ = det(A)
r = sqrt(a^2 + b^2)
s = sqrt(c^2 + d^2)

The transform matrix(a,b,c,d,e,f) can be written as:

r != 0
translate(e,f) * rotate(sign(b) * acos(a/r)) * scale(r, Δ/r) * skewX((a*c + b*d)/r^2)

s != 0
translate(e,f) * rotate(90° - sign(d) * acos(-c/s)) * scale(Delta/s, s) * skewY(((a*c + b*d)/s^2))

r = s = 0
scale(0,0)

*/

func QR(A Mat3) {
	var B Mat3
	a, b, c, d, e, f := coeff(A)
	r := math.Hypot(a, b)
	s := math.Hypot(c, d)
	det := det2(A)
	if det == 0 {
		goto out
	}
	if r != 0 {
		T := translate(e, f)
		R := rotate(sign(b) * math.Acos(a/r))
		S := scale(r, det/r)
		KX := skewX(((a*c + b*d) / (r * r)))
		B = mul(T, mul(R, mul(S, KX)))
	} else if s != 0 {
		T := translate(e, f)
		R := rotate(math.Pi/2 - sign(d)*math.Acos(-c/s))
		S := scale(det/s, s)
		KY := skewY(((a*c + b*d) / (s * s)))
		B = mul(T, mul(R, mul(S, KY)))
	}

out:
	fmt.Println("QR")
	fmt.Println(A)
	fmt.Println(B)
}

func affine2(s float64) Mat3 {
	A := Mat3{}
	for i := range A {
		for j := range A[i] {
			A[i][j] = rand.Float64() * s
		}
	}
	A[2][0] = 0
	A[2][1] = 0
	A[2][2] = 1
	return A
}

func det2(A Mat3) float64 {
	return A[0][0]*A[1][1] - A[1][0]*A[0][1]
}

func coeff(A Mat3) (a, b, c, d, e, f float64) {
	a = A[0][0]
	b = A[1][0]
	c = A[0][1]
	d = A[1][1]
	e = A[0][2]
	f = A[1][2]
	return
}

func skewX(kx float64) Mat3 {
	return Mat3{
		{1, kx, 0},
		{0, 1, 0},
		{0, 0, 1},
	}
}

func skewY(ky float64) Mat3 {
	return Mat3{
		{1, 0, 0},
		{ky, 1, 0},
		{0, 0, 1},
	}
}

func scale(sx, sy float64) Mat3 {
	return Mat3{
		{sx, 0, 0},
		{0, sy, 0},
		{0, 0, 1},
	}
}

func translate(tx, ty float64) Mat3 {
	return Mat3{
		{1, 0, tx},
		{0, 1, ty},
		{0, 0, 1},
	}
}

func rotate(theta float64) Mat3 {
	s, c := math.Sincos(theta)
	return Mat3{
		{c, -s, 0},
		{s, c, 0},
		{0, 0, 1},
	}
}

func mul(X, Y Mat3) Mat3 {
	R := Mat3{}
	for i := range X {
		for j := range X {
			for k := range X {
				R[i][j] += X[i][k] * Y[k][j]
			}
		}
	}
	return R
}

func (m Mat3) String() string {
	s := ""
	for i := range m {
		for j := range m {
			s += fmt.Sprintf("%v ", m[i][j])
		}
		s += fmt.Sprint("\n")
	}
	return s
}

func sign(x float64) float64 {
	if x < 0 {
		return -1
	}
	if x == 0 {
		return 0
	}
	return 1
}
