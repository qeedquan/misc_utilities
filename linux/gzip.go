package main

import (
	"compress/flate"
	"compress/gzip"
	"flag"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"strings"
)

var (
	level = flag.Int("l", flate.DefaultCompression, "compression level")
	dec   = flag.Bool("d", false, "decompress")
	force = flag.Bool("f", false, "force")

	status = 0
)

func main() {
	flag.Usage = usage
	flag.Parse()

	if flag.NArg() < 1 {
		if !*force {
			fmt.Fprintln(os.Stderr, "gzip: compressed data not written to terminal")
			fmt.Fprintln(os.Stderr, "For help, use -h")
			os.Exit(1)
		}
		r, w, err := newstd()
		ck(err)
		ck(xcopy(w, r))
	} else {
		for _, name := range flag.Args() {
			r, w, err := newio(name)
			if err == os.ErrExist {
				continue
			}
			if ek(err) {
				continue
			}
			if !ek(xcopy(w, r)) {
				os.Remove(name)
			}
		}
	}
	os.Exit(status)
}

func ck(err error) {
	if ek(err) {
		os.Exit(status)
	}
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "gzip", err)
		status |= 1
		return true
	}
	return false
}

func pe(errs ...error) error {
	for _, err := range errs {
		if err != nil {
			return err
		}
	}
	return nil
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] file ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func newstd() (r io.ReadCloser, w io.WriteCloser, err error) {
	defer func() {
		if err != nil {
			if r != nil {
				r.Close()
			}
			if w != nil {
				w.Close()
			}
		}
	}()

	if *dec {
		r, err = gzip.NewReader(os.Stdin)
		w = os.Stdout
	} else {
		r = os.Stdin
		w, err = gzip.NewWriterLevel(os.Stdout, *level)
	}
	return
}

func newio(name string) (r io.ReadCloser, w io.WriteCloser, err error) {
	defer func() {
		if err != nil {
			if r != nil {
				r.Close()
			}
			if w != nil {
				w.Close()
			}
		}
	}()

	err = checkio(name)
	if err != nil {
		return
	}

	r, err = os.Open(name)
	if err != nil {
		return
	}

	if *dec {
		r, err = newReader(r)
		if err != nil {
			return
		}
		ext := filepath.Ext(name)
		w, err = os.Create(name[:len(name)-len(ext)])
	} else {
		w, err = os.Create(name + ".gz")
		if err != nil {
			return
		}
		w, err = newWriter(w)
	}

	return
}

func checkio(name string) error {
	if *force {
		return nil
	}

	ext := strings.ToLower(filepath.Ext(name))
	name = name[:len(name)-len(ext)]

	switch *dec {
	case true:
		if ext != ".gz" || name == "" {
			return fmt.Errorf("unknown suffix -- ignored")
		}
		return checkExist(name)

	case false:
		if ext == ".gz" {
			return fmt.Errorf("already has .gz suffix -- unchanged")
		}
		return checkExist(name + ".gz")
	}
	panic("unreachable")
}

func checkExist(name string) error {
	_, err := os.Stat(name)
	if err == nil {
		var c rune
		fmt.Printf("%s already exists; do you wish to overwrite (y or n)? ", name)
		fmt.Scanf("%c", &c)
		if c == 'y' {
			return nil
		}
		fmt.Println("not overwritten")
		return os.ErrExist
	}
	return nil
}

func xcopy(w io.WriteCloser, r io.ReadCloser) error {
	_, e1 := io.Copy(w, r)
	e2 := r.Close()
	e3 := w.Close()
	return pe(e1, e2, e3)
}

type gzipReader struct {
	*gzip.Reader
	r io.ReadCloser
}

func newReader(r io.ReadCloser) (io.ReadCloser, error) {
	zr, err := gzip.NewReader(r)
	if err != nil {
		return nil, err
	}
	return &gzipReader{zr, r}, nil
}

func (z *gzipReader) Close() error {
	e1 := z.Reader.Close()
	e2 := z.r.Close()
	return pe(e1, e2)
}

type gzipWriter struct {
	*gzip.Writer
	w io.WriteCloser
}

func newWriter(w io.WriteCloser) (io.WriteCloser, error) {
	zw, err := gzip.NewWriterLevel(w, *level)
	if err != nil {
		return nil, err
	}
	return &gzipWriter{zw, w}, nil
}

func (z *gzipWriter) Close() error {
	e1 := z.Writer.Close()
	e2 := z.w.Close()
	return pe(e1, e2)
}
