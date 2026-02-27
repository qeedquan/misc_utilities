package main

import (
	"flag"
	"fmt"
	"io"
	"log"
	"net/http"
	"os"
	"path"
	"path/filepath"
)

var (
	addr   = flag.String("addr", ":8080", "address to listen on")
	udir   = flag.String("dir", ".", "directory to upload to")
	status = 0
)

func main() {
	flag.Parse()
	log.Println("Starting server...")
	http.HandleFunc("/", form)
	http.HandleFunc("/upload", upload)
	ek(http.ListenAndServe(*addr, nil))
	os.Exit(status)
}

func ek(err error) {
	if err != nil {
		log.Println(err)
		status = 1
	}
}

const html = `
<html>
	<title>Upload</title>
	<body>
		<form action="upload" method="post" enctype="multipart/form-data">
			<input type="file" name="file" id="file">
			<br />
			<input type="submit" name="submit" value="Submit">
		</form>
	</body>
</html>
`

func form(w http.ResponseWriter, r *http.Request) {
	log.Printf("form request from %q\n", r.RemoteAddr)
	fmt.Fprintln(w, html)
}

func upload(w http.ResponseWriter, r *http.Request) {
	log.Printf("upload request from %q\n", r.RemoteAddr)

	var err error
	defer func() {
		if err != nil {
			log.Printf("%s: %s\n", r.RemoteAddr, err)
			fmt.Fprintln(w, err)
		}
	}()

	f, h, err := r.FormFile("file")
	if err != nil {
		return
	}
	defer f.Close()

	o, err := os.Create(filepath.Join(*udir, path.Base(h.Filename)))
	if err != nil {
		return
	}

	_, err = io.Copy(o, f)
	if err != nil {
		return
	}

	err = o.Close()
	if err != nil {
		return
	}

	log.Printf("%s: upload request %q completed", r.RemoteAddr, h.Filename)
	fmt.Fprintf(w, "File uploaded successfully: %s", h.Filename)
}
