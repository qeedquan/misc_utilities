package main

import (
	"flag"
	"fmt"
	"io"
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

	flag.Usage = usage
	flag.Var(&values, "D", "define value -D=key=value; can be set multiple times")
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

	for _, url := range flag.Args() {
		resp, err := http.PostForm(url, data)
		if ek(err) {
			continue
		}
		fmt.Println(resp.Header)
		io.Copy(os.Stdout, resp.Body)
		resp.Body.Close()
	}

	os.Exit(status)
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "postform:", err)
		status = 1
	}
	return status != 0
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: postform [options] url ...")
	flag.PrintDefaults()
	os.Exit(2)
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
