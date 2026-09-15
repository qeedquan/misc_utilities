// +build linux

// ported from toybox
package main

import (
	"bufio"
	"flag"
	"fmt"
	"os"
	"path/filepath"
	"strconv"
)

var (
	xflag = flag.Bool("x", false, "show the extended format")
	qflag = flag.Bool("q", false, "do not display some header/footer lines")
)

func main() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}
	for _, arg := range flag.Args() {
		ek(pmap(arg))
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: pmap [-xq] [pids ...]")
	flag.PrintDefaults()
	os.Exit(2)
}

func ek(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "pmap:", err)
		os.Exit(1)
	}
}

func pmap(fn string) error {
	pid, err := strconv.Atoi(fn)
	if err != nil {
		return err
	}

	fn = fmt.Sprintf("/proc/%d/cmdline", pid)
	buf, err := os.ReadFile(fn)
	if err != nil {
		return fmt.Errorf("no %d", pid)
	}
	fmt.Printf("%s\n", buf)

	if *xflag {
		fn = fmt.Sprintf("/proc/%d/smaps", pid)
	} else {
		fn = fmt.Sprintf("/proc/%d/maps", pid)
	}

	f, err := os.Open(fn)
	if err != nil {
		return err
	}
	defer f.Close()

	if *xflag && !*qflag {
		fmt.Printf("%-16s   Kbytes     PSS   Dirty    Swap  Mode  Mapping\n", "Address")
	}

	var total, tdirty, tpss, tswap uint64
	var pss, dirty, swap uint64
	var start, end, off uint64
	var name, oldLine, maps, k string
	if *xflag {
		k = "K"
	}

	x := 0
	s := bufio.NewScanner(f)
	for s.Scan() {
		line := s.Text()

		count, _ := fmt.Sscanf(line, "%x-%x %s %*s %*s %d", &start, &end, &maps, &off)
		if count == 3 {
			name = line
			if off >= uint64(len(line)) {
				name = " [anon]\n"
			}
			if len(maps) >= 4 && maps[3] == 'p' {
				maps = maps[:3] + "-" + maps[4:]
			}
			end = (end - start) / 1024
			total += end
			fmt.Printf("%016x %7d%s ", start, end, k)
			if *xflag {
				oldLine = line
				continue
			}
		} else {
			n1, _ := fmt.Sscanf(line, "Pss: %d", &pss)
			n2, _ := fmt.Sscanf(line, "Private_Dirty: %d", &dirty)
			n3, _ := fmt.Sscanf(line, "Swap: %d", &swap)
			if n1 > 0 || n2 > 0 || n3 > 0 {
				x++
			}
			if x < 3 {
				continue
			}
			x = 0

			line = oldLine
			name = filepath.Base(name)
			fmt.Printf("%7d %7d %7d ", pss, dirty, swap)
			tpss += pss
			tdirty += dirty
			tswap += swap
		}

		fmt.Printf("%s- %s\n", maps, name)
		line = ""
	}

	if !*qflag {
		if *xflag {
			fmt.Printf("------ ------ ------ ------\n")
		}
		fmt.Printf("total %d%s", total, k)
		if *xflag {
			fmt.Printf(" %d %d %d", tpss, tdirty, tswap)
		}
		fmt.Printf("\n")
	}

	return nil
}
