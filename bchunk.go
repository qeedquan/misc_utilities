// port from http://he.fi/bchunk/

package main

import (
	"bufio"
	"encoding/binary"
	"flag"
	"fmt"
	"io"
	"log"
	"os"
	"strconv"
	"strings"
)

type Track struct {
	Num       int
	Audio     bool
	Mode      string
	Extension string
	Bstart    int64
	Bsize     int64
	StartSect int64
	StopSect  int64
	Start     int64
	Stop      int64
}

const (
	SECTLEN = 2352

	WAV_RIFF_HLEN   = 12
	WAV_FORMAT_HLEN = 24
	WAV_DATA_HLEN   = 8
	WAV_HEADER_LEN  = WAV_RIFF_HLEN + WAV_FORMAT_HLEN + WAV_DATA_HLEN
)

var (
	raw       = flag.Bool("r", false, "raw mode for MODE2/2352: write all 2352 bytes from offset 0 (VCD/MPEG)")
	verbose   = flag.Bool("v", false, "verbose mode")
	towav     = flag.Bool("w", false, "output audio files in WAV format")
	psxmode   = flag.Bool("p", false, "psx mode for MODE2/2352: write 2336 bytes from offset 24")
	swabaudio = flag.Bool("s", false, "swap byte order in audio tracks")
)

func main() {
	log.SetFlags(0)
	parseFlags()
	convert(flag.Arg(0), flag.Arg(1), flag.Arg(2))
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] <base> <cue> <bin>")
	flag.PrintDefaults()
	os.Exit(2)
}

func parseFlags() {
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 3 {
		usage()
	}
}

func time2frame(str string) int64 {
	var min, sec, frame int64
	n, err := fmt.Sscanf(str, "%d:%d:%d", &min, &sec, &frame)
	if n != 3 || err != nil {
		return -1
	}
	return 75*(min*60+sec) + frame
}

func convert(basefile, binfile, cuefile string) {
	binf, err := os.Open(binfile)
	ck(err)
	defer binf.Close()

	cue, err := os.Open(cuefile)
	ck(err)
	defer cue.Close()

	fmt.Printf("Reading the CUE file:\n")
	scan := bufio.NewScanner(cue)
	scan.Scan()

	var tracks []Track
	var track, prevTrack *Track
	for scan.Scan() {
		line := strings.TrimSpace(scan.Text())

		if i := strings.Index(line, "TRACK"); i >= 0 {
			line = line[i:]

			fmt.Printf("\nTrack ")
			i = strings.IndexRune(line, ' ')
			if i < 0 {
				fmt.Fprintf(os.Stderr, "... ouch, no space after TRACK.\n")
				continue
			}
			line = line[i+1:]

			i = strings.IndexRune(line, ' ')
			if i < 0 {
				fmt.Fprintf(os.Stderr, "... ouch, no space after track number.\n")
				continue
			}

			p := line[:i]
			t := line[i+1:]
			n, _ := strconv.Atoi(p)
			fmt.Printf("%2d: %-12.12s ", n, t)
			tracks = append(tracks, newTrack(n, t))
			prevTrack, track = track, &tracks[len(tracks)-1]
		} else if i := strings.Index(line, "INDEX"); i >= 0 {
			line = line[i:]

			if track == nil {
				fmt.Fprintf(os.Stderr, "... ouch, encountered INDEX before TRACK.\n")
				continue
			}

			i = strings.IndexRune(line, ' ')
			if i < 0 {
				fmt.Fprintf(os.Stderr, "... ouch, no space after INDEX.\n")
				continue
			}
			line = line[i+1:]

			i = strings.IndexRune(line, ' ')
			if i < 0 {
				fmt.Fprintf(os.Stderr, "... ouch, no space after index number.\n")
			}

			p := line[:i]
			t := line[i+1:]
			fmt.Printf(" %s %s", p, t)

			track.StartSect = time2frame(t)
			track.Start = track.StartSect * SECTLEN
			if *verbose {
				fmt.Printf(" (startsect %d ofs %d)", track.StartSect, track.Start)
			}
			if prevTrack != nil && prevTrack.StopSect < 0 {
				prevTrack.StopSect = track.StartSect
				prevTrack.Stop = track.Start - 1
			}
		}
	}

	if track != nil {
		info, err := binf.Stat()
		ck(fmt.Errorf("Failed to stat bin file: %v", err))

		track.Stop = info.Size()
		track.StopSect = track.Stop / SECTLEN
	}

	fmt.Printf("\n\n")

	fmt.Printf("Writing tracks:\n\n")
	for _, track := range tracks {
		writeTrack(binf, track, basefile)
	}
}

