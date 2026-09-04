package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"sort"
	"strings"

	"github.com/qeedquan/go-media/image/imageutil"
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("imgcmp: ")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 2 {
		usage()
	}

	status := 0
	p1 := walk(flag.Arg(0))
	p2 := walk(flag.Arg(1))
	n := min(len(p1), len(p2))
	for i := 0; i < n; i++ {
		same, err := cmp(p1[i], p2[i])
		if err != nil {
			fmt.Fprintln(os.Stderr, "imgcmp: ", err)
			status = 1
			continue
		}

		if same {
			fmt.Println(p1[i], p2[i], "same")
		} else {
			status = 1
		}
	}
	os.Exit(status)
}

func min(a, b int) int {
	if a < b {
		return a
	}
	return b
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: imgcmp [options] file1 file2")
	flag.PrintDefaults()
	os.Exit(2)
}

func walk(path string) []string {
	fi, err := os.Stat(path)
	if err != nil {
		return nil
	}
	if !fi.IsDir() {
		return []string{path}
	}

	var paths []string
	filepath.Walk(path, func(name string, info os.FileInfo, err error) error {
		ext := filepath.Ext(name)
		ext = strings.ToLower(ext)
		switch ext {
		case ".png", ".gif", ".jpg", ".jpeg", ".bmp", ".tga", ".tif":
			paths = append(paths, name)
		}
		return nil
	})
	sort.Strings(paths)
	return paths
}

func cmp(file1, file2 string) (bool, error) {
	m1, err := imageutil.LoadRGBAFile(file1)
	if err != nil {
		return false, err
	}

	m2, err := imageutil.LoadRGBAFile(file2)
	if err != nil {
		return false, err
	}

	same := true
	r1 := m1.Bounds()
	r2 := m2.Bounds()
	r := r1.Union(r2)
	for y := r.Min.Y; y < r.Max.Y; y++ {
		for x := r.Min.X; x < r.Max.X; x++ {
			c1 := m1.At(x, y)
			c2 := m2.At(x, y)
			if c1 != c2 {
				same = false
				fmt.Println(file1, file2, x, y, c1, c2)
			}
		}
	}
	return same, nil
}
