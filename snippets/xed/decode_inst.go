package main

import (
	"encoding/hex"
	"flag"
	"fmt"
	"log"
	"os"
	"strings"

	"github.com/qeedquan/go-media/debug/xed"
)

var (
	mpxMode        = flag.Bool("mpx", false, "mpx mode")
	cetMode        = flag.Bool("cet", false, "cet mode")
	longMode       = flag.Bool("64", false, "long mode")
	realMode       = flag.Bool("16", false, "real mode")
	realAddressing = flag.Bool("s16", false, "real addressing")
	chipset        = flag.String("chip", "", "chipset")
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("decode_inst: ")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	var dstate xed.State
	dstate.Zero()
	dstate.SetMachineMode(xed.MACHINE_MODE_LEGACY_32)
	dstate.SetStackAddrWidth(xed.ADDRESS_WIDTH_32b)
	if *longMode {
		dstate.SetMachineMode(xed.MACHINE_MODE_LONG_64)
	} else if *realMode {
		dstate.SetMachineMode(xed.MACHINE_MODE_LEGACY_16)
		dstate.SetStackAddrWidth(xed.ADDRESS_WIDTH_16b)
	} else if *realAddressing {
		dstate.SetStackAddrWidth(xed.ADDRESS_WIDTH_16b)
	}

	var chip xed.Chip
	if *chipset != "" {
		chip = xed.Str2Chip(*chipset)
		fmt.Printf("Setting chip to %s\n", chip.String())
	}

	var xedd xed.DecodedInst
	xedd.ZeroSetMode(&dstate)
	xedd.SetInputChip(chip)
	if *mpxMode {
		xedd.SetMpxMode(1)
	}
	if *cetMode {
		xedd.SetCetMode(1)
	}

	itext, err := hex.DecodeString(strings.Join(flag.Args(), ""))
	ck(err)

	fmt.Printf("Attempting to decode: % 02x\n", itext)
	err = xedd.Decode(itext)
	ck(err)

	for _, syntax := range []xed.Syntax{xed.SYNTAX_XED, xed.SYNTAX_INTEL, xed.SYNTAX_ATT} {
		str, err := xed.FormatContext(syntax, &xedd, 0, nil)
		if err == nil {
			fmt.Printf("syntax %s: %s\n", syntax, str)
		}
	}
	fmt.Println(xedd.Dump())

	for i := uint(0); i < xedd.Length(); i++ {
		fmt.Printf("%02x ", xedd.Byte(i))
	}
	fmt.Println()

	fmt.Printf("iclass %s\t", xedd.IClass())
	fmt.Printf("category %s\t", xedd.Category())
	fmt.Printf("ISA-extension %s\t", xedd.Extension())
	fmt.Printf("ISA-set %s\n", xedd.ISASet())
	fmt.Printf("instruction-length %d\n", xedd.Length())
	fmt.Printf("operand-width %d\n", xedd.OperandWidth())
	fmt.Printf("effective-operand-width %d\n", xedd.OperandsConst().EffectiveOperandWidth())
	fmt.Printf("effective-address-width %d\n", xedd.OperandsConst().EffectiveAddressWidth())
	fmt.Printf("stack-address-width %d\n", xedd.OperandsConst().StackAddressWidth())
	fmt.Printf("iform-enum-name %s\n", xedd.Iform())
	fmt.Printf("iform-enum-name-dispatch (zero based) %d\n", xedd.IformDispatch())
	fmt.Printf("iclass-max-iform-dispatch %d\n", xedd.IClass().IformMaxPerIclass())

	printOperands(&xedd)

	printMemOps(&xedd)

	printFlags(&xedd)
	printReadsZfFlag(&xedd)

	printAttributes(&xedd)

	printMisc(&xedd)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] opcode")
	flag.PrintDefaults()
	os.Exit(2)
}

func printReadsZfFlag(xedd *xed.DecodedInst) {
	if xedd.UsesRflags() {
		rfi := xedd.RflagsInfo()
		readset := rfi.ReadFlagSet()
		if readset.Flat()&xed.FLAGSET_ZF != 0 {
			fmt.Printf("READS ZF\n")
		}
	}
}

func printFlags(xedd *xed.DecodedInst) {
	if xedd.UsesRflags() {
		rfi := xedd.RflagsInfo()
		fmt.Printf("FLAGS:\n")
		if rfi.ReadsFlags() {
			fmt.Printf("   reads-rflags ")
		} else if rfi.WritesFlags() {
			if rfi.MayWrite() {
				fmt.Printf("  may-write-rflags ")
			}
			if rfi.MustWrite() {
				fmt.Printf("  must-write-rflags ")
			}
		}
		nflags := rfi.NumFlags()
		for i := uint(0); i < nflags; i++ {
			fa := rfi.FlagAction(i)
			fmt.Printf("%s ", fa)
		}
		fmt.Printf("\n")

		readset := rfi.ReadFlagSet()
		writtenset := rfi.WrittenFlagSet()
		undefinedset := rfi.UndefinedFlagSet()
		fmt.Printf("       read: %30s mask=0x%x\n", readset, readset.Mask())
		fmt.Printf("    written: %30s mask=0x%x\n", writtenset, writtenset.Mask())
		fmt.Printf("  undefined: %30s mask=0x%x\n", undefinedset, undefinedset.Mask())

	}
}

