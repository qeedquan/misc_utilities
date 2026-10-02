// https://en.wikipedia.org/wiki/Tupper%27s_self-referential_formula
package main

import (
	"flag"
	"image"
	"image/color"
	"image/draw"
	"image/png"
	"math/big"
	"os"
)

var (
	blocksize = flag.Int("s", 16, "block size")
	borders   = flag.Bool("b", false, "draw borders")
)

func main() {
	flag.Parse()

	b := newblocky(*blocksize, *borders)
	tupper(b)
	png.Encode(os.Stdout, b)
}

func tupper(m draw.Image) {
	draw.Draw(m, m.Bounds(), image.NewUniform(color.White), image.ZP, draw.Over)
	for y := 0; y < 17; y++ {
		for x := 0; x <= 106; x++ {
			if intupper(106-x, y) {
				m.Set(x, y, color.RGBA{91, 91, 172, 255})
			}
		}
	}
}

func intupper(x, y int) bool {
	const k = "960939379918958884971672962127852754715004339660129306651505519271702802395266424689642842174350718121267153782770623355993237280874144307891325963941337723487857735749823926629715517173716995165232890538221612403238855866184013235585136048828693337902491454229288667081096184496091705183454067827731551705405381627380967602565625016981482083418783163849115590225610003652351370343874461848378737238198224849863465033159410054974700593138339226497249461751545728366702369745461014655997933798537483143786841806593422227898388722980000748404719"

	K := new(big.Int)
	K.SetString(k, 10)

	Q := big.NewInt(2)
	Q.Exp(Q, big.NewInt(int64(17*x+y%17)), nil)

	R := big.NewInt(int64(y))
	R.Add(R, K)
	R.Quo(R, big.NewInt(17))
	R.Quo(R, Q)
	R.Mod(R, big.NewInt(2))

	return float64(R.Int64()) > 0.5
}

type blocky struct {
	*image.RGBA
	size    int
	borders bool
}

func newblocky(size int, borders bool) *blocky {
	return &blocky{
		RGBA:    image.NewRGBA(image.Rect(0, 0, 107*size, 17*size)),
		size:    size,
		borders: borders,
	}
}

func (b *blocky) Set(x, y int, c color.Color) {
	r := image.Rect(x*b.size, y*b.size, (x+1)*b.size, (y+1)*b.size)
	if b.borders {
		draw.Draw(b.RGBA, r, image.NewUniform(color.Black), image.ZP, draw.Over)
		r = r.Inset(1)
	}
	draw.Draw(b.RGBA, r, image.NewUniform(c), image.ZP, draw.Over)
}
