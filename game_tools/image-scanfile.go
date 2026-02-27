// scan a file for raw embedded images and dump them out
package main

import (
	"bufio"
	"bytes"
	"flag"
	"fmt"
	"image"
	_ "image/gif"
	_ "image/jpeg"
	"image/png"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
)

var opt struct {
	outdir string
	prefix string
}

func main() {
	log.SetFlags(0)
	log.SetPrefix("image-scanfile: ")

	parseflags()

	imgs, err := scan(flag.Arg(0), sigtab)
	ck(err)

	os.MkdirAll(opt.outdir, 0755)
	for i, m := range imgs {
		name := fmt.Sprintf("%s/%s%d.png", opt.outdir, opt.prefix, i)
		name = filepath.Clean(name)

		f, err := os.Create(name)
		if err != nil {
			fmt.Fprintln(os.Stderr, err)
			continue
		}

		w := bufio.NewWriter(f)
		png.Encode(w, m)
		f.Close()
		fmt.Printf("Wrote %s\n", name)
	}
	if len(imgs) == 0 {
		fmt.Printf("No supported image files found\n")
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] file")
	flag.PrintDefaults()
	os.Exit(2)
}

func parseflags() {
	flag.StringVar(&opt.outdir, "o", ".", "output directory")
	flag.StringVar(&opt.prefix, "p", "", "add prefix to output names")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func scan(name string, sigs [][]byte) ([]image.Image, error) {
	b, err := ioutil.ReadFile(name)
	if err != nil {
		return nil, err
	}

	var imgs []image.Image
	for _, sig := range sigs {
		i := 0
		for {
			n := bytes.Index(b[i:], sig)
			if n < 0 {
				break
			}
			i += n

			r := bytes.NewReader(b[i:])
			m, typ, err := image.Decode(r)
			if err == nil {
				fmt.Printf("Found %s image at offset %x\n", typ, i)
				imgs = append(imgs, m)
			}

			i += len(sig)
		}
	}

	return imgs, nil
}

var sigtab = [][]byte{
	[]byte("\x89PNG\r\n\x1a\n"), // png
	[]byte("\xff\xd8"),          // jpeg
	[]byte("GIF8"),              // gif
}