func printOperands(xedd *xed.DecodedInst) {
	xi := xedd.Inst()
	noperands := xi.NumOperands()

	fmt.Printf("Operands\n")
	fmt.Printf("#   TYPE               DETAILS        VIS  RW       OC2 BITS BYTES NELEM ELEMSZ   ELEMTYPE   REGCLASS\n")
	fmt.Printf("#   ====               =======        ===  ==       === ==== ===== ===== ======   ========   ========\n")
	for i := uint(0); i < noperands; i++ {
		op := xi.Operand(i)
		opname := op.Name()
		fmt.Printf("%d %6s ", i, opname)

		var tbuf string
		switch opname {
		case xed.OPERAND_AGEN, xed.OPERAND_MEM0, xed.OPERAND_MEM1:
			tbuf = "(see below)"
		case xed.OPERAND_PTR, xed.OPERAND_RELBR:
			dispbits := xedd.BranchDisplacementWidth()
			if dispbits != 0 {
				disp := xedd.BranchDisplacement()
				tbuf = fmt.Sprintf("BRANCH_DISPLACEMENT_BYTES=%d %08x", dispbits, disp)
			}
		case xed.OPERAND_IMM0:
			var buf string

			ibits := xedd.ImmediateWidthBits()
			if xedd.ImmediateIsSigned() != 0 {
				rbits := ibits
				if rbits == 0 {
					rbits = 8
				}
				x := xedd.SignedImmediate()
				y := xed.SignExtendArbitraryTo64(uint64(x), ibits)
				buf = fmt.Sprintf("%x", y)
			} else {
				x := xedd.UnsignedImmediate()
				rbits := ibits
				if rbits == 0 {
					rbits = 16
				}
				buf = fmt.Sprintf("%x", x)
			}
			tbuf = fmt.Sprintf("0x%s(%db)", buf, ibits)

		case xed.OPERAND_IMM1:
			x := xedd.SecondImmediate()
			tbuf = fmt.Sprintf("0x%02x", x)

		case xed.OPERAND_REG0, xed.OPERAND_REG1, xed.OPERAND_REG2, xed.OPERAND_REG3,
			xed.OPERAND_REG4, xed.OPERAND_REG5, xed.OPERAND_REG6, xed.OPERAND_REG7,
			xed.OPERAND_REG8, xed.OPERAND_BASE0, xed.OPERAND_BASE1:
			r := xedd.Reg(opname)
			tbuf = fmt.Sprintf("%s=%s", opname, r)

		default:
			fmt.Printf("need to add support for printing operand: %s", opname)
		}

		fmt.Printf("%21s", tbuf)

		rw := xedd.OperandAction(i)
		fmt.Printf(" %10s %3s %9s", op.Visibility(), rw, op.Width())

		bits := xedd.OperandLengthBits(i)
		fmt.Printf("  %3d", bits)
		fmt.Printf("  %4d", (bits+7)>>3)

		fmt.Printf("    %2d", xedd.OperandElements(i))
		fmt.Printf("    %3d", xedd.OperandElementSizeBits(i))

		fmt.Printf(" %10s", xedd.OperandElementType(i))
		fmt.Printf(" %10s\n", xedd.Reg(opname).Class())
	}
}

func printAttributes(xedd *xed.DecodedInst) {
	xi := xedd.Inst()
	numAttributes := xed.AttributeMax()

	fmt.Printf("ATTRIBUTES: ")
	for i := uint(0); i < numAttributes; i++ {
		attr := xed.AttributeEnum(i)
		if xi.Attribute(attr) != 0 {
			fmt.Printf("%s ", attr)
		}
	}
	fmt.Printf("\n")
}

