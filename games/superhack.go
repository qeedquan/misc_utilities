// ported from eric raymond game
// http://www.catb.org/~esr/wumpus/
package main

import (
	"bufio"
	"flag"
	"fmt"
	"math/rand"
	"os"
	"time"
	"unicode"
	"unicode/utf8"
)

const (
	You = iota
	Rms
	Starlet1
	Starlet2
	Droid1
	Droid2
	Luser1
	Luser2
	Locs
)

const (
	Not  = 0
	Win  = 1
	Lose = -1
)

var cave = [20][3]int{
	{1, 4, 7},
	{0, 2, 9},
	{1, 3, 11},
	{2, 4, 13},
	{0, 3, 5},
	{4, 6, 14},
	{5, 7, 16},
	{0, 6, 8},
	{7, 9, 17},
	{1, 8, 10},
	{9, 11, 18},
	{2, 10, 12},
	{11, 13, 19},
	{3, 12, 14},
	{5, 13, 15},
	{14, 16, 19},
	{6, 15, 17},
	{8, 16, 18},
	{10, 17, 19},
	{12, 15, 18},
}

var (
	stdin = bufio.NewScanner(os.Stdin)
	inp   string

	seed = flag.Int64("s", time.Now().UnixNano(), "seed to use")

	loc        [Locs]int
	scratchloc int
	pies       int
	finished   int
)

func fna() int { return rand.Intn(20) }
func fnb() int { return rand.Intn(3) }
func fnc() int { return rand.Intn(4) }

func getlet(prompt string) rune {
	fmt.Printf("%s? ", prompt)
	if !stdin.Scan() {
		fmt.Println()
		os.Exit(1)
	}
	inp = stdin.Text()
	r, _ := utf8.DecodeRuneInString(inp)
	return unicode.ToLower(r)
}

func getargs() string {
	r, size := utf8.DecodeRuneInString(inp)
	if !unicode.IsLetter(r) {
		return inp
	}
	return inp[size:]
}

func printInstructions() {
	fmt.Println("Welcome to `Hunt the Superhack'\n")

	fmt.Println("   The superhack lives on the 9th floor of 45 Technology Square in")
	fmt.Println("Cambridge, Massachusetts.  Your mission is to throw a pie in his face.\n")

	fmt.Println("   First, you'll have to find him.  A botched experiment by an MIT")
	fmt.Println("physics group has regularized the floor's topology, so that each")
	fmt.Println("room has exits to three other rooms.  (Look at a dodecahedron to")
	fmt.Println("see how this works --- if you don't know what a dodecahedron is,")
	fmt.Println("ask someone.)\n")

	fmt.Println("You:")
	fmt.Println("   Each turn you may move to an adjacent room or throw a pie.  If")
	fmt.Println("you run out of pies, you lose.  Each pie can pass through up to")
	fmt.Println("five rooms (connected by a continuous path from where you are).  You")
	fmt.Println("aim by telling the computer which rooms you want to throw through.")
	fmt.Println("If the path is incorrect (presumes a nonexistent connection) the ")
	fmt.Println("pie moves at random.")

	fmt.Println("   If a pie hits the superhack, you win. If it hits you, you lose!\n")

	fmt.Println("<Press return to continue>")
	stdin.Scan()
	fmt.Println()

	fmt.Println("Hazards:")
	fmt.Println("   Starlets --- two rooms contain lonely, beautiful women.  If you")
	fmt.Println("enter these, you will become fascinated and forget your mission as")
	fmt.Println("you engage in futile efforts to pick one up.  You weenie.")
	fmt.Println("   Droids --- two rooms are guarded by experimental AI security ")
	fmt.Println("droids.  If you enter either, the droid will grab you and hustle")
	fmt.Println("you off to somewhere else, at random.")
	fmt.Println("   Lusers --- two rooms contain hungry lusers.  If you blunder into")
	fmt.Println("either, they will eat one of your pies.")
	fmt.Println("   Superhack --- the superhack is not bothered by hazards (the")
	fmt.Println("lusers are in awe of him, he's programmed the droids to ignore him,")
	fmt.Println("and he has no sex life).  Usually he is hacking.  Two things can")
	fmt.Println("interrupt him; you throwing a pie or you entering his room.\n")
	fmt.Println("   On an interrupt, the superhack moves (3/4 chance) or stays where")
	fmt.Println("he is (1/4 chance).  After that, if he is where you are, he flames")
	fmt.Println("you and you lose!\n")

	fmt.Println("<Press return to continue>")
	stdin.Scan()
	fmt.Println()

	fmt.Println("Warnings:")
	fmt.Println("   When you are one room away from the superhack or a hazard,")
	fmt.Println("the computer says:")
	fmt.Println("   superhack:       \"I smell a superhack!\"")
	fmt.Println("   security droid:  \"Droids nearby!\"")
	fmt.Println("   starlet:         \"I smell perfume!\"")
	fmt.Println("   luser:           \"Lusers nearby!\"")

	fmt.Println("If you take too long finding the superhack, hazards may move.  You")
	fmt.Println("will get a warning when this happens.\n")

	fmt.Println("Commands:")
	fmt.Println("   Available commands are:\n")
	fmt.Println("  ?            --- print long instructions.")
	fmt.Println("  m <number>   --- move to room with given number.")
	fmt.Println("  t <numbers>  --- throw through given rooms.")
	fmt.Println("  d            --- dump hazard locations.")
	fmt.Println("  q            --- quit the game.")
	fmt.Println("\nThe list of room numbers after t must be space-separated.  Anything")
	fmt.Println("other than one of these commands displays a short help message.")
}

