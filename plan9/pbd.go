package main

import (
	"fmt"
	"os"
	"strings"
)

func main() {
	pwd, err := os.Getwd()
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}

	i := strings.LastIndex(pwd, string(os.PathSeparator))
	if i >= 0 && len(pwd[i+1:]) > 1 {
		pwd = pwd[i+1:]
	}
	fmt.Println(pwd)
}
