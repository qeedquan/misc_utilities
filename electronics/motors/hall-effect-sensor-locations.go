/*

http://mitrocketscience.blogspot.com/2011/08/hall-effect-sensor-placement-for.html

*/

package main

import (
	"fmt"
	"math"
)

func main() {
	// 18 stator slots, 20 poles, so 10 pairs (20/2)
	location(360, 10, 18)
}

/*

Legend:

edeg : electrical degrees
erot: electrical rotation. 1 erot = 360 edeg
mdeg : mechanical degrees
mrot: mechanical rotation. 1 mrot = 360 mdeg
pp : number of magnet pole pairs . 1 pp = 2 magnets (1 north, 1 south)
s : number of slots (in the stator)

We want to place hall effect sensors onto a three phase motor system.
The most common three phase motors setup is to place the sensors 60 or 120 edeg apart.

We are working in two spaces here, mechanical and electrical and need to have a formula to translate them.
We place the sensor in mechanical degree space but this corresponds to a different electrical degree apart.
Off by a few mechanical degrees can have a big effect on electrical degrees (tens of electrical degrees)
so it is important to get it right.

Eq 1:
360 mdeg / pp = n mdeg per erot = n mdeg per 360 edeg

Use above equation to derive the equation for 120 edeg apart, just divide by 3 to get:

360 mdeg / (3*pp) = m mdeg per 120 edeg

If we want to place the hall sensors on a jig/board, then we are done.
Can multiply m with any integer value to get more spacing in mechanical degree space but will still work with n edeg controllers

If we want to put the hall sensors in the stator slots, need to find number of mdeg per slot
360 mdeg / s = x mdeg per slot

Now need to find a multiple i such that m*i is divisible by x.
m*i / x = # of slots between hall effect sensors

*/

func location(mdeg, pp, slots float64) {
	mdeg360 := mdeg / pp
	mdeg120 := mdeg / (3 * pp)
	mdeg60 := mdeg / (6 * pp)

	fmt.Printf("%v mdeg per 360 edeg\n", mdeg360)
	fmt.Printf("%v mdeg per 120 edeg\n", mdeg120)
	fmt.Printf("%v mdeg per 60 edeg\n", mdeg60)
	fmt.Println()

	mdegslot := mdeg / slots
	multiple360 := multiple(mdeg360, mdegslot)
	multiple120 := multiple(mdeg120, mdegslot)
	multiple60 := multiple(mdeg60, mdegslot)
	fmt.Printf("%v mdeg per slot\n", mdegslot)
	fmt.Printf("%v stator slots between hall effect sensors (360 edeg)\n", mdeg360*multiple360/mdegslot)
	fmt.Printf("%v stator slots between hall effect sensors (120 edeg)\n", mdeg120*multiple120/mdegslot)
	fmt.Printf("%v stator slots between hall effect sensors (60 edeg)\n", mdeg60*multiple60/mdegslot)
}

func multiple(a, b float64) float64 {
	for i := 1.0; ; i++ {
		c := a * i
		r := c / b
		v := r - math.Floor(r)
		if v < 1e-3 {
			return i
		}
	}
}
