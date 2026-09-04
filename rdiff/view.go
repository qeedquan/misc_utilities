package main

import (
	"bufio"
	"fmt"
	"os"
	"path/filepath"
	"sort"
	"strings"
)

// this is flattened view of the directory tree
// made it easy to output
type View struct {
	Branch string
	Name   string
	Tag    string
	Files  []File
	Dirs   []File
	Mods   []File
	Errs   []error
}

type viewSlice []View

func (v viewSlice) Len() int      { return len(v) }
func (v viewSlice) Swap(i, j int) { v[i], v[j] = v[j], v[i] }
func (v viewSlice) Less(i, j int) bool {
	a := filepath.Join(v[i].Branch, v[i].Name)
	b := filepath.Join(v[j].Branch, v[j].Name)
	return a < b
}

func (v *View) Empty() bool {
	if len(v.Files) > 0 {
		return false
	}
	if len(v.Dirs) > 0 {
		return false
	}
	if len(v.Mods) > 0 {
		return false
	}
	if len(v.Errs) > 0 {
		return false
	}
	return true
}

func BuildView(d0, d1 *Dir, pat, exfiles, exdirs regex) (v0, v1 []View) {
	b0, b1 := shortBase(d0.Path, d1.Path)
	p := View{Branch: b0, Tag: "Top Level"}
	q := View{Branch: b1, Tag: "Top Level"}
	nm0, mm0 := walkView(&p, d0, pat)
	nm1, mm1 := walkView(&q, d1, pat)
	if !p.Empty() {
		v0 = append(v0, p)
	}
	if !q.Empty() {
		v1 = append(v1, q)
	}

	b := []string{b0, b1}
	nm := [][]Dir{nm0, nm1}
	mm := [][]Dir{mm0, mm1}
	v := [][]View{v0, v1}
	for i := range nm {
		for j := range nm[i] {
			t := &nm[i][j]
			p := View{Branch: b[i], Name: t.Rel, Tag: "New"}
			genViewDir(&p, t, exfiles, exdirs)
			v[i] = append(v[i], p)
		}
	}

	for i := range mm {
		for j := range mm[i] {
			t := &mm[i][j]
			p := View{Branch: b[i], Name: t.Rel, Tag: "Modified"}
			walkView(&p, t, regex{})
			v[i] = append(v[i], p)
		}
	}

	v0, v1 = v[0], v[1]
	sort.Sort(viewSlice(v0))
	sort.Sort(viewSlice(v1))
	return
}

func OutputView(v0, v1 []View) {
	b := bufio.NewWriter(os.Stdout)
	defer b.Flush()

	sep := strings.Repeat("*", 20)
	for _, v := range [][]View{v0, v1} {
		for _, p := range v {
			fmt.Fprintf(b, "%v\n", sep)
			fmt.Fprintf(b, "Module %v/%v (%v):\n", p.Branch, p.Name, p.Tag)
			fmt.Fprintf(b, "%v\n\n\n", sep)
			for _, q := range p.Errs {
				fmt.Fprintln(b, "Errors:", q)
			}
			for _, q := range p.Files {
				fmt.Fprintln(b, "Added file:", q.Rel)
			}
			for _, q := range p.Dirs {
				fmt.Fprintln(b, "Added directory:", q.Rel)
			}
			for _, q := range p.Mods {
				s := revision(&q)
				fmt.Fprintf(b, "Modified file: %v (%v)\n", q.Rel, s)
			}

			fmt.Fprintf(b, "\n\n")
		}
	}
}

func revision(f *File) string {
	s := ""
	switch {
	case f.RevErr != nil:
		s = f.RevErr.Error()
	case f.Rev[0] < f.Rev[1]:
		s = fmt.Sprintf("file revision is older: (%v, %v)", f.Rev[0], f.Rev[1])
	case f.Rev[0] == f.Rev[1]:
		s = fmt.Sprintf("file revision is the same: (%v, %v)", f.Rev[0], f.Rev[1])
	default:
		s = fmt.Sprintf("file revision is newer: (%v, %v)", f.Rev[0], f.Rev[1])
	}
	return s
}

func walkView(v *View, d *Dir, pat regex) (nm, mm []Dir) {
	for _, p := range d.Errs {
		v.Errs = append(v.Errs, p)
	}
	for _, p := range d.Files {
		v.Files = append(v.Files, p)
	}
	for _, p := range d.Mods {
		v.Mods = append(v.Mods, p)
	}
	for _, p := range d.Dirs {
		if pat.MatchString(p.Rel) >= 0 {
			nm = append(nm, p)
			continue
		}
		v.Dirs = append(v.Dirs, p.File)
	}
	for _, p := range d.Mdirs {
		if pat.MatchString(p.Rel) >= 0 {
			mm = append(mm, p)
			continue
		}
		x, y := walkView(v, &p, pat)
		nm = append(nm, x...)
		mm = append(mm, y...)
	}
	return
}

func genViewDir(v *View, d *Dir, exfiles, exdirs regex) {
	_, fis, dis, err := readDir(d.Path, d.Rel, exfiles, exdirs)
	if err != nil {
		v.Errs = append(v.Errs, err)
	}
	v.Files = append(v.Files, fis...)
	v.Dirs = append(v.Dirs, dis...)

	for i := range dis {
		d = &Dir{File: dis[i]}
		genViewDir(v, d, exfiles, exdirs)
	}
}

func shortBase(a, b string) (string, string) {
	a = filepath.Clean(a)
	b = filepath.Clean(b)

	if a == b {
		return filepath.Base(a), filepath.Base(b)
	}

	na, nb := "", ""
	for {
		d0, b0 := filepath.Split(a)
		d1, b1 := filepath.Split(b)

		na = filepath.Join(b0, na)
		nb = filepath.Join(b1, nb)

		if na != nb {
			break
		}

		a, b = filepath.Clean(d0), filepath.Clean(d1)
	}
	return na, nb
}