func moveHazard(where int) {
retry:
	newloc := fna()
	for j := 0; j < Locs; j++ {
		if loc[j] == newloc {
			goto retry
		}
	}

	loc[where] = newloc
}

func checkHazards() {
	// basic status report
	fmt.Printf("You are in room %d.  Exits lead to %d, %d, %d.  You have %d pies left.\n",
		loc[You]+1,
		cave[loc[You]][0]+1,
		cave[loc[You]][1]+1,
		cave[loc[You]][2]+1,
		pies)

	// maybe it's migration time
	if fna() == 0 {
		switch 2 + fnb() {
		case Starlet1, Starlet2:
			fmt.Println("Swish, swish, swish --- starlets are moving!")
			moveHazard(Starlet1)
			moveHazard(Starlet2)

		case Droid1, Droid2:
			fmt.Println("Clank, clank, clank --- droids are moving!")
			moveHazard(Droid1)
			moveHazard(Droid2)

		case Luser1, Luser2:
			fmt.Println("Grumble, grumble, grumble --- lusers are moving!")
			moveHazard(Luser1)
			moveHazard(Luser2)
		}
	}

	// display hazard warnings
	for k := 0; k < 3; k++ {
		room := cave[loc[You]][k]

		switch room {
		case loc[Rms]:
			fmt.Println("I smell a superhack!")
		case loc[Starlet1], loc[Starlet2]:
			fmt.Println("I smell perfume!")
		case loc[Droid1], loc[Droid2]:
			fmt.Println("Droids nearby!")
		case loc[Luser1], loc[Luser2]:
			fmt.Println("Lusers nearby!")
		}
	}
}

func throw() {
	var path [5]int
	j9, _ := fmt.Sscan(getargs(), &path[0], &path[1], &path[2], &path[3], &path[4])
	if j9 < 1 {
		fmt.Println("Sorry, I didn't see any room numbers after your throw command.")
		return
	}

	for k := 0; k < j9; k++ {
		if k >= 2 && path[k] == path[k-2] {
			fmt.Println("Pies can't fly that crookedly --- try again.")
			return
		}
	}

	scratchloc = loc[You]

	for k := 0; k < j9; k++ {
		for k1 := 0; k1 < 3; k1++ {
			if cave[scratchloc][k1] == path[k] {
				scratchloc = path[k]
				checkShot()
				if finished != Not {
					return
				}

			}

			scratchloc = cave[scratchloc][fnb()]
			checkShot()
		}
	}

	if finished == Not {
		fmt.Println("You missed.")

		scratchloc = loc[You]

		moveSuperHack()

		if pies--; pies <= 0 {
			finished = Lose
		}
	}
}

