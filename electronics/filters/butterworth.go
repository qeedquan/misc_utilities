// https://en.wikipedia.org/wiki/Butterworth_filter
// https://tttapa.github.io/Pages/Mathematics/Systems-and-Control-Theory/Analog-Filters/Butterworth-Filters.html
// https://www.ece.uic.edu/~jmorisak/blpf.html
// https://www.egr.msu.edu/classes/ece480/capstone/fall11/group02/web/Documents/How%20to%20Design%2010%20kHz%20filter-Vadim.pdf
// https://github.com/LRDPRDX/ButterworthRooFit
package main

import (
	"bufio"
	"fmt"
	"log"
	"math"
	"math/cmplx"
	"os"
)

func main() {
	dumpfpr("normalized", 16, 0, 2, 1e-3)
	dumpgain("normalized", 16)
	dumpimp("impulse_response.txt", 8)

	// determine magnitude/phase response at 800 hz 6th order filter
	// having a cutoff frequency of 400hz
	// the idea is we use the normalized bessel filter but we use
	// the frequency/cutoff_frequency ratio to determine where on the
	// normalized scale we should look
	fmt.Println(bfr(6, 800/400.0, 'l'))
	fmt.Println(bord(-24, 800, 400, 'l'))
	fmt.Println(bfr(13, 800/400.0, 'l'))
}

func dumpimp(name string, order int) {
	f1 := create(name)
	defer f1.Close()

	w1 := bufio.NewWriter(f1)
	defer w1.Flush()

	const N = 1e5
	const dt = 1e-3
	t := 0.0
	for i := 0; i < N; i++ {
		for j := 1; j <= order; j++ {
			fmt.Fprintf(w1, "%v ", bimp(j, t))
		}
		fmt.Fprintln(w1)
		t += dt
	}
}

// the butterworth filter is a design that exhibits
// these properties where the cutoff frequency is normalized
// to 1 radian per second whose gain is the following,
// these should match the frequency response plots gain calculations
// where we explicit solve for the filter coefficients, in this case
// we just plot the gain of what we should expect
func dumpgain(name string, order int) {
	f1 := create(fmt.Sprintf("%s_gain_lpf.txt", name))
	f2 := create(fmt.Sprintf("%s_gain_hpf.txt", name))
	f3 := create(fmt.Sprintf("%s_gain_bpf.txt", name))
	f4 := create(fmt.Sprintf("%s_gain_bsf.txt", name))
	defer f1.Close()
	defer f2.Close()
	defer f3.Close()
	defer f4.Close()

	w1 := bufio.NewWriter(f1)
	w2 := bufio.NewWriter(f2)
	w3 := bufio.NewWriter(f3)
	w4 := bufio.NewWriter(f4)
	defer w1.Flush()
	defer w2.Flush()
	defer w3.Flush()
	defer w4.Flush()

	for w := 0.0; w <= 10.0; w += 1e-3 {
		for i := 1.0; i <= float64(order); i++ {
			G1 := bgain(w, i, 'l')
			G2 := bgain(w, i, 'h')
			G3 := bgain(w, i, 'b')
			G4 := bgain(w, i, 's')

			fmt.Fprintf(w1, "%v ", math.Abs(G1))
			fmt.Fprintf(w2, "%v ", math.Abs(G2))
			fmt.Fprintf(w3, "%v ", math.Abs(G3))
			fmt.Fprintf(w4, "%v ", math.Abs(G4))
		}
		fmt.Fprintln(w1)
		fmt.Fprintln(w2)
		fmt.Fprintln(w3)
		fmt.Fprintln(w4)
	}
}

// calculate gain for a type of filter (normalized)
// the full equation is

// H_lp(jw) = H0 / sqrt(1 + e^2(ws/wp)^(2n))
// H_hp(jw) = w^n * H_lp(jw)
// H_bp(jw) = H_lp(jw) + H_hp(jw)
// H_bs(jw) = 1 / H_bp(jw)
func bgain(w, order float64, typ rune) float64 {
	lo := 1 / math.Sqrt(1+math.Pow(w, 2*order))
	hi := math.Pow(w, order) * lo
	bp := lo + hi
	bs := 1 / bp
	switch typ {
	case 'l':
		return lo
	case 'h':
		return hi
	case 'b':
		return bp
	case 's':
		return bs
	default:
		panic("unknown filter type")
	}
}

