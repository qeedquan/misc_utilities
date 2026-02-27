package main

import (
	"bytes"
	"fmt"
	"log"
	"os"
	"os/exec"
	"path/filepath"
	"sort"
)

type File struct {
	Name   string
	Path   string
	Rel    string
	RevErr error
	Rev    [2]int
}

type Dir struct {
	File
	Files []File
	Dirs  []Dir
	Mods  []File
	Mdirs []Dir
	Errs  []error
}

type fileInfoSlice []os.FileInfo

func (f fileInfoSlice) Len() int           { return len(f) }
func (f fileInfoSlice) Swap(i, j int)      { f[i], f[j] = f[j], f[i] }
func (f fileInfoSlice) Less(i, j int) bool { return f[i].Name() < f[j].Name() }

func Diff(dir0, dir1 string, exfiles, exdirs regex) (d0, d1 *Dir, err error) {
	log.Println("starting diff, this could take a while...")

	fi0, f0, fd0, err := readDir(dir0, "", exfiles, exdirs)
	if err != nil {
		return
	}

	fi1, f1, fd1, err := readDir(dir1, "", exfiles, exdirs)
	if err != nil {
		return
	}

	d0 = &Dir{File: fi0}
	d1 = &Dir{File: fi1}
	diff(d0, d1, dir0, dir1, "", "", f0, f1, fd0, fd1, exfiles, exdirs)
	log.Println("finished diff")
	return
}

func diff(d0, d1 *Dir, p0, p1, r0, r1 string, f0, f1, fd0, fd1 []File, exfiles, exdirs regex) {
	x, y := f0, f1
	for len(x) > 0 && len(y) > 0 {
		switch a, b := x[0].Name, y[0].Name; {
		case a < b:
			d0.Files = append(d0.Files, x[0])
			x = x[1:]
		case a == b:
			cmd := exec.Command("diff", x[0].Path, y[0].Path)
			err := cmd.Run()
			if err != nil {
				p, q := x[0], y[0]
				rv, err := svnVersion(p.Path, q.Path)
				p.Rev[0], p.Rev[1] = rv[0], rv[1]
				q.Rev[0], q.Rev[1] = rv[1], rv[0]
				p.RevErr, q.RevErr = err, err

				d0.Mods = append(d0.Mods, p)
				d1.Mods = append(d1.Mods, q)
			}
			x, y = x[1:], y[1:]
		default:
			d1.Files = append(d1.Files, y[0])
			y = y[1:]
		}
	}
	d0.Files = append(d0.Files, x...)
	d1.Files = append(d1.Files, y...)

	x, y = fd0, fd1
	for len(x) > 0 && len(y) > 0 {
		switch a, b := x[0].Name, y[0].Name; {
		case a < b:
			d0.Dirs = append(d0.Dirs, Dir{File: x[0]})
			x = x[1:]
		case a == b:
			var nd0, nd1 Dir

			p0 := filepath.Join(p0, a)
			r0 := filepath.Join(r0, a)
			p1 := filepath.Join(p1, b)
			r1 := filepath.Join(r1, b)

			i0, f0, fd0, err := readDir(p0, r0, exfiles, exdirs)
			if err != nil {
				nd0.Errs = append(nd0.Errs, err)
			}
			i1, f1, fd1, err := readDir(p1, r1, exfiles, exdirs)
			if err != nil {
				nd1.Errs = append(nd1.Errs, err)
			}
			nd0.File = i0
			nd1.File = i1

			diff(&nd0, &nd1, p0, p1, r0, r1, f0, f1, fd0, fd1, exfiles, exdirs)
			if !emptyDir(&nd0, &nd1) {
				d0.Mdirs = append(d0.Mdirs, nd0)
				d1.Mdirs = append(d1.Mdirs, nd1)
			}
			x, y = x[1:], y[1:]
		default:
			d1.Dirs = append(d1.Dirs, Dir{File: y[0]})
			y = y[1:]
		}
	}
	for i := range x {
		d0.Dirs = append(d0.Dirs, Dir{File: x[i]})
	}
	for i := range y {
		d1.Dirs = append(d1.Dirs, Dir{File: y[i]})
	}

	return
}

func emptyDir(d0, d1 *Dir) bool {
	switch {
	case len(d0.Errs) > 0 || len(d1.Errs) > 0:
		return false
	case len(d0.Files) > 0 || len(d1.Files) > 0:
		return false
	case len(d0.Dirs) > 0 || len(d1.Dirs) > 0:
		return false
	case len(d0.Mods) > 0 || len(d1.Mods) > 0:
		return false
	}
	for i := range d0.Mdirs {
		if !emptyDir(&d0.Mdirs[i], &d1.Mdirs[i]) {
			return false
		}
	}
	return true
}

func readDir(path, rel string, exfiles, exdirs regex) (i File, fis, dis []File, err error) {
	f, err := os.Open(path)
	if err != nil {
		return
	}
	defer f.Close()

	fi, err := f.Stat()
	if err != nil {
		return
	}
	if !fi.IsDir() {
		err = fmt.Errorf("%v: not a dir", fi.Name())
		return
	}
	i = File{
		Name: fi.Name(),
		Path: path,
		Rel:  rel,
	}

	l, err := f.Readdir(0)
	if err != nil {
		return
	}

	sort.Sort(fileInfoSlice(l))

	for _, p := range l {
		exc := exfiles
		fd := &fis
		if p.IsDir() {
			exc = exdirs
			fd = &dis
		}

		relname := filepath.Join(rel, p.Name())
		if exc.MatchString(relname) >= 0 {
			continue
		}

		*fd = append(*fd, File{
			Name: p.Name(),
			Path: filepath.Join(path, p.Name()),
			Rel:  relname,
		})
	}

	return
}

func svnVersion(p0, p1 string) (v [2]int, err error) {
	cmd := exec.Command("svn", "status", "-v", p0, p1)
	b, err := cmd.CombinedOutput()
	if err != nil {
		return
	}
	for len(b) > 0 && !('0' <= b[0] && b[0] <= '9') {
		b = b[1:]
	}
	buf := bytes.NewBuffer(b)

	var n, t int
	var s string
	for i := range v {
		n, err = fmt.Fscanf(buf, "%d %d %s %s\n", &t, &v[i], &s, &s)
		if err != nil {
			return
		}
		if n != 4 {
			err = fmt.Errorf("error parsing svn output: %s", buf.Bytes())
			return
		}
	}
	return
}

func fullPath(s string) (p string, err error) {
	if filepath.IsAbs(s) {
		p = s
		return
	}
	dir, err := os.Getwd()
	if err != nil {
		return
	}
	p = filepath.Join(dir, s)
	return
}
