// +build linux
// ported from suckless

package main

import (
	"flag"
	"fmt"
	"os"
	"syscall"
)

var (
	unit  uint64
	shift uint64
)

func main() {
	flag.Var(unitFlag('b'), "b", "display in bytes")
	flag.Var(unitFlag('k'), "k", "display in kibibytes")
	flag.Var(unitFlag('m'), "m", "display in megabytes")
	flag.Var(unitFlag('g'), "g", "display in gigabytes")
	flag.Usage = usage
	flag.Parse()

	var info syscall.Sysinfo_t
	err := syscall.Sysinfo(&info)
	ck(err)

	unit = uint64(info.Unit)
	if unit == 0 {
		unit = 1
	}

	fmt.Printf("     %13s%13s%13s%13s%13s\n",
		"total", "used", "free", "shared", "buffers")
	fmt.Printf("Mem: ")

	fmt.Printf("%13d%13d%13d%13d%13d\n",
		scale(info.Totalram),
		scale(info.Totalram-info.Freeram),
		scale(info.Freeram),
		scale(info.Sharedram),
		scale(info.Bufferram))
	fmt.Printf("-/+ buffers/cache:")
	fmt.Printf("%13d%13d\n",
		scale(info.Totalram-info.Freeram-info.Bufferram),
		scale(info.Freeram+info.Bufferram))
	fmt.Printf("Swap:")
	fmt.Printf("%13d%13d%13d\n",
		scale(info.Totalswap),
		scale(info.Totalswap-info.Freeswap),
		scale(info.Freeswap))
}

func ck(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr)
	}
}

func scale(v uint64) uint64 {
	return (v * unit) >> shift
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options]")
	flag.PrintDefaults()
	os.Exit(2)
}

type unitFlag rune

func (unitFlag) IsBoolFlag() bool { return true }
func (unitFlag) String() string   { return "" }
func (u unitFlag) Set(string) error {
	switch u {
	case 'b':
		shift = 0
	case 'k':
		shift = 10
	case 'm':
		shift = 20
	case 'g':
		shift = 30
	}
	return nil
}
