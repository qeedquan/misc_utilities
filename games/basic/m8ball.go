// ported from https://github.com/rlauzon54/PicoCalcBasic/blob/main/m8ball.bas
package main

import (
	"fmt"
	"math/rand"
)

func main() {
	outlook := []string{
		"It is certain",
		"It is decidely so",
		"Without a doubt",
		"Yes, definitely",
		"You may rely on it",
		"As I see it, Yes",
		"Most likely",
		"Outlook good",
		"Yes",
		"Signs point to Yes",
		"Reply hazy. Try again",
		"Ask again later",
		"Better not tell you now",
		"Cannot predict now",
		"Concentrate and ask again",
		"Do not count on it",
		"My reply is No",
		"My sources say No",
		"Outlook not so good",
		"Very doubtful",
	}

	var (
		line   string
		chance = rand.Intn(len(outlook))
	)
	fmt.Println("What is your question?")
	fmt.Print(" ) ")
	_, err := fmt.Scanln(&line)
	if err != nil {
		fmt.Println()
		return
	}
	fmt.Println(" (", outlook[chance])
}
