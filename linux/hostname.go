package main

import (
	"fmt"
	"os"
)

func main() {
	hostname, err := os.Hostname()
	if err != nil {
		fmt.Fprintln(os.Stderr, "hostname:", err)
		os.Exit(1)
	}
	fmt.Println(hostname)
}
