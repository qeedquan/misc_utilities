package main

import (
	"bytes"
	"flag"
	"fmt"
	"io"
	"log"
	"mime/multipart"
	"net/http"
	"os"
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("httpformfile: ")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	url := flag.Arg(0)
	if flag.NArg() < 2 {
		errlook(upload(url, "stdin", os.Stdin))
	} else {
		args := flag.Args()
		for _, name := range args[1:] {
			f, err := os.Open(name)
			if errlook(err) {
				continue
			}
			errlook(upload(url, name, f))
			f.Close()
		}
	}
}

func errlook(err error) bool {
	if err != nil {
		log.Println(err)
		return true
	}
	return false
}

func upload(url string, name string, r io.Reader) error {
	buf := new(bytes.Buffer)
	mpw := multipart.NewWriter(buf)
	fw, err := mpw.CreateFormFile("file", name)
	if err != nil {
		return err
	}

	_, err = io.Copy(fw, r)
	if err != nil {
		return err
	}
	mpw.Close()

	req, err := http.NewRequest("POST", url, buf)
	if err != nil {
		return err
	}
	req.Header.Set("Content-Type", mpw.FormDataContentType())

	rsp, err := http.DefaultClient.Do(req)
	if err != nil {
		return err
	}

	if rsp.StatusCode != http.StatusOK {
		return fmt.Errorf("bad status: %v", rsp.Status)
	}

	fmt.Printf("uploaded to %v: %v\n", url, name)
	return nil
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] url <file> ...")
	flag.PrintDefaults()
	os.Exit(2)
}
