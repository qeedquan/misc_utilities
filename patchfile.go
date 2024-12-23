package main

import (
	"bufio"
	"flag"
	"fmt"
	"os"
	"strings"
)

var (
	status int
)

var flags struct {
	Patches []Patch
}

func main() {
	parseFlags()
	ek(patchFile(flag.Arg(0), flags.Patches))
	os.Exit(status)
}

func parseFlags() {
	var rules, files MultiFlag

	flag.Var(&rules, "r", "add a rule")
	flag.Var(&files, "i", "add a rule file")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	for _, file := range files {
		patches, err := readPatchFile(file)
		if ek(err) {
			continue
		}
		flags.Patches = append(flags.Patches, patches...)
	}
	for _, rule := range rules {
		patch, err := makePatch(rule)
		if ek(err) {
			continue
		}
		flags.Patches = append(flags.Patches, patch)
	}
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "patchfile:", err)
		status |= 1
		return true
	}
	return false
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: patchfile [options] file")
	flag.PrintDefaults()
	os.Exit(2)
}

func readPatchFile(name string) ([]Patch, error) {
	fd, err := os.Open(name)
	if err != nil {
		return nil, err
	}
	defer fd.Close()

	var patches []Patch
	scan := bufio.NewScanner(fd)
	for scan.Scan() {
		line := scan.Text()
		patch, err := makePatch(line)
		if ek(err) {
			continue
		}
		patches = append(patches, patch)
	}
	return patches, nil
}

func makePatch(rule string) (patch Patch, err error) {
	var (
		file string
		data []byte
	)

	start, end, off := -1, -1, -1
	switch {
	case strings.HasPrefix(rule, "file"):
		n, _ := fmt.Sscanf(rule, "file %s %v %v %v", &file, &start, &end, &off)
		if n < 1 {
			err = fmt.Errorf("file rule %q does not have a filename", rule)
			return
		}
		data, err = os.ReadFile(file)
		if err != nil {
			return
		}

	case strings.HasPrefix(rule, "data"):
		toks := strings.Split(rule, " ")
		for toks = toks[1:]; len(toks) > 0; {
			var val int
			n, _ := fmt.Sscanf(toks[0], "%v", &val)
			toks = toks[1:]
			if n != 1 {
				break
			}
			if val == -1 {
				break
			}
			data = append(data, byte(val))
		}
		switch {
		case len(toks) >= 3:
			fmt.Sscanf(toks[2], "%v", &off)
			fallthrough
		case len(toks) >= 2:
			fmt.Sscanf(toks[1], "%v", &end)
			fallthrough
		case len(toks) >= 1:
			fmt.Sscanf(toks[0], "%v", &start)
		}

	default:
		return patch, fmt.Errorf("unknown rule %q", rule)
	}

	if start < 0 || start >= len(data) {
		start = 0
	}
	if end < 0 || end >= len(data) {
		end = len(data)
	}
	if end < start {
		start, end = end, start
	}

	patch = Patch{
		Data: data[start:end],
		Off:  off,
	}
	return
}

func patchFile(name string, patches []Patch) error {
	var maxAddr int
	for _, patch := range patches {
		size := len(patch.Data)
		if patch.Off < 0 || patch.Off == maxAddr {
			maxAddr += size
		} else if patch.Off+size > maxAddr {
			maxAddr = patch.Off + size
		}
	}

	data, err := os.ReadFile(name)
	if err != nil {
		data = make([]byte, maxAddr)
	} else if len(data) < maxAddr {
		data = append(data, make([]byte, maxAddr-len(data))...)
	}

	pos := 0
	for _, patch := range patches {
		if patch.Off < 0 {
			copy(data[pos:], patch.Data)
			pos += len(patch.Data)
		} else {
			copy(data[patch.Off:], patch.Data)
			pos = patch.Off + len(patch.Data)
		}
	}

	return os.WriteFile(name, data, 0644)
}

type Patch struct {
	Off  int
	Data []byte
}

type MultiFlag []string

func (m *MultiFlag) Set(s string) error {
	*m = append(*m, s)
	return nil
}

func (m MultiFlag) String() string {
	return fmt.Sprint([]string(m))
}