func dumpfpr(name string, orders int, start, end, step float64) {
	f1 := create(fmt.Sprintf("%s_lpf_magitude_response.txt", name))
	f2 := create(fmt.Sprintf("%s_hpf_magitude_response.txt", name))
	f3 := create(fmt.Sprintf("%s_bpf_magnitude_response.txt", name))
	f4 := create(fmt.Sprintf("%s_bsf_magnitude_response.txt", name))
	f5 := create(fmt.Sprintf("%s_lpf_phase_response.txt", name))
	f6 := create(fmt.Sprintf("%s_hpf_phase_response.txt", name))
	f7 := create(fmt.Sprintf("%s_bpf_phase_response.txt", name))
	f8 := create(fmt.Sprintf("%s_bsf_phase_response.txt", name))
	defer f1.Close()
	defer f2.Close()
	defer f3.Close()
	defer f4.Close()
	defer f5.Close()
	defer f6.Close()
	defer f7.Close()
	defer f8.Close()

	w1 := bufio.NewWriter(f1)
	w2 := bufio.NewWriter(f2)
	w3 := bufio.NewWriter(f3)
	w4 := bufio.NewWriter(f4)
	w5 := bufio.NewWriter(f5)
	w6 := bufio.NewWriter(f6)
	w7 := bufio.NewWriter(f7)
	w8 := bufio.NewWriter(f8)
	defer w1.Flush()
	defer w2.Flush()
	defer w3.Flush()
	defer w4.Flush()
	defer w5.Flush()
	defer w6.Flush()
	defer w7.Flush()
	defer w8.Flush()

	tp := make([][4]float64, orders+1)
	lp := make([][4]float64, orders+1)
	for i := start; i <= end; i += step {
		for j := 1; j <= orders; j++ {
			ml, pl := bfr(j, i, 'l')
			mh, ph := bfr(j, i, 'h')
			mb, pb := bfr(j, i, 'b')
			ms, ps := bfr(j, i, 's')

			tp[j][0] += sdangdist(lp[j][0], pl)
			tp[j][1] += sdangdist(lp[j][1], ph)
			tp[j][2] += sdangdist(lp[j][2], pb)
			tp[j][3] += sdangdist(lp[j][3], ps)
			lp[j][0] = pl
			lp[j][1] = ph
			lp[j][2] = pb
			lp[j][3] = ps

			fmt.Fprintf(w1, "%v ", ml)
			fmt.Fprintf(w2, "%v ", mh)
			fmt.Fprintf(w3, "%v ", mb)
			fmt.Fprintf(w4, "%v ", ms)
			fmt.Fprintf(w5, "%v ", tp[j][0])
			fmt.Fprintf(w6, "%v ", tp[j][1])
			fmt.Fprintf(w7, "%v ", tp[j][2])
			fmt.Fprintf(w8, "%v ", tp[j][3])
		}
		fmt.Fprintln(w1)
		fmt.Fprintln(w2)
		fmt.Fprintln(w3)
		fmt.Fprintln(w4)
		fmt.Fprintln(w5)
		fmt.Fprintln(w6)
		fmt.Fprintln(w7)
		fmt.Fprintln(w8)
	}
}

func create(name string) *os.File {
	f, err := os.Create(name)
	if err != nil {
		log.Fatal(err)
	}
	return f
}

// the butterworth low pass filter has the schema
// 1 / ((s - p0)(s - p1)(s - p2)) ...
// where the poles are on the complex unit circle
// evenly spaced apart, we can use transformations
// to get high pass, bandpass and bandstop filter
// the poles are stable if it is on the left side of the plane
// (if the arg is between 90 and 270 degree)

// this calculates the normalized frequency (cutoff represents 1)
func bfr(order int, freq float64, typ rune) (magnitude, phase float64) {
	lo := complex(1, 0)
	hi := complex(math.Pow(freq, float64(order)), 0)
	den := complex(1, 0)
	s := complex(0, freq)
	for k := 1; k <= order; k++ {
		x := math.Pi * float64(2*k+order-1) / float64(2*order)
		pole := cmplx.Exp(complex(0, x))
		den *= (s - pole)
	}

	var transfer complex128
	switch typ {
	case 'l':
		transfer = lo / den
	case 'h':
		transfer = hi / den
	case 'b':
		transfer = (lo + hi) / den
	case 's':
		transfer = den / (lo + hi)
	default:
		panic("unknown filter type")
	}
	magnitude = 20 * math.Log10(cmplx.Abs(transfer))
	phase = rad2deg(math.Mod(cmplx.Phase(transfer), 2*math.Pi))
	return
}

// usually the order of the filter is not given, and we want to calculate it
// given a magnitude, frequency, and cutoff frequency (3db frequency),
// we want to determine the order
// the order will give us a filter in which where the magnitude response starts
// falling below 'mag' at 'w1' frequency
// the magnitude is given in negative decibels
func bord(mag, w1, wc float64, typ rune) float64 {
	switch typ {
	case 'l':
		return math.Ceil(math.Log10(math.Pow(10, -mag/10)-1) / (2 * math.Log10(w1/wc)))
	default:
		panic("unknown filter type")
	}
}

func bimp(order int, t float64) float64 {
	wc := complex(1, 0)

	// Returns k-th pole s_k of Butterworth transfer in S-domain
	Sk := func(k int) complex128 {
		x := math.Pi * float64(2*k+order-1) / float64(2*order)
		return complex(math.Cos(x), math.Sin(x))
	}

	// return  (s - s_k) * H(s)
	// H(s) - BW transfer function
	// s_k  - k-th pole of H(s)
	J := func(k int) complex128 {
		res := complex(1, 0)
		for n := 1; n <= order; n++ {
			if n == k {
				continue
			}

			res /= (Sk(k) - Sk(n))
		}
		return res
	}

	H := func(x float64) complex128 {
		res := complex(0, 0)
		if x >= 0 {
			//         n
			// h(t) = SUM[ (s - s_k) * H(s_k) * exp(s_k * t) ]
			//        k=1
			for k := 1; k <= order; k++ {
				res += J(k) * cmplx.Exp(wc*Sk(k)*complex(x, 0))
			}
		}
		return res
	}

	return real(H(t))
}

func rad2deg(x float64) float64 {
	return x * 180 / math.Pi
}

func sdangdist(a, b float64) float64 {
	d := math.Mod(b-a+math.Pi, 2*math.Pi) - math.Pi
	if d < -math.Pi {
		return 2*math.Pi + d
	}
	return d
}
