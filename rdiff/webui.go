package main

import (
	"bytes"
	"flag"
	"fmt"
	"html/template"
	"io"
	"log"
	"net/http"
	"os"
	"os/exec"
	"path/filepath"
	"sort"
	"strings"
)

type WebUI struct {
	webroot string
	tmpls   *template.Template
	v0, v1  []View
}

func RunUI(addr, webroot string, v0, v1 []View) {
	log.Println("starting web-ui listening on", addr, "serving", webroot)

	pattern := filepath.Join(webroot, "*.html")
	funcmap := template.FuncMap{
		"revision": revision,
	}
	tmpls := template.Must(template.New("").Funcs(funcmap).ParseGlob(pattern))
	found := false
	for _, t := range tmpls.Templates() {
		if t.Name() == "base.html" {
			tmpls = t
			found = true
			break
		}
	}
	if !found {
		log.Fatalln("could not find base.html template")
	}

	wui := &WebUI{
		webroot: webroot,
		tmpls:   tmpls,
		v0:      v0,
		v1:      v1,
	}

	http.Handle("/", wui)
	http.Handle("/view", wui)
	http.Handle("/mod", wui)
	staticdir := filepath.Join(webroot, "static")
	http.Handle("/static/", http.StripPrefix("/static/", http.FileServer(http.Dir(staticdir))))

	err := http.ListenAndServe(addr, nil)
	if err != nil {
		log.Fatalln(err)
	}
}

func (wui *WebUI) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	log.Printf("Request: Addr: %v URL: %v, Method: %v\n", r.RemoteAddr, r.URL, r.Method)
	switch {
	case strings.HasPrefix(r.URL.Path, "/view"):
		wui.serveView(w, r)
	case strings.HasPrefix(r.URL.Path, "/mod"):
		wui.serveMod(w, r)
	case r.URL.Path == "/":
		wui.serveMain(w, r)
	default:
		http.NotFound(w, r)
	}
}

func (wui *WebUI) serveMain(w http.ResponseWriter, r *http.Request) {
	v := struct {
		Title string
		V     [][]View
	}{
		Title: "main",
		V:     [][]View{wui.v0, wui.v1},
	}
	b, err := wui.genTemplate("main.html", &v)
	if err != nil {
		fmt.Fprintf(w, "%v", err)
		return
	}
	w.Write(b)
}

func (wui *WebUI) serveMod(w http.ResponseWriter, r *http.Request) {
	if r.Method != "GET" {
		return
	}
	err := r.ParseForm()
	if err != nil {
		fmt.Fprintf(w, "%v", err)
		return
	}
	p := r.FormValue("path")

	p0, err := fullPath(filepath.Join(flag.Arg(0), p))
	if err != nil {
		fmt.Fprintf(w, "%v", err)
		return
	}
	p1, err := fullPath(filepath.Join(flag.Arg(1), p))
	if err != nil {
		fmt.Fprintf(w, "%v", err)
		return
	}

	prog := filepath.Join(wui.webroot, "diff2html")
	cmd := exec.Command(prog, p0, p1)
	b, _ := cmd.CombinedOutput()
	w.Write(b)
}

func (wui *WebUI) serveView(w http.ResponseWriter, r *http.Request) {
	if r.Method != "GET" {
		return
	}
	err := r.ParseForm()
	if err != nil {
		fmt.Fprintf(w, "%v", err)
		return
	}

	p, err := fullPath(r.FormValue("path"))
	if err != nil {
		fmt.Fprintf(w, "%v", err)
		return
	}

	f, err := os.Open(p)
	if err != nil {
		fmt.Fprintf(w, "%v", err)
		return
	}

	fi, err := f.Stat()
	if err != nil {
		fmt.Fprintf(w, "%v", err)
		return
	}

	if !fi.IsDir() {
		b, err := io.ReadAll(f)
		if err != nil {
			fmt.Fprintf(w, "%v", err)
			return
		}
		t := http.DetectContentType(b)
		t = strings.Replace(t, "text/html", "text/plain", -1)
		w.Header().Set("Content-Type", t)
		w.Write(b)
	} else {
		fis, err := f.Readdir(0)
		if err != nil {
			fmt.Fprintf(w, "%v", err)
			return
		}
		sort.Sort(fileInfoSlice(fis))

		v := struct {
			Title string
			Dir   string
			Path  string
			Info  []os.FileInfo
		}{
			Title: "viewdir",
			Dir:   filepath.Dir(p),
			Path:  p,
			Info:  fis,
		}

		b, err := wui.genTemplate("viewdir.html", &v)
		if err != nil {
			fmt.Fprintf(w, "%v", err)
			return
		}

		w.Write(b)
	}

}

func (wui *WebUI) genTemplate(name string, v interface{}) (w []byte, err error) {
	var b bytes.Buffer

	t, err := wui.tmpls.Clone()
	if err != nil {
		return
	}

	t.ExecuteTemplate(&b, name, v)

	t, err = wui.tmpls.Clone()
	if err != nil {
		return
	}
	s := fmt.Sprintf(`{{define "content"}} %v {{end}}`, b.String())
	_, err = t.Parse(s)
	if err != nil {
		return
	}

	b.Reset()
	t.ExecuteTemplate(&b, "base.html", v)
	w = b.Bytes()

	return
}