func newTrack(num int, mode string) Track {
	var bstart, bsize int64
	var extension string
	var audio bool

	switch mode {
	case "MODE1/2352":
		bstart = 16
		bsize = 2048
		extension = "iso"

	case "MODE2/2352":
		switch {
		case *raw:
			bstart = 0
			bsize = 2352
		case *psxmode:
			bstart = 0
			bsize = 2336
		default:
			bstart = 24
			bsize = 2048
		}
		extension = "iso"

	case "MODE2/2336":
		// was 2352 in V1.361B still work?
		// what if MODE2/2336 single track bin, still 2352 sectors?
		bstart = 16
		bsize = 2336
		extension = "iso"

	case "AUDIO":
		bsize = 2352
		audio = true
		if *towav {
			extension = "wav"
		} else {
			extension = "cdr"
		}

	default:
		fmt.Printf("(?) ")
		bsize = 2352
		extension = "ugh"
	}

	return Track{
		Num:       num,
		Audio:     audio,
		Mode:      mode,
		Extension: extension,
		Bstart:    bstart,
		Bsize:     bsize,
		StartSect: -1,
		StopSect:  -1,
	}
}

func progressbar(f float64, l int) string {
	s := ""
	n := int64(float64(l) * f)
	i := int64(0)
	for ; i < n; i++ {
		s += "*"
	}
	for ; i < int64(l); i++ {
		s += " "
	}

	return s
}

func writeTrack(bf *os.File, track Track, basename string) {
	name := fmt.Sprintf("%s%2.2d.%s", basename, track.Num, track.Extension)

	f, err := os.Create(name)
	ck(err)

	_, err = bf.Seek(track.Start, os.SEEK_SET)
	ck(err)

	length := (track.StopSect - track.StartSect + 1) * track.Bsize
	if *verbose {
		fmt.Printf("\n mmc sectors %d-%d (%d)",
			track.StartSect, track.StopSect, track.StopSect-track.StartSect+1)
		fmt.Printf("\n mmc bytes %d-%d (%d)", track.Start, track.Stop, track.Stop-track.Start+1)
		fmt.Printf("\n sector data at %d, %d bytes per sector", track.Bstart, track.Bsize)
		fmt.Printf("\n real data %d bytes", (track.StopSect-track.StartSect+1)*track.Bsize)
		fmt.Printf("\n")
	}
	fmt.Printf("                                          ")

	w := bufio.NewWriter(f)
	if track.Audio && *towav {
		fmt.Fprintf(w, "RIFF")
		binary.Write(w, binary.LittleEndian, uint32(length+WAV_DATA_HLEN+WAV_FORMAT_HLEN+4))
		fmt.Fprintf(w, "WAVE")
		fmt.Fprintf(w, "fmt ")
		binary.Write(w, binary.LittleEndian, uint32(0x10))    // length of FORMAT header
		binary.Write(w, binary.LittleEndian, uint16(0x01))    // constant
		binary.Write(w, binary.LittleEndian, uint16(0x02))    // channels
		binary.Write(w, binary.LittleEndian, uint32(44100))   // sample rate
		binary.Write(w, binary.LittleEndian, uint32(44100*4)) // bytes per second
		binary.Write(w, binary.LittleEndian, uint16(4))       // bytes per sample
		binary.Write(w, binary.LittleEndian, uint16(2*8))     // bits per channel
		fmt.Fprintf(w, "data")
		binary.Write(w, binary.LittleEndian, uint32(length)) // length
	}

	var buf [SECTLEN + 10]byte
	if track.Bstart+track.Bsize > int64(len(buf)) {
		log.Fatalf("invalid size %d at offset %d", track.Bsize, track.Bstart)
	}

	realsz := 0
	sz := track.Start
	sect := track.StartSect
	for sect <= track.StopSect {
		_, err = io.ReadAtLeast(bf, buf[:], len(buf))
		if err != nil {
			break
		}

		if track.Audio {
			if *swabaudio {
				p := buf[track.Bstart:]
				for i := int64(0); i < track.Bsize; i += 2 {
					p[i], p[i+1] = p[i+1], p[i]
				}
			}
		}

		w.Write(buf[track.Bstart : track.Bstart+track.Bsize])

		sect++
		sz += SECTLEN
		realsz += int(track.Bsize)
		if (sz/SECTLEN)%500 == 0 {
			fl := float64(realsz) / float64(length)
			fmt.Printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b%4d/%-4d MB  [%s] %3.0f %%",
				realsz/1024/1024, length/1024/1024, progressbar(fl, 20), fl*100)
		}
	}
	fl := float64(realsz) / float64(length)
	fmt.Printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b%4d/%-4d MB  [%s] %3.0f %%",
		realsz/1024/1024, length/1024/1024, progressbar(fl, 20), fl*100)

	ck(w.Flush())
	ck(f.Close())
}