func checkShot() {
	if scratchloc == loc[Rms] {
		fmt.Println("Splat!  You got the superhack!  You win.")
		finished = Win
	} else if scratchloc == loc[You] {
		fmt.Println("Ugh!  The pie hit you!  You lose.")
		finished = Lose
	}
}

func moveSuperHack() {
	k := fnc()
	if k < 3 {
		loc[Rms] = cave[loc[Rms]][k]
	}

	if loc[Rms] != loc[You] {
		return
	}

	fmt.Println("The superhack flames you to a crisp.  You lose!")

	finished = Lose
}

func move() {
	if n, _ := fmt.Sscan(getargs(), &scratchloc); n < 1 {
		fmt.Println("Sorry, I didn't see a room number after your `m' command.")
		return
	}
	scratchloc--

	for k := 0; k < 3; k++ {
		if cave[loc[You]][k] == scratchloc {
			goto goodmove
		}
	}

	fmt.Println("You can't get there from here!")
	return

goodmove:
	loc[You] = scratchloc

	switch scratchloc {
	case loc[Rms]:
		fmt.Println("Yow! You interrupted the superhack.")
		moveSuperHack()
	case loc[Starlet1], loc[Starlet2]:
		fmt.Println("You begin to babble at an unimpressed starlet.  You lose!")
		finished = Lose
	case loc[Droid1], loc[Droid2]:
		fmt.Println("Zap --- security droid snatch.  Elsewheresville for you!")
		scratchloc = fna()
		loc[You] = scratchloc
		goto goodmove
	case loc[Luser1], loc[Luser2]:
		fmt.Println("Munch --- lusers ate one of your pies!")
		pies--
	}
}

func main() {
	flag.Parse()
	rand.Seed(*seed)

	for {
	badlocs:
		for j := 0; j < Locs; j++ {
			loc[j] = fna()
		}

		for j := 0; j < Locs; j++ {
			for k := 0; k < Locs; k++ {
				if j == k {
					continue
				} else if loc[j] == loc[k] {
					goto badlocs
				}
			}
		}

		fmt.Println("Hunt the Superhack")

		pies = 5
		scratchloc = loc[You]
		finished = Not

		for finished == Not {
			checkHazards()

			c := getlet("Throw, move, dump, quit or help [t,m,d,q,?]")
			switch c {
			case 't':
				throw()
			case 'm':
				move()
			case '?':
				printInstructions()
			case 'd':
				fmt.Printf("RMS is at %d, starlets at %d/%d, droids %d/%d, lusers %d/%d\n",
					loc[Rms]+1,
					loc[Starlet1]+1, loc[Starlet2]+1,
					loc[Droid1]+1, loc[Droid2]+1,
					loc[Luser1]+1, loc[Luser2]+1)
			case 'q':
				fmt.Println("Quitting!")
				return
			default:
				fmt.Println("Available commands are:\n")
				fmt.Println("  ?            --- print long instructions.")
				fmt.Println("  m <number>   --- move to room with given number.")
				fmt.Println("  t <numbers>  --- throw through given rooms.")
				fmt.Println("  d            --- dump hazard locations.")
				fmt.Println("  q            --- quit the game.")
				fmt.Println("The list of room numbers after t must be space-separated")
			}

			fmt.Println()
		}

		if getlet("Play again") != 'y' {
			fmt.Println("Happy hacking!")
			break
		}
	}
}