func printMisc(xedd *xed.DecodedInst) {
	ov := xedd.OperandsConst()
	xi := xedd.Inst()
	e := xi.Exception()
	np := xedd.NumPrefixes()
	isaset := xedd.ISASet()
	if ov.HasRealRep() {
		norep := xed.RepRemove(xedd.IClass())
		fmt.Printf("REAL REP ")
		fmt.Printf("corresponding no-rep iclass: %s\n", norep)
	}
	if ov.HasRepPrefix() {
		fmt.Printf("F3 PREFIX\n")
	}
	if ov.HasRepnePrefix() {
		fmt.Printf("F2 PREFIX\n")
	}
	if ov.HasAddressSizePrefix() {
		fmt.Printf("67 PREFIX\n")
	}
	if ov.HasOperandSizePrefix() {
		/* this 66 prefix is not part of the opcode */
		fmt.Printf("66-OSZ PREFIX\n")
	}
	if ov.Has66Prefix() {
		// this is any 66 prefix including the above
		fmt.Printf("ANY 66 PREFIX\n")
	}
	if xedd.Attribute(xed.ATTRIBUTE_RING0) != 0 {
		fmt.Printf("RING0 only\n")
	}
	if e != xed.EXCEPTION_INVALID {
		fmt.Printf("EXCEPTION TYPE: %s\n", e)
	}
	if xedd.IsBroadcast() {
		fmt.Printf("BROADCAST\n")
	}

	if xedd.ClassifySSE() || xedd.ClassifyAVX() || xedd.ClassifyAVX512() {
		if xedd.ClassifyAVX512MaskOp() {
			fmt.Printf("AVX512 KMASK-OP\n")
		} else {
			sse := false
			if xedd.ClassifySSE() {
				fmt.Printf("SSE\n")
			}
			if xedd.ClassifyAVX() {
				fmt.Printf("AVX\n")
			}
			if xedd.ClassifyAVX512() {
				fmt.Printf("AVX512\n")
			}
			if xedd.Attribute(xed.ATTRIBUTE_SIMD_SCALAR) != 0 {
				fmt.Printf("SCALAR\n")
			} else {
				vlbits := uint(128)
				if sse {
					// xed_decoded_inst_vector_length_bits is only for VEX/EVEX instr.
					// This will print 128 vl for FXSAVE and LD/ST MXCSR which is unfortunate.
					vlbits = xedd.VectorLengthBits()
					fmt.Printf("Vector length: %d\n", vlbits)
				}
			}
		}
	}
	// does not include instructions that have XED_ATTRIBUTE_MASK_AS_CONTROL.
	// does not include vetor instructions that have k0 as a mask register.
	if xedd.MaskedVectorOperation() {
		fmt.Printf("WRITE-MASKING\n")
	}

	if np != 0 {
		fmt.Printf("Number of legacy prefixes: %d\n", np)
	}
	fmt.Printf("ISA SET: [%s]\n", isaset)
	for i := uint(0); i < xed.MAX_CPUID_BITS_PER_ISA_SET; i++ {
		cpuidbit := isaset.CPUIDBit(i)
		if cpuidbit == xed.CPUID_BIT_INVALID {
			break
		}
		fmt.Printf("%d\tCPUID BIT NAME: [%s]\n", i, cpuidbit)
		var crec xed.CPUIDRec
		r := xed.GetCPUIDRec(cpuidbit, &crec)
		if r {
			fmt.Printf("\tLeaf 0x%08x, subleaf 0x%08x, %s[%u]\n",
				crec.Leaf(), crec.Subleaf(), crec.Reg(), crec.Bit())
		} else {
			fmt.Printf("Could not find cpuid leaf information\n")
		}
	}
}

func printMemOps(xedd *xed.DecodedInst) {
	fmt.Printf("Memory Operands\n")

	memops := xedd.NumberOfMemoryOperands()
	for i := uint(0); i < memops; i++ {
		fmt.Printf("  %d ", i)

		r_or_w := false
		if xedd.MemRead(i) {
			fmt.Printf("   read ")
			r_or_w = true
		}
		if xedd.MemWritten(i) {
			fmt.Printf("written ")
			r_or_w = true
		}
		if !r_or_w {
			fmt.Printf("   agen ") // LEA instructions
		}
		seg := xedd.SegReg(i)
		if seg != xed.REG_INVALID {
			fmt.Printf("SEG= %s ", seg)
		}
		base := xedd.BaseReg(i)
		if base != xed.REG_INVALID {
			fmt.Printf("BASE= %3s/%3s ", base, base.RegClass())
		}
		indx := xedd.IndexReg(i)
		if i == 0 && indx != xed.REG_INVALID {
			fmt.Printf("INDEX= %3s/%3s ", indx, indx.RegClass())
			if xedd.Scale(i) != 0 {
				// only have a scale if the index exists.
				fmt.Printf("SCALE= %u ", xedd.Scale(i))
			}
		}

		ov := xedd.OperandsConst()
		if ov.HasMemoryDisplacement() {
			disp_bits := xedd.MemoryDisplacementWidth(i)
			if disp_bits != 0 {
				fmt.Printf("DISPLACEMENT_BYTES= %d ", disp_bits)
				disp := xedd.MemoryDisplacement(i)
				fmt.Printf("0x%x base10=%d", disp, disp)
			}
		}
		fmt.Printf(" ASZ%d=%d\n", i, xedd.MemOpAddressWidth(i))
	}
	fmt.Printf("  MemopBytes = %d\n", xedd.MemoryOperandLength(0))
}
