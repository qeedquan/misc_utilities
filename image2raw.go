package main

import (
	"flag"
	"fmt"
	"image"
	"image/color"
	_ "image/gif"
	_ "image/jpeg"
	_ "image/png"
	"io"
	"os"
)

var (
	format  = flag.String("f", "rgba", "output format")
	reverse = flag.Bool("r", false, "reverse color for bit format")

	status = 0
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		flag.Usage()
	}

	for _, name := range flag.Args() {
		ek(convert(name))
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [-f format] [-r] <img> ...")
	fmt.Fprintln(os.Stderr, "supported formats: bit gray alpha rgb rgba bgr bgra cymk ycbcr")
	flag.PrintDefaults()
	os.Exit(2)
}

func ek(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "image2raw:", err)
		status = 1
	}
}

func convert(name string) error {
	f, err := os.Open(name)
	if err != nil {
		return err
	}
	defer f.Close()

	m, _, err := image.Decode(f)
	if err != nil {
		return err
	}

	switch *format {
	case "bit":
		outputBit(os.Stdout, m)
	case "gray":
		output(m, gray)
	case "alpha":
		output(m, alpha)
	case "rgb":
		output(m, rgb)
	case "rgba":
		output(m, rgba)
	case "bgr":
		output(m, bgr)
	case "bgra":
		output(m, bgra)
	case "cymk":
		output(m, cymk)
	case "ycbcr":
		output(m, ycbcr)
	default:
		return fmt.Errorf("unknown image format %q", *format)
	}

	return nil
}

func outputBit(w io.Writer, m image.Image) {
	v := uint8(0)
	n := uint(0)
	r := m.Bounds()
	for y := r.Min.Y; y < r.Max.Y; y++ {
		for x := r.Min.X; x < r.Max.X; x++ {
			cr, cg, cb, _ := m.At(x, y).RGBA()
			on := cr != 0 && cg != 0 && cb != 0
			if (!*reverse && on) || (*reverse && !on) {
				v |= 1 << n
			}

			if n++; n >= 8 {
				w.Write([]byte{v})
				v = 0
				n = 0
			}
		}
	}
	if n > 0 {
		w.Write([]byte{v})
	}
}

func output(m image.Image, f func(io.Writer, color.Color)) {
	r := m.Bounds()
	for y := r.Min.Y; y < r.Max.Y; y++ {
		for x := r.Min.X; x < r.Max.X; x++ {
			f(os.Stdout, m.At(x, y))
		}
	}
}

func gray(w io.Writer, c color.Color) {
	p := color.GrayModel.Convert(c).(color.Gray)
	w.Write([]byte{p.Y})
}

func alpha(w io.Writer, c color.Color) {
	p := color.AlphaModel.Convert(c).(color.Alpha)
	w.Write([]byte{p.A})
}

func rgb(w io.Writer, c color.Color) {
	p := color.RGBAModel.Convert(c).(color.RGBA)
	w.Write([]byte{p.R, p.G, p.B})
}

func rgba(w io.Writer, c color.Color) {
	p := color.RGBAModel.Convert(c).(color.RGBA)
	w.Write([]byte{p.R, p.G, p.B, p.A})
}

func bgr(w io.Writer, c color.Color) {
	p := color.RGBAModel.Convert(c).(color.RGBA)
	w.Write([]byte{p.B, p.G, p.R})
}

func bgra(w io.Writer, c color.Color) {
	p := color.RGBAModel.Convert(c).(color.RGBA)
	w.Write([]byte{p.B, p.G, p.R, p.A})
}

func cymk(w io.Writer, c color.Color) {
	p := color.RGBAModel.Convert(c).(color.CMYK)
	w.Write([]byte{p.C, p.M, p.Y, p.K})
}

func ycbcr(w io.Writer, c color.Color) {
	p := color.RGBAModel.Convert(c).(color.YCbCr)
	w.Write([]byte{p.Y, p.Cb, p.Cr})
}
