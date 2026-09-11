package main

import (
	"fmt"
	"os"
)

func main() {
	pwd, err := os.Getwd()
	if err != nil {
		fmt.Fprintln(os.Stderr, "pwd:", err)
		os.Exit(1)
	}
	fmt.Println(pwd)
}
