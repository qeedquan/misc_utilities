package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	"image/draw"
	"image/png"
	"log"
	"math"
	"os"
)

var (
	scale = flag.Int("s", 2, "scale by a factor of N")
	alg   = flag.String("a", "superxbr", "use algorithm")
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("scalex: ")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 2 {
		usage()
	}

	r, err := os.Open(flag.Arg(0))
	ck(err)
	defer r.Close()

	m, _, err := image.Decode(r)
	ck(err)

	var s image.Image
	d := vals(*scale)
	for i := range d {
		switch d[i] {
		case 1:
			s = scale1x(m)
		case 2:
			if *alg == "superxbr" {
				s = superxbr2(m)
			} else {
				s = scale2x(m)
			}
		case 3:
			if *alg == "superxbr" {
				s = nil
				break
			}
			s = scale3x(m)
		}
		m = s
	}
	if s == nil {
		log.Fatalf("scale by %d using algorithm %s not supported", *scale, *alg)
	}

	w, err := os.Create(flag.Arg(1))
	ck(err)

	err = png.Encode(w, s)
	ck(err)
	ck(w.Close())
}

func vals(n int) []int {
	var p []int
	for {
		switch {
		case n%2 == 0:
			n /= 2
			p = append(p, 2)
		case n%3 == 0:
			n /= 3
			p = append(p, 3)
		case n == 1:
			p = append(p, 1)
		default:
			return nil
		}
		if n == 1 {
			break
		}
	}
	return p
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: scalex [options] input output")
	flag.PrintDefaults()
	fmt.Fprintln(os.Stderr, "\navailable algorithms: superxbr scalex")
	fmt.Fprintln(os.Stderr, "superxbr - supports scaling by 2^N")
	fmt.Fprintln(os.Stderr, "scalex   - supports scaling by 2^N * 3^M")
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func scale1x(img image.Image) image.Image {
	r := img.Bounds()
	p := image.NewRGBA(r)
	draw.Draw(p, r, img, image.ZP, draw.Src)
	return p
}

// scale2x, scale3x, scale4x family
func scale2x(img image.Image) image.Image {
	b := img.Bounds()
	p := image.NewRGBA(image.Rect(0, 0, b.Dx()*2, b.Dy()*2))

	for y := b.Min.Y; y < b.Max.Y; y++ {
		for x := b.Min.X; x < b.Max.X; x++ {
			E0, E1, E2, E3 := interp2x(img, x, y)
			px, py := x*2, y*2
			p.Set(px, py, E0)
			p.Set(px+1, py, E1)
			p.Set(px, py+1, E2)
			p.Set(px+1, py+1, E3)
		}
	}
	return p
}

func scale3x(img image.Image) image.Image {
	b := img.Bounds()
	p := image.NewRGBA(image.Rect(0, 0, b.Dx()*3, b.Dy()*3))

	for y := b.Min.Y; y < b.Max.Y; y++ {
		for x := b.Min.X; x < b.Max.X; x++ {
			E0, E1, E2, E3, E4, E5, E6, E7, E8 := interp3x(img, x, y)
			px, py := x*3, y*3
			p.Set(px, py, E0)
			p.Set(px+1, py, E1)
			p.Set(px+2, py, E2)
			p.Set(px, py+1, E3)
			p.Set(px+1, py+1, E4)
			p.Set(px+2, py+1, E5)
			p.Set(px, py+2, E6)
			p.Set(px+1, py+2, E7)
			p.Set(px+2, py+2, E8)

		}
	}
	return p
}

func scale4x(img image.Image) image.Image {
	s2x := scale2x(img)
	s4x := scale2x(s2x)
	return s4x
}

func at(img image.Image, x, y, dx, dy int) color.Color {
	b := img.Bounds()

	nx, ny := x+dx, y+dy
	if nx < b.Min.X || nx >= b.Max.X || ny < b.Min.Y || ny >= b.Max.Y {
		return img.At(x, y)
	}

	return img.At(nx, ny)
}

func interp2x(img image.Image, x, y int) (E0, E1, E2, E3 color.Color) {
	B := at(img, x, y, 0, -1)
	D := at(img, x, y, -1, 0)
	E := at(img, x, y, 0, 0)
	F := at(img, x, y, 1, 0)
	H := at(img, x, y, 0, 1)

	E0, E1, E2, E3 = E, E, E, E

	if cmp(D, B) && !cmp(B, F) && !cmp(D, H) {
		E0 = D
	}

	if cmp(B, F) && !cmp(B, D) && !cmp(F, H) {
		E1 = F
	}

	if cmp(D, H) && !cmp(D, B) && !cmp(H, F) {
		E2 = D
	}

	if cmp(H, F) && !cmp(D, H) && !cmp(B, F) {
		E3 = F
	}

	return
}

func interp3x(img image.Image, x, y int) (E0, E1, E2, E3, E4, E5, E6, E7, E8 color.Color) {
	A := at(img, x, y, -1, -1)
	B := at(img, x, y, 0, -1)
	C := at(img, x, y, 1, -1)
	D := at(img, x, y, -1, 0)
	E := at(img, x, y, 0, 0)
	F := at(img, x, y, 1, 0)
	G := at(img, x, y, -1, 1)
	H := at(img, x, y, 0, 1)
	I := at(img, x, y, 1, 1)

	E0, E1, E2, E3, E4, E5, E6, E7, E8 = E, E, E, E, E, E, E, E, E

	if cmp(D, B) && !cmp(B, F) && !cmp(D, H) {
		E0 = D
	}

	if (cmp(D, B) && !cmp(B, F) && !cmp(D, H) && !cmp(E, C)) ||
		(cmp(B, F) && !cmp(B, D) && !cmp(F, H) && !cmp(E, A)) {
		E1 = B
	}

	if cmp(B, F) && !cmp(B, D) && !cmp(F, H) {
		E2 = F
	}

	if (cmp(D, B) && !cmp(B, F) && !cmp(D, H) && !cmp(E, G)) ||
		(cmp(D, H) || !cmp(D, B) && !cmp(H, F) && !cmp(E, A)) {
		E3 = D
	}

	if (cmp(B, F) && !cmp(B, D) && !cmp(F, H) && !cmp(E, I)) ||
		(cmp(H, F) || !cmp(D, H) && !cmp(B, F) && !cmp(E, C)) {
		E5 = F
	}

	if cmp(D, H) && !cmp(D, B) && !cmp(H, F) {
		E6 = D
	}

	if (cmp(D, H) && !cmp(D, B) && !cmp(H, F) && !cmp(E, I)) ||
		(cmp(H, F) || !cmp(D, H) && !cmp(B, F) && !cmp(E, G)) {
		E7 = H
	}

	if cmp(H, F) && !cmp(D, H) && !cmp(B, F) {
		E8 = F
	}

	return
}

func cmp(a, b color.Color) bool {
	r1, g1, b1, a1 := a.RGBA()
	r2, g2, b2, a2 := b.RGBA()
	if r1 != r2 || g1 != g2 || b1 != b2 || a1 != a2 {
		return false
	}

	return true
}

// ported from hylian super xbr scaler code

/*
                         P1
|P0|B |C |P1|         C     F4          |a0|b1|c2|d3|
|D |E |F |F4|      B     F     I4       |b0|c1|d2|e3|   |e1|i1|i2|e2|
|G |H |I |I4|   P0    E  A  I     P3    |c0|d1|e2|f3|   |e3|i3|i4|e4|
|P2|H5|I5|P3|      D     H     I5       |d0|e1|f2|g3|
                      G     H5
                         P2

sx, sy
-1  -1 | -2  0   (x+y) (x-y)    -3  1  (x+y-1)  (x-y+1)
-1   0 | -1 -1                  -2  0
-1   1 |  0 -2                  -1 -1
-1   2 |  1 -3                   0 -2

 0  -1 | -1  1   (x+y) (x-y)      ...     ...     ...
 0   0 |  0  0
 0   1 |  1 -1
 0   2 |  2 -2

 1  -1 |  0  2   ...
 1   0 |  1  1
 1   1 |  2  0
 1   2 |  3 -1

 2  -1 |  1  3   ...
 2   0 |  2  2
 2   1 |  3  1
 2   2 |  4  0

*/
func superxbr2(img image.Image) image.Image {
	const (
		f    = 2
		wgt1 = 0.129633
		wgt2 = 0.175068
		w1   = -wgt1
		w2   = wgt1 + 0.5
		w3   = -wgt2
		w4   = wgt2 + 0.5
	)

	bounds := img.Bounds()
	w := bounds.Dx()
	h := bounds.Dy()
	outw := w * f
	outh := h * f
	out := image.NewRGBA(image.Rect(0, 0, outw, outh))

	wp := [6]float64{2, 1, -1, 4, -1, 1}

	// first pass
	for y := 0; y < outh; y++ {
		for x := 0; x < outw; x++ {
			var r, g, b, a, Y [4][4]float64

			// central pixels on original image
			cx := x / f
			cy := y / f

			// sample supporting pixels in original image
			for sx := -1; sx <= 2; sx++ {
				for sy := -1; sy <= 2; sy++ {
					csy := clamp(sy+cy, 0, h-1)
					csx := clamp(sx+cx, 0, w-1)
					col := rgbaat(img, bounds.Min.X+csx, bounds.Min.Y+csy)
					r[sx+1][sy+1] = float64(col.R)
					g[sx+1][sy+1] = float64(col.G)
					b[sx+1][sy+1] = float64(col.B)
					a[sx+1][sy+1] = float64(col.A)
					Y[sx+1][sy+1] = (0.2126*r[sx+1][sy+1] + 0.7152*g[sx+1][sy+1] + 0.0722*b[sx+1][sy+1])
				}
			}

			min_r_sample := min4(r[1][1], r[2][1], r[1][2], r[2][2])
			min_g_sample := min4(g[1][1], g[2][1], g[1][2], g[2][2])
			min_b_sample := min4(b[1][1], b[2][1], b[1][2], b[2][2])
			min_a_sample := min4(a[1][1], a[2][1], a[1][2], a[2][2])
			max_r_sample := max4(r[1][1], r[2][1], r[1][2], r[2][2])
			max_g_sample := max4(g[1][1], g[2][1], g[1][2], g[2][2])
			max_b_sample := max4(b[1][1], b[2][1], b[1][2], b[2][2])
			max_a_sample := max4(a[1][1], a[2][1], a[1][2], a[2][2])
			d_edge := diagonal_edge(Y[:][:], wp[:])

			r1 := w1*(r[0][3]+r[3][0]) + w2*(r[1][2]+r[2][1])
			g1 := w1*(g[0][3]+g[3][0]) + w2*(g[1][2]+g[2][1])
			b1 := w1*(b[0][3]+b[3][0]) + w2*(b[1][2]+b[2][1])
			a1 := w1*(a[0][3]+a[3][0]) + w2*(a[1][2]+a[2][1])
			r2 := w1*(r[0][0]+r[3][3]) + w2*(r[1][1]+r[2][2])
			g2 := w1*(g[0][0]+g[3][3]) + w2*(g[1][1]+g[2][2])
			b2 := w1*(b[0][0]+b[3][3]) + w2*(b[1][1]+b[2][2])
			a2 := w1*(a[0][0]+a[3][3]) + w2*(a[1][1]+a[2][2])

			var rf, gf, bf, af float64
			// generate and write result
			if d_edge <= 0 {
				rf = r1
				gf = g1
				bf = b1
				af = a1
			} else {
				rf = r2
				gf = g2
				bf = b2
				af = a2
			}

			// anti-ringing, clamp
			rf = clampf(rf, min_r_sample, max_r_sample)
			gf = clampf(gf, min_g_sample, max_g_sample)
			bf = clampf(bf, min_b_sample, max_b_sample)
			af = clampf(af, min_a_sample, max_a_sample)

			ri := clamp(int(math.Ceil(rf)), 0, 255)
			gi := clamp(int(math.Ceil(gf)), 0, 255)
			bi := clamp(int(math.Ceil(bf)), 0, 255)
			ai := clamp(int(math.Ceil(af)), 0, 255)

			col := img.At(bounds.Min.X+cx, bounds.Min.Y+cy)
			out.Set(x, y, col)
			out.Set(x+1, y, col)
			out.Set(x, y+1, col)
			out.Set(x+1, y+1, color.RGBA{uint8(ri), uint8(gi), uint8(bi), uint8(ai)})
			x++
		}
		y++
	}

	// second pass
	wp[0] = 2.0
	wp[1] = 0.0
	wp[2] = 0.0
	wp[3] = 0.0
	wp[4] = 0.0
	wp[5] = 0.0

	for y := 0; y < outh; y++ {
		for x := 0; x < outw; x++ {
			var r, g, b, a, Y [4][4]float64

			// sample supporting pixels in original image
			for sx := -1; sx <= 2; sx++ {
				for sy := -1; sy <= 2; sy++ {
					// clamp pixel locations
					csy := clamp(sx-sy+y, 0, f*h-1)
					csx := clamp(sx+sy+x, 0, f*w-1)
					col := rgbaat(out, csx, csy)
					r[sx+1][sy+1] = float64(col.R)
					g[sx+1][sy+1] = float64(col.G)
					b[sx+1][sy+1] = float64(col.B)
					a[sx+1][sy+1] = float64(col.A)
					Y[sx+1][sy+1] = (0.2126*r[sx+1][sy+1] + 0.7152*g[sx+1][sy+1] + 0.0722*b[sx+1][sy+1])
				}
			}

			min_r_sample := min4(r[1][1], r[2][1], r[1][2], r[2][2])
			min_g_sample := min4(g[1][1], g[2][1], g[1][2], g[2][2])
			min_b_sample := min4(b[1][1], b[2][1], b[1][2], b[2][2])
			min_a_sample := min4(a[1][1], a[2][1], a[1][2], a[2][2])
			max_r_sample := max4(r[1][1], r[2][1], r[1][2], r[2][2])
			max_g_sample := max4(g[1][1], g[2][1], g[1][2], g[2][2])
			max_b_sample := max4(b[1][1], b[2][1], b[1][2], b[2][2])
			max_a_sample := max4(a[1][1], a[2][1], a[1][2], a[2][2])

			d_edge := diagonal_edge(Y[:][:], wp[:])
			r1 := w3*(r[0][3]+r[3][0]) + w4*(r[1][2]+r[2][1])
			g1 := w3*(g[0][3]+g[3][0]) + w4*(g[1][2]+g[2][1])
			b1 := w3*(b[0][3]+b[3][0]) + w4*(b[1][2]+b[2][1])
			a1 := w3*(a[0][3]+a[3][0]) + w4*(a[1][2]+a[2][1])
			r2 := w3*(r[0][0]+r[3][3]) + w4*(r[1][1]+r[2][2])
			g2 := w3*(g[0][0]+g[3][3]) + w4*(g[1][1]+g[2][2])
			b2 := w3*(b[0][0]+b[3][3]) + w4*(b[1][1]+b[2][2])
			a2 := w3*(a[0][0]+a[3][3]) + w4*(a[1][1]+a[2][2])

			// generate and write result
			var rf, gf, bf, af float64
			if d_edge <= 0.0 {
				rf = r1
				gf = g1
				bf = b1
				af = a1
			} else {
				rf = r2
				gf = g2
				bf = b2
				af = a2
			}

			// anti-ringing, clamp.
			rf = clampf(rf, min_r_sample, max_r_sample)
			gf = clampf(gf, min_g_sample, max_g_sample)
			bf = clampf(bf, min_b_sample, max_b_sample)
			af = clampf(af, min_a_sample, max_a_sample)

			ri := clamp(int(math.Ceil(rf)), 0, 255)
			gi := clamp(int(math.Ceil(gf)), 0, 255)
			bi := clamp(int(math.Ceil(bf)), 0, 255)
			ai := clamp(int(math.Ceil(af)), 0, 255)
			out.Set(x+1, y, color.RGBA{uint8(ri), uint8(gi), uint8(bi), uint8(ai)})

			for sx := -1; sx <= 2; sx++ {
				for sy := -1; sy <= 2; sy++ {
					// clamp pixel locations
					csy := clamp(sx-sy+1+y, 0, f*h-1)
					csx := clamp(sx+sy-1+x, 0, f*w-1)
					col := rgbaat(out, csx, csy)
					r[sx+1][sy+1] = float64(col.R)
					g[sx+1][sy+1] = float64(col.G)
					b[sx+1][sy+1] = float64(col.B)
					a[sx+1][sy+1] = float64(col.A)
					Y[sx+1][sy+1] = (0.2126*r[sx+1][sy+1] + 0.7152*g[sx+1][sy+1] + 0.0722*b[sx+1][sy+1])
				}
			}
			d_edge = diagonal_edge(Y[:][:], wp[:])
			r1 = w3*(r[0][3]+r[3][0]) + w4*(r[1][2]+r[2][1])
			g1 = w3*(g[0][3]+g[3][0]) + w4*(g[1][2]+g[2][1])
			b1 = w3*(b[0][3]+b[3][0]) + w4*(b[1][2]+b[2][1])
			a1 = w3*(a[0][3]+a[3][0]) + w4*(a[1][2]+a[2][1])
			r2 = w3*(r[0][0]+r[3][3]) + w4*(r[1][1]+r[2][2])
			g2 = w3*(g[0][0]+g[3][3]) + w4*(g[1][1]+g[2][2])
			b2 = w3*(b[0][0]+b[3][3]) + w4*(b[1][1]+b[2][2])
			a2 = w3*(a[0][0]+a[3][3]) + w4*(a[1][1]+a[2][2])
			// generate and write result
			if d_edge <= 0.0 {
				rf = r1
				gf = g1
				bf = b1
				af = a1
			} else {
				rf = r2
				gf = g2
				bf = b2
				af = a2
			}
			// anti-ringing, clamp.
			rf = clampf(rf, min_r_sample, max_r_sample)
			gf = clampf(gf, min_g_sample, max_g_sample)
			bf = clampf(bf, min_b_sample, max_b_sample)
			af = clampf(af, min_a_sample, max_a_sample)

			ri = clamp(int(math.Ceil(rf)), 0, 255)
			gi = clamp(int(math.Ceil(gf)), 0, 255)
			bi = clamp(int(math.Ceil(bf)), 0, 255)
			ai = clamp(int(math.Ceil(af)), 0, 255)
			out.Set(x, y+1, color.RGBA{uint8(ri), uint8(gi), uint8(bi), uint8(ai)})

			x++
		}
		y++
	}

	// third pass
	wp[0] = 2.0
	wp[1] = 1.0
	wp[2] = -1.0
	wp[3] = 4.0
	wp[4] = -1.0
	wp[5] = 1.0

	for y := outh - 1; y >= 0; y-- {
		for x := outw - 1; x >= 0; x-- {
			var r, g, b, a, Y [4][4]float64

			for sx := -2; sx <= 1; sx++ {
				for sy := -2; sy <= 1; sy++ {
					// clamp pixel locations
					csy := clamp(sy+y, 0, f*h-1)
					csx := clamp(sx+x, 0, f*w-1)
					col := rgbaat(out, csx, csy)
					r[sx+2][sy+2] = float64(col.R)
					g[sx+2][sy+2] = float64(col.G)
					b[sx+2][sy+2] = float64(col.B)
					a[sx+2][sy+2] = float64(col.A)
					Y[sx+2][sy+2] = (0.2126*r[sx+2][sy+2] + 0.7152*g[sx+2][sy+2] + 0.0722*b[sx+2][sy+2])
				}
			}

			min_r_sample := min4(r[1][1], r[2][1], r[1][2], r[2][2])
			min_g_sample := min4(g[1][1], g[2][1], g[1][2], g[2][2])
			min_b_sample := min4(b[1][1], b[2][1], b[1][2], b[2][2])
			min_a_sample := min4(a[1][1], a[2][1], a[1][2], a[2][2])
			max_r_sample := max4(r[1][1], r[2][1], r[1][2], r[2][2])
			max_g_sample := max4(g[1][1], g[2][1], g[1][2], g[2][2])
			max_b_sample := max4(b[1][1], b[2][1], b[1][2], b[2][2])
			max_a_sample := max4(a[1][1], a[2][1], a[1][2], a[2][2])
			d_edge := diagonal_edge(Y[:][:], wp[:])
			r1 := w1*(r[0][3]+r[3][0]) + w2*(r[1][2]+r[2][1])
			g1 := w1*(g[0][3]+g[3][0]) + w2*(g[1][2]+g[2][1])
			b1 := w1*(b[0][3]+b[3][0]) + w2*(b[1][2]+b[2][1])
			a1 := w1*(a[0][3]+a[3][0]) + w2*(a[1][2]+a[2][1])
			r2 := w1*(r[0][0]+r[3][3]) + w2*(r[1][1]+r[2][2])
			g2 := w1*(g[0][0]+g[3][3]) + w2*(g[1][1]+g[2][2])
			b2 := w1*(b[0][0]+b[3][3]) + w2*(b[1][1]+b[2][2])
			a2 := w1*(a[0][0]+a[3][3]) + w2*(a[1][1]+a[2][2])

			// generate and write result
			var rf, gf, bf, af float64
			if d_edge <= 0.0 {
				rf = r1
				gf = g1
				bf = b1
				af = a1
			} else {
				rf = r2
				gf = g2
				bf = b2
				af = a2
			}
			// anti-ringing, clamp.
			rf = clampf(rf, min_r_sample, max_r_sample)
			gf = clampf(gf, min_g_sample, max_g_sample)
			bf = clampf(bf, min_b_sample, max_b_sample)
			af = clampf(af, min_a_sample, max_a_sample)

			ri := clamp(int(math.Ceil(rf)), 0, 255)
			gi := clamp(int(math.Ceil(gf)), 0, 255)
			bi := clamp(int(math.Ceil(bf)), 0, 255)
			ai := clamp(int(math.Ceil(af)), 0, 255)

			out.Set(x, y, color.RGBA{uint8(ri), uint8(gi), uint8(bi), uint8(ai)})
		}
	}

	return out
}

func clampf(x, a, b float64) float64 {
	if x < a {
		x = a
	}
	if x > b {
		x = b
	}
	return x
}

func clamp(x, a, b int) int {
	if x < a {
		x = a
	}
	if x > b {
		x = b
	}
	return x
}

func min4(a, b, c, d float64) float64 {
	return math.Min(math.Min(a, b), math.Min(c, d))
}

func max4(a, b, c, d float64) float64 {
	return math.Max(math.Max(a, b), math.Max(c, d))
}

func diagonal_edge(mat [][4]float64, wp []float64) float64 {
	dw1 := wp[0]*(df(mat[0][2], mat[1][1])+df(mat[1][1], mat[2][0])+df(mat[1][3], mat[2][2])+df(mat[2][2], mat[3][1])) +
		wp[1]*(df(mat[0][3], mat[1][2])+df(mat[2][1], mat[3][0])) +
		wp[2]*(df(mat[0][3], mat[2][1])+df(mat[1][2], mat[3][0])) +
		wp[3]*df(mat[1][2], mat[2][1]) +
		wp[4]*(df(mat[0][2], mat[2][0])+df(mat[1][3], mat[3][1])) +
		wp[5]*(df(mat[0][1], mat[1][0])+df(mat[2][3], mat[3][2]))

	dw2 := wp[0]*(df(mat[0][1], mat[1][2])+df(mat[1][2], mat[2][3])+df(mat[1][0], mat[2][1])+df(mat[2][1], mat[3][2])) +
		wp[1]*(df(mat[0][0], mat[1][1])+df(mat[2][2], mat[3][3])) +
		wp[2]*(df(mat[0][0], mat[2][2])+df(mat[1][1], mat[3][3])) +
		wp[3]*df(mat[1][1], mat[2][2]) +
		wp[4]*(df(mat[1][0], mat[3][2])+df(mat[0][1], mat[2][3])) +
		wp[5]*(df(mat[0][2], mat[1][3])+df(mat[2][0], mat[3][1]))

	return dw1 - dw2
}

func df(A, B float64) float64 {
	return math.Abs(A - B)
}

func rgbaat(img image.Image, x, y int) color.RGBA {
	p := img.At(x, y)
	c := color.RGBAModel.Convert(p)
	return c.(color.RGBA)
}
