// ported from https://github.com/rlauzon54/PicoCalcBasic/blob/main/lunar.bas
package main

import (
	"fmt"
	"math"
	"strings"
)

const (
	RESET = iota + 1
	NAVIGATE
	NO_FUEL
	BURN_CORRECTION
	SOFT_LANDING
	TRY_AGAIN
	FINISH
)

var (
	timeSec      float64
	altitude     float64
	velocity     float64
	totalMass    float64
	fuelMass     float64
	gravity      float64
	thrustFactor float64

	burnRate float64
	burnTime float64

	state int
)

func main() {
	play()
}

func intro() {
	fmt.Println(strings.Repeat(" ", 33))
	fmt.Println("LUNAR")
	fmt.Println(strings.Repeat(" ", 15))
	fmt.Println("CREATIVE COMPUTING MORRISTOWN, NEW JERSEY")
	fmt.Println()
	fmt.Println()
	fmt.Println()

	fmt.Println("THIS IS A COMPUTER SIMULATION OF AN APOLLO LUNAR")
	fmt.Println("LANDING CAPSULE.")
	fmt.Println()
	fmt.Println()
	fmt.Println("THE ON-BOARD COMPUTER HAS FAILED (IT WAS MADE BY")
	fmt.Println("XEROX) SO YOU HAVE TO LAND THE CAPSULE MANUALLY.")
	fmt.Println()
	fmt.Println("SET BURN RATE OF RETRO ROCKETS TO ANY VALUE BETWEEN")
	fmt.Println("0 (FREE FALL) AND 200 (MAXIMUM BURN) POUNDS PER SECOND.")
	fmt.Println("SET NEW BURN RATE EVERY 10 SECONDS.")
	fmt.Println()
	fmt.Println("CAPSULE WEIGHT 32,500 LBS; FUEL WEIGHT 16,500 LBS.")
	fmt.Println()
	fmt.Println()
	fmt.Println("GOOD LUCK")
}

func play() {
	intro()
	state = RESET
	for {
		switch state {
		case RESET:
			reset()
		case NAVIGATE:
			state = navigate()
		case NO_FUEL:
			state = noFuel()
		case BURN_CORRECTION:
			state = burnCorrection()
		case SOFT_LANDING:
			state = softLanding()
		case TRY_AGAIN:
			state = tryAgain()
		case FINISH:
			fmt.Println()
			fmt.Println("THANKS FOR PLAYING!")
			fmt.Println()
			return
		}
	}
}

func reset() {
	timeSec = 0
	altitude = 120
	velocity = 1
	totalMass = 33000
	fuelMass = 16500
	gravity = 1e-03
	thrustFactor = 1.8

	state = NAVIGATE
}

func navigate() int {
	fmt.Println()
	fmt.Println("SEC", "MI + FT", "MPH", "LB FUEL", "BURN RATE")
	fmt.Println()
	for {
		miles := math.Floor(altitude)
		feet := math.Floor(5280 * (altitude - miles))
		mph := 3600 * velocity
		fuelRemaining := totalMass - fuelMass

		fmt.Printf("%.3f %.3f + %.3f %.3f %.3f\n", timeSec, miles, feet, mph, fuelRemaining)

		fmt.Printf("? ")
		_, err := fmt.Scan(&burnRate)
		if err != nil {
			return FINISH
		}

		burnTime = 10
		if fuelRemaining < 1e-03 {
			return NO_FUEL
		}

		for burnTime >= 1e-3 {
			if totalMass < fuelMass+burnTime*burnRate {
				burnTime = (totalMass - fuelMass) / burnRate
			}

			// Calculate thrust acceleration and update states
			q := burnTime * burnRate / totalMass
			q2 := q * q
			q3 := q2 * q
			q4 := q3 * q
			q5 := q4 * q
			nextVelocity := velocity + gravity*burnTime + thrustFactor*(-q-q2/2-q3/3-q4/4-q5/5)
			nextAltitude := altitude - gravity*burnTime*burnTime/2 - velocity*burnTime + thrustFactor*burnTime*(q/2+q2/6+q3/12+q4/20+q5/30)

			if nextAltitude <= 0 {
				return SOFT_LANDING
			}
			if velocity <= 0 && nextVelocity > 0 {
				return BURN_CORRECTION
			}

			// Apply state update
			timeSec = timeSec + burnTime
			burnTime = 0
			totalMass = totalMass - burnRate*burnTime
			altitude = nextAltitude
			velocity = nextVelocity
		}
	}
}

func noFuel() int {
	fmt.Printf("FUEL OUT AT %.3f SECONDS\n", timeSec)

	burnTime = (-velocity + SQR(velocity*velocity+2*altitude*gravity)) / gravity
	velocity = velocity + gravity*burnTime
	timeSec = timeSec + burnTime
	impactVelocity := 3600 * velocity

	fmt.Printf("ON MOON AT %.3f SECONDS - IMPACT VELOCITY %.3f MPH\n", timeSec, impactVelocity)

	if impactVelocity <= 1.2 {
		fmt.Println("PERFECT LANDING!")
	} else if impactVelocity <= 10 {
		fmt.Println("GOOD LANDING (COULD BE BETTER)")
	} else if impactVelocity <= 60 {
		fmt.Println("CRAFT DAMAGE... YOU'RE STRANDED UNTIL A RESCUE PARTY ARRIVES.")
		fmt.Println("HOPE YOU HAVE ENOUGH OXYGEN!")
	} else {
		fmt.Println("SORRY, THERE WERE NO SURVIVORS. YOU BLEW IT!")
		fmt.Printf("YOU BLASTED A NEW LUNAR CRATER %.3f FEET DEEP!", impactVelocity*0.227)
	}

	return TRY_AGAIN
}

func burnCorrection() int {
	w := (1 - totalMass*gravity/(thrustFactor*burnRate)) / 2
	burnTime = totalMass*velocity/(thrustFactor*burnRate*(w+SQR(w*w+velocity/thrustFactor))) + 0.05
	return NO_FUEL
}

func softLanding() int {
	if burnTime < 5e-03 {
		return NO_FUEL
	}

	decel := velocity + SQR(velocity*velocity+2*altitude*(gravity-thrustFactor*burnRate/totalMass))
	burnTime = 2 * altitude / decel

	// Recalculate and update state
	q := burnTime * burnRate / totalMass
	q2 := q * q
	q3 := q2 * q
	q4 := q3 * q
	q5 := q4 * q
	nextVelocity := velocity + gravity*burnTime + thrustFactor*(-q-q2/2-q3/3-q4/4-q5/5)
	nextAltitude := altitude - gravity*burnTime*burnTime/2 - velocity*burnTime + thrustFactor*burnTime*(q/2+q2/6+q3/12+q4/20+q5/30)

	timeSec = timeSec + burnTime
	totalMass = totalMass - burnRate*burnTime
	altitude = nextAltitude
	velocity = nextVelocity

	return NO_FUEL
}

func tryAgain() int {
	var selection string
	fmt.Println()
	fmt.Println()
	fmt.Println("TRY AGAIN? (Y/N)")
	_, err := fmt.Scan(&selection)
	if err != nil || strings.ToUpper(selection) != "Y" {
		return FINISH
	}
	return RESET
}

func SQR(x float64) float64 {
	return x * x
}
