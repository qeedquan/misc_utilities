package main

import (
	"bytes"
	"flag"
	"fmt"
	"io"
	"mime/multipart"
	"net/http"
	"net/url"
	"os"
	"strings"
)

var (
	status = 0
)

func main() {
	var values MultiValue
	var formName string

	flag.Usage = usage
	flag.Var(&values, "D", "define value -D=key=value; can be set multiple times")
	flag.StringVar(&formName, "n", "file", "form name")
	flag.Parse()

	if flag.NArg() < 1 {
		usage()
	}

	data := url.Values{}
	for _, v := range values {
		toks := strings.Split(v, "=")
		if len(toks) != 2 {
			continue
		}
		data.Set(toks[0], toks[1])
	}

	url := flag.Arg(0)
	for _, file := range flag.Args()[1:] {
		ek(upload(url, file, formName, data))
	}

	os.Exit(status)
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "postupload:", err)
		status = 1
	}
	return status != 0
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: postupload [options] url file ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func upload(url, file, formName string, data url.Values) (err error) {
	var b bytes.Buffer
	w := multipart.NewWriter(&b)
	f, err := os.Open(file)
	if err != nil {
		return
	}
	defer f.Close()
	fw, err := w.CreateFormFile(formName, file)
	if err != nil {
		return
	}
	if _, err = io.Copy(fw, f); err != nil {
		return
	}

	for key := range data {
		if fw, err = w.CreateFormField(key); err != nil {
			return
		}
		if _, err = fw.Write([]byte(data.Get(key))); err != nil {
			return
		}
	}

	w.Close()

	req, err := http.NewRequest("POST", url, &b)
	if err != nil {
		return
	}
	req.Header.Set("Content-Type", w.FormDataContentType())

	client := &http.Client{}
	res, err := client.Do(req)
	if err != nil {
		return
	}

	if res.StatusCode != http.StatusOK {
		err = fmt.Errorf("bad status: %s", res.Status)
	}
	return
}

type MultiValue []string

func (v *MultiValue) String() string {
	if len(*v) == 0 {
		return ""
	}
	return fmt.Sprint(*v)
}

func (v *MultiValue) Set(val string) error {
	(*v) = append(*v, val)
	return nil
}
