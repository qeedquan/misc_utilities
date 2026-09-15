// ported from https://intelxed.github.io/ref-manual/group__SMALLEXAMPLES.html

package main

import (
	"flag"
	"fmt"

	"github.com/qeedquan/go-media/debug/xed"
)

var (
	longMode = flag.Bool("l", false, "long mode")
)

func main() {
	flag.Parse()

	itext := [15]byte{0xf, 0x85, 0x99, 0x00, 0x00, 0x00}

	var mmode xed.MachineMode
	var stackAddrWidth xed.AddressWidth
	if *longMode {
		mmode = xed.MACHINE_MODE_LONG_64
		stackAddrWidth = xed.ADDRESS_WIDTH_64b
	} else {
		mmode = xed.MACHINE_MODE_LEGACY_32
		stackAddrWidth = xed.ADDRESS_WIDTH_32b
	}

	// This is a test of error handling. I vary the instuction length from
	// 0 bytes to 15 bytes.  Normally, you should send in 15 bytes of itext
	// unless you are near the end of a page and don't want to take a page
	// fault or tlb miss. Note, you have to reinitialize the xedd each time
	// you try to decode in to it.
	// Try different instruction lengths to see when XED recognizes an
	// instruction as valid.
	for bytes := 0; bytes <= 15; bytes++ {
		var xedd xed.DecodedInst
		xedd.Zero()
		xedd.SetMode(mmode, stackAddrWidth)
		err := xedd.Decode(itext[:bytes])
		fmt.Printf("%d %v\n", bytes, err)
	}
}
