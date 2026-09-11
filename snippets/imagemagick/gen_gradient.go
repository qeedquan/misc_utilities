/*

https://imagemagick.org/gradient/
https://www.fmwconcepts.com/imagemagick/multigradient/index.php

*/

package main

import "fmt"

func main() {
	w := 673
	h := 422

	// imagemagick only supports linear/radial gradients of 2 colors
	// to get more than 2 colors, need to use various composting techniques
	// like the multigradient script above

	/*
		css:
		linear-gradient(to bottom, white, black);
		linear-gradient(to bottom, red, blue);
		linear-gradient(45deg, red, blue);
		linear-gradient(135deg, blue, yellow);
	*/
	gradient(w, h, "gradient:white-black")
	gradient(w, h, "gradient:red-blue")
	gradient(w, h, "-define gradient:angle=45 gradient:red-blue")
	gradient(w, h, "-define gradient:angle=135 gradient:blue-yellow")

	/*
		css:
		radial-gradient(white, black);
		radial-gradient(red, blue);
		radial-gradient(blue, yellow);
		radial-gradient(at -50px -50px, blue, yellow);
	*/
	gradient(w, h, "radial-gradient:white-black")
	gradient(w, h, "radial-gradient:red-blue")
	gradient(w, h, "radial-gradient:blue-yellow")
	gradient(w, h, "-define gradient:vector=-50,0,50,0 radial-gradient:blue-yellow")

	gradient(w, h, "-define gradient:radii=128,64 -define gradient:angle=45 radial-gradient:black-white")
}

func gradient(w, h int, args string) {
	fmt.Printf("magick -size %dx%d %s 'gradient_%dx%d_(%s).png'\n", w, h, args, w, h, args)
}
