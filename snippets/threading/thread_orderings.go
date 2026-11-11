/*

Given a set of threads each with a number of operations to execute before finishing,
this program generates all thread orderings that can be executed

Special cases:

For N threads each with exactly M operations, the number of orderings that can be made is:
(N*M)! / (M!)^N

*/

package main

import (
	"flag"
	"fmt"
	"os"
	"strconv"
)

var (
	silent = flag.Bool("s", false, "silent mode")
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	var ops []int
	for _, arg := range flag.Args() {
		val, _ := strconv.Atoi(arg)
		ops = append(ops, val)
	}

	fmt.Println(orderings(ops))
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] <ops> ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func orderings(ops []int) int {
	var (
		count  int
		result []int
	)

	state := make([]int, len(ops))
	recurse(ops, state, &result, &count)
	return count
}

func recurse(ops, state []int, result *[]int, count *int) {
	choices := genchoices(ops, state)
	if len(choices) == 0 {
		if !*silent {
			fmt.Println(*result)
		}
		*count += 1
		return
	}

	for _, i := range choices {
		state[i] += 1
		*result = append(*result, i+1)

		recurse(ops, state, result, count)

		state[i] -= 1
		*result = (*result)[:len(*result)-1]
	}
}

func genchoices(ops, state []int) []int {
	var choices []int
	for i := range state {
		if state[i] < ops[i] {
			choices = append(choices, i)
		}
	}
	return choices
}
