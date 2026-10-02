package main

import (
	"flag"
	"fmt"
	"log"
	"strconv"
)

var table = [12]string{
	"C", "Db", "D", "Eb",
	"E", "F", "Gb", "G",
	"Ab", "A", "Bb", "B",
}

func main() {
	var series [12][12]int

	log.SetFlags(0)
	flag.Parse()
	if flag.NArg() != 12 {
		log.Fatalln("usage: note1 ... note12")
	}

	for i := 0; i < 12; i++ {
		n, err := strconv.Atoi(flag.Arg(i))
		if err != nil {
			log.Fatalln(err)
		}
		series[0][i] = mod12(n)
	}

	for i := 1; i < 12; i++ {
		series[i][0] = mod12(series[i-1][0] + series[0][i-1] - series[0][i])
	}

	for i := 1; i < 12; i++ {
		for j := 1; j < 12; j++ {
			series[i][j] = mod12(series[0][j] + series[i][0] - series[0][0])
		}
	}

	for i := 0; i < 12; i++ {
		for j := 0; j < 12; j++ {
			fmt.Printf(" %v ", table[series[i][j]])
		}
		fmt.Printf("\n")
	}
}

func mod12(note int) int {
	for note >= 12 {
		note -= 12
	}
	for note < 0 {
		note += 12
	}
	return note
}
