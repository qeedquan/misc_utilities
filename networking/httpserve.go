package main

import (
	"flag"
	"fmt"
	"log"
	"net/http"
	"os"
	"path/filepath"
)

type Logger struct {
	handler http.Handler
}

func (l *Logger) ServeHTTP(w http.ResponseWriter, req *http.Request) {
	str := "{\n"
	str += fmt.Sprintf("\tHost           %v\n", req.Host)
	str += fmt.Sprintf("\tRemote Addr    %v\n", req.RemoteAddr)
	str += fmt.Sprintf("\tRequest URI    %v\n", req.RequestURI)
	str += fmt.Sprintf("\tHost           %v\n", req.Host)
	str += fmt.Sprintf("\tMethod         %v\n", req.Method)
	str += fmt.Sprintf("\tURL            %v\n", req.URL)
	str += "}"
	log.Printf("\n%s\n\n", str)

	l.handler.ServeHTTP(w, req)
}

var (
	addr = flag.String("addr", ":8080", "address to bind to")
)

func main() {
	flag.Usage = usage
	flag.Parse()

	var dir string
	var err error
	if flag.NArg() == 0 {
		dir, err = os.Getwd()
	} else {
		dir, err = filepath.Abs(flag.Arg(0))
	}
	if err != nil {
		log.Fatal(err)
	}
	log.Printf("Serving %q on %q", dir, *addr)
	log.Fatal(http.ListenAndServe(*addr, &Logger{http.FileServer(http.Dir(dir))}))
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] [dir]")
	flag.PrintDefaults()
	os.Exit(2)
}
