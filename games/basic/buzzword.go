// ported from https://github.com/rlauzon54/PicoCalcBasic/blob/main/buzzword.bas
package main

import (
	"fmt"
	"math/rand"
	"strings"
)

func main() {
	intro()
	for {
		phrase()
		if !input() {
			break
		}
	}
}

func intro() {
	fmt.Println("Buzzword Generator")
	fmt.Println("Creative Computing")
	fmt.Println("Morristown, NJ")
	fmt.Println()
	fmt.Println("This program prints highly acceptable")
	fmt.Println("phrases in 'Educator-speak' that you")
	fmt.Println("can work into reports and speeches.")
	fmt.Println("Whenever a question mark is printed,")
	fmt.Println("Type a 'Y' for another phrase or")
	fmt.Println("'N' to quit.")
	fmt.Println()
	fmt.Println("Here's the first phrase:")
}

func phrase() {
	words := []string{
		"ABILITY", "BASAL", "BEHAVIORAL", "CHILD-CENTERED",
		"DIFFERENTIATED", "DISCOVERY", "FLEXIBLE", "HETEROGENEOUS",
		"HOMOGENEOUS", "MANIPULATIVE", "MODULAR", "TAVISTOCK",
		"INDIVIDUALIZED", "LEARNING", "EVALUATIVE", "OBJECTIVE",
		"COGNITIVE", "ENRICHMENT", "SCHEDULING", "HUMANISTIC",
		"INTEGRATED", "NON-GRADED", "TRAINING", "VERTICAL AGE",
		"MOTIVATIONAL", "CREATIVE", "GROUPING", "MODIFICATION",
		"ACCOUNTABILITY", "PROCESS", "CORE CURRICULUM", "ALGORITHM",
		"PERFORMANCE", "REINFORCEMENT", "OPEN CLASSROOM", "RESOURCE",
		"STRUCTURE", "FACILITY", "ENVIRONMENT",
	}

	i1 := rand.Intn(13) + 1
	i2 := rand.Intn(13) + 14
	i3 := rand.Intn(13) + 27
	fmt.Println(words[i1-1], words[i2-1], words[i3-1])
}

func input() bool {
	var str string
	fmt.Printf("? ")
	_, err := fmt.Scan(&str)
	if err != nil {
		fmt.Println()
		return false
	}
	return strings.ToUpper(str) == "Y"
}
