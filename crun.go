package main

import (
	"bytes"
	"flag"
	"fmt"
	"log"
	"math/rand"
	"os"
	"os/exec"
	"os/signal"
	"path/filepath"
	"runtime"
	"strings"
	"syscall"
)

var (
	buildOnly   = flag.Bool("b", false, "build only")
	useDebugger = flag.Bool("d", false, "run in debugger")
	outputFile  = flag.String("o", "", "output file")
	format      = flag.Bool("f", false, "format source file")
	pkgcfg      = flag.String("p", "", "use pkg-config for package flags")
	exflags     = flag.String("x", "", "pass extra flags")
	verbose     = flag.Bool("v", false, "be verbose")
	optimize    = flag.Bool("z", false, "turn optimization on")
	stdc        = flag.String("stdc", "c2y", "standard c version")
	stdcpp      = flag.String("stdcpp", "c++26", "standard cpp version")
	isCpp       = false
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("crun: ")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	files, args := splitArgs(flag.Args())
	if len(files) == 0 {
		log.Fatal("no C source file specified")
	}
	crun(files, args)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] file ... [args]")
	flag.PrintDefaults()
	os.Exit(2)
}

func splitArgs(args []string) ([]string, []string) {
	for i := range args {
		arg := strings.ToLower(args[i])
		if strings.HasSuffix(arg, ".cpp") || strings.HasSuffix(arg, ".cc") || strings.HasSuffix(arg, ".cxx") {
			isCpp = true
		}
		if !strings.HasSuffix(arg, ".c") && !isCpp {
			return args[:i], args[i:]
		}
	}
	return args, nil
}

func crun(files, args []string) {
	cc := os.Getenv("CC")
	if cc == "" {
		cc = "cc"
	}
	if isCpp {
		cc = os.Getenv("CPP")
		if cc == "" {
			cc = "c++"
		}
	}
	sharedFlags := "-Wall -Wextra -pedantic -ggdb -g3 -lm -lpthread -lrt -ldl"
	if *optimize {
		sharedFlags += " -march=native -O2"
	}
	if runtime.GOOS == "linux" && (runtime.GOARCH == "amd64" || runtime.GOARCH == "386") && !*useDebugger && !*optimize {
		sharedFlags += " -fsanitize=address"
	}

	*pkgcfg += os.Getenv("CRUN_PKG_CONFIG")
	if *pkgcfg != "" {
		pw := new(bytes.Buffer)
		cmd := exec.Command("pkg-config", "--cflags", "--libs", *pkgcfg)
		cmd.Stderr = pw
		cmd.Stdout = pw
		err := cmd.Run()
		if err != nil {
			fmt.Fprintf(os.Stderr, "%s\n", pw.Bytes())
			log.Fatal(err)
		}
		sharedFlags += " " + strings.TrimSpace(pw.String())
	}

	cflags := os.Getenv("CFLAGS")
	if cflags == "" {
		cflags = fmt.Sprintf("-std=%v %v", *stdc, sharedFlags)
	}
	if isCpp {
		cflags = os.Getenv("CPPFLAGS")
		if cflags == "" {
			cflags = fmt.Sprintf("-std=%v %v", *stdcpp, sharedFlags)
		}
	}

	binName := tempName(files)
	wd, _ := os.Getwd()

	if *format {
		var fargs []string
		fargs = append(fargs, "-i")
		fargs = append(fargs, files...)
		cmd := exec.Command("clang-format", fargs...)
		cmd.Dir = wd
		cmd.Stdout = os.Stdout
		cmd.Stderr = os.Stderr
		cmd.Stdin = os.Stdin
		err := cmd.Run()
		if err != nil {
			log.Fatal(err)
		}
	}

	if *outputFile != "" {
		binName = *outputFile
		*buildOnly = true
	}

	var cargs []string
	cargs = append(cargs, "-o")
	cargs = append(cargs, binName)
	cargs = append(cargs, files...)
	cargs = append(cargs, strings.Split(cflags, " ")...)
	if *exflags != "" {
		cargs = append(cargs, *exflags)
	}
	if *verbose {
		fmt.Printf("%s %v\n", cc, cargs)
	}

	cmd := exec.Command(cc, cargs...)
	cmd.Dir = wd
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	cmd.Stdin = os.Stdin
	err := cmd.Run()
	if err != nil {
		log.Fatal(err)
	}

	if *buildOnly {
		return
	}

	donech := make(chan error)
	sigch := make(chan os.Signal)
	signal.Notify(sigch)

	exePath := filepath.Join(wd, binName)
	exe := filepath.Join(exePath)
	if *useDebugger {
		dbg := os.Getenv("CRUNDBG")
		if dbg == "" {
			dbg = "gdb --args"
		}

		toks := strings.Split(dbg, " ")
		dbgargs := append(toks[1:], []string{exe}...)
		dbgargs = append(dbgargs, args...)

		args = dbgargs
		exe = toks[0]
	}
	cmd = exec.Command(exe, args...)
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	cmd.Stdin = os.Stdin
	go func() {
		donech <- cmd.Run()
	}()

loop:
	for {
		select {
		case sig := <-sigch:
			if sig != syscall.SIGCHLD && sig != syscall.SIGURG {
				fmt.Printf("signal: %v\n", sig)
				break loop
			}
		case err = <-donech:
			break loop
		}
	}
	os.Remove(exePath)
	if err != nil {
		log.Fatal(err)
	}
}

func tempName(files []string) string {
	name := "crun"
	if len(files) > 0 {
		name = stripExt(files[0])
	}
	return fmt.Sprintf("%s-%d", name, rand.Int())
}

func stripExt(str string) string {
	ext := filepath.Ext(str)
	return str[:len(str)-len(ext)]
}
