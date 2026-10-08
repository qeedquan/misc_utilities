package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"regexp"
	"runtime/pprof"
)

var exFiles = []string{
	"^.*Makefile.log$",
	"^.*\\.(so|o|O)$",
	"^.*svn-commit\\.",
	"^.*~$",
}

var exDirs = []string{
	"^.*\\.(svn|git|hg)$",
	"^.*(build|release)$",
}

var modules = []string{
	"^.*lts/.*$",
	"^.*fdc/.*$",
	"^.*tools/.*$",
}

type regex []*regexp.Regexp

func (regex) String() string { return "regular expression" }
func (p *regex) Set(s string) error {
	r, err := regexp.Compile(s)
	if err != nil {
		return err
	}
	*p = append(*p, r)
	return nil
}

func (p regex) MatchString(s string) int {
	for i := range p {
		if p[i].MatchString(s) {
			return i
		}
	}
	return -1
}

func main() {
	var exfiles, exdirs regex
	var modpat regex
	var exbuiltin, webui bool
	var cpuprofile string
	var webaddr, webroot string

	log.SetOutput(os.Stderr)

	flag.BoolVar(&exbuiltin, "exclude-builtin", false, "exclude builtin patterns when matching")
	flag.Var(&exfiles, "exclude-file", "exclude files that match a pattern")
	flag.Var(&exdirs, "exclude-dir", "exclude dirs that match a pattern")
	flag.Var(&modpat, "module-pattern", "pattern to say which directory is a module")
	flag.StringVar(&cpuprofile, "cpu-profile", "", "write cpu profile to a file")
	flag.StringVar(&webroot, "webroot", "webroot", "root web directory to use")
	flag.StringVar(&webaddr, "webaddr", "127.0.0.1:8080", "address to run the web-ui on")
	flag.BoolVar(&webui, "webui", false, "runs the web-ui after outputting to stdout")
	flag.Parse()
	if flag.NArg() != 2 {
		usage()
	}

	if !exbuiltin {
		for _, s := range exFiles {
			exfiles = append(exfiles, regexp.MustCompile(s))
		}
		for _, s := range exDirs {
			exdirs = append(exdirs, regexp.MustCompile(s))
		}
		for _, s := range modules {
			modpat = append(modpat, regexp.MustCompile(s))
		}
	}

	if cpuprofile != "" {
		f, err := os.Create(cpuprofile)
		if err != nil {
			log.Fatalln(err)
		}
		pprof.StartCPUProfile(f)
		defer pprof.StopCPUProfile()
	}

	a0, err := fullPath(flag.Arg(0))
	if err != nil {
		log.Fatalln(err)
	}
	a1, err := fullPath(flag.Arg(1))
	if err != nil {
		log.Fatalln(err)
	}
	d0, d1, err := Diff(a0, a1, exfiles, exdirs)
	if err != nil {
		log.Fatalln(err)
	}

	v0, v1 := BuildView(d0, d1, modpat, exfiles, exdirs)
	OutputView(v0, v1)
	if webui {
		RunUI(webaddr, webroot, v0, v1)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] dir1 dir2")
	flag.PrintDefaults()
	os.Exit(1)
}
