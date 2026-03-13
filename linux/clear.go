// +build unix

package main

import "fmt"

func main() {
	fmt.Printf("\x1b[2J\x1b[H")
}
