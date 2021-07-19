package main

func main() {
}

// https://www.allaboutcircuits.com/technical-articles/understanding-transfer-functions-for-low-pass-filters/
// 1st order low pass filter has the transfer function of the form
// H(s) = K / (1 + s/w0)
// If we want to know the magnitude/phase information at a certain
// frequency, replace s with jw at that given angular frequency

// A RC low pass filter in the s-domain has
// Resistance = R
// Impedance = 1/sC
// Vout/Vin = 1/sC / (1/sC + R) = 1/(1 + sRC)
// compare to H(s), we can see K=1 and w0 = 1/RC
// using standard form of H(s), we can say K is the gain at DC and
// w0 is the cutoff frequency, so the gain here is 1 (unity gain) and
// cutoff frequency is at 1/RC, we might want to normalize it at 2pi so
// it wil be 1/(2*pi*R*C)

// There is another form we can use to represent gain/cutoff frequency
// H(s) = a0 / (s + w0)
// H(s = 0) represents the DC gain of our filter which is a0/w0
// since it represents Vout/Vin

// To see why w0 is the cutoff frequency, use following derivation
// H(s) = K / (1 + s/w0)
// evaluate at the cutoff frequency (at w0)
// H(jw = jw0) = K / (1 + jw0/w0) = K / (1 + j)
// now the denominator is the complex number (1 + j) (j is the imaginary unit)
// the magnitude is then K/(sqrt(1^2 + 1^2)) = K/sqrt(2)

// For phase shift theta(w) = tan^-1(y/x)
// evaluate at the cutoff frequency
// and get -tan^-1(w0/w0) = -tan^-1(1) = -45 degrees
// maximum phase shift of a first order low pass filter is 90 degree, so
// this tells us the cutoff frequency is the center of the circuit phase response, where
// the filter generate half of its maximum phase shift

// https://www.allaboutcircuits.com/technical-articles/understanding-poles-and-zeros-in-transfer-functions/
// To understand more about poles/zeroes use the transfer function
// H(s) = K / (1 + s/w0), if we multiply top and bottom by s, we get
// H(s) = Ks / (s + w0), hence we have 1 zero and 1 pole
// zeroes = s=0
// pole = s=-w0
// A zero correspond to a corner frequency that increases 20db/decade while
// a pole correspond to a corner frequency that decreases 20db/decade
// In some representation such as
// H(s) = a0 / (s + w0), there does not seem to be a zero since there is s in the numerator,
// but we need to take the limit where lim s->z H(s) if we want to find asymptotic behavior of a filter
// which case will give as H(s) goes to infinity, so the zero will be at w0=infinity

// to cascade filters together, multiply the transfer function (s domain) together
// filter1(t) -> filter2(t) is H1(s) * H2(s) (this is like the fourier transform where convolution becomes multiplication)
