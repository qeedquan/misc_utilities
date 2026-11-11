// +build linux

// ported from toybox

package main

import (
	"flag"
	"fmt"
	"log"
	"syscall"
	"time"
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("uptime: ")
	flag.Parse()

	var info syscall.Sysinfo_t
	var err error
	syscall.Sysinfo(&info)
	ck(err)

	now := time.Now().Format("15:04:05")
	fmt.Printf(" %s up ", now)

	info.Uptime /= 60
	min := info.Uptime % 60
	info.Uptime /= 60
	hours := info.Uptime % 24
	days := info.Uptime / 24
	if days != 0 {
		fmt.Printf("%d day", days)
		if days != 1 {
			fmt.Printf("s")
		}
		fmt.Printf(", ")
	}
	if hours != 0 {
		fmt.Printf("%2d:%02d, ", hours, min)
	} else {
		fmt.Printf("%d min,", min)
	}
	fmt.Printf(" load average: %.02f, %.02f, %.02f\n",
		float64(info.Loads[0])/65536, float64(info.Loads[1])/65536, float64(info.Loads[2])/65536)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}
