package main

import (
	"fmt"
	"io/ioutil"
	"path/filepath"
	"strconv"
	"strings"
)

func main() {
	dump("/sys/bus/w1/devices/*")
}

func dump(pat string) {
	ndevs := 0
	dirs, _ := filepath.Glob(pat)
	for _, dir := range dirs {
		file := filepath.Join(dir, "temperature")
		buf, err := ioutil.ReadFile(file)
		if err != nil {
			continue
		}

		str := strings.TrimSpace(string(buf))
		val, err := strconv.Atoi(str)
		if err != nil {
			continue
		}

		dev := filepath.Base(dir)
		cel := float64(val) / 1000.0
		fahr := cel*9.0/5.0 + 32
		fmt.Printf("%s - %v C %v F\n", dev, cel, fahr)
		ndevs++
	}
	if ndevs == 0 {
		fmt.Println("No W1 temperature devices found")
	}
}
