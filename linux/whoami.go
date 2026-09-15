package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"os/user"
)

func main() {
	log.SetFlags(0)
	flag.Usage = usage
	flag.Parse()
	user, err := user.Current()
	if err != nil {
		log.Fatal("whoami: ", err)
	}
	fmt.Println(user.Username)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options]")
	flag.PrintDefaults()
	os.Exit(1)
}
