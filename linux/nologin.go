// ported from suckless
package main

import (
	"fmt"
	"io"
	"os"
)

func main() {
	f, err := os.Open("/etc/login.txt")
	if err != nil {
		fmt.Println("The account is currently unavailable")
	} else {
		io.Copy(os.Stdout, f)
		f.Close()
	}
	os.Exit(1)
}
