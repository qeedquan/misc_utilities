package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"os"
	"strconv"
	"strings"
)

var flags struct {
	verbose bool
}

func main() {
	parseflags()

	g := &GNS{Verbose: flags.verbose}
	if flag.NArg() < 1 {
		g.Parse(os.Stdin)
	} else {
		for _, name := range flag.Args() {
			f, err := os.Open(name)
			if ek(err) {
				continue
			}
			g.Parse(f)
			f.Close()
		}
	}
}

func ek(err error) bool {
	if err != nil {
		fmt.Fprintln(os.Stderr, "nmea-dumper: ", err)
		return true
	}
	return false
}

func parseflags() {
	flag.BoolVar(&flags.verbose, "v", true, "be verbose")
	flag.Usage = usage
	flag.Parse()
}

func usage() {
	fmt.Fprintln(os.Stderr, "nmea-dumper usage: [options] file ...")
	flag.PrintDefaults()
	os.Exit(2)
}

type GNS struct {
	inp     string
	gga     Location
	gll     Location
	rmc     Location
	Verbose bool
}

type Location struct {
	// GLL/GNA
	Latitude  float64
	Longitude float64
	Timestamp string
	Valid     bool

	// GNA
	Quality           int
	Satellites        int
	HDOP              float64
	Altitude          float64
	GeoidalSeparation float64
	DGNS              int

	// RMC
	Speed  float64
	Course float64
	Mode   rune
	Date   string
}

func (c *GNS) Parse(r io.Reader) {
	sc := bufio.NewScanner(r)
	for {
		ln, eof := c.readline(sc)
		if eof {
			break
		}

		err := c.ParseNMEA(ln)
		if err != nil {
			fmt.Fprintf(os.Stderr, "nmea-dumper: %q: %q\n", ln, err)
		}
		fmt.Printf("GLL: %#+v\n", c.gll)
		fmt.Printf("GGA: %#+v\n", c.gga)
		fmt.Printf("RMC: %#+v\n", c.rmc)
		fmt.Println()
	}
}

func (c *GNS) readline(sc *bufio.Scanner) (ln string, eof bool) {
	eof = true
	for sc.Scan() {
		c.inp += sc.Text()
		i := strings.IndexByte(c.inp, '$')
		j := strings.IndexByte(c.inp, '*')
		if i >= 0 && j >= 0 && j+2 < len(c.inp) {
			ln = c.inp[i : j+3]
			c.inp = c.inp[j+3:]
			eof = false
			break
		}
	}
	return
}

// http://navspark.mybigcommerce.com/content/NMEA_Format_v0.1.pdf
func (c *GNS) ParseNMEA(str string) error {
	c.dprintf("NMEA: %q\n", str)

	str = strings.TrimSpace(str)
	if str == "" {
		return nil
	}

	if len(str) < 2 {
		return fmt.Errorf("input too short")
	}

	n := strings.LastIndexByte(str, '*')
	if str[0] != '$' || n < 0 || str[n] != '*' {
		return fmt.Errorf("no marker boundary")
	}

	cs, err := strconv.ParseInt(str[n+1:], 16, 0)
	if err != nil {
		return fmt.Errorf("no checksum")
	}

	str = str[1:n]
	ccs := checksum(str)
	if byte(cs) != byte(ccs) {
		return fmt.Errorf("checksum mismatch: %#x %#x", cs, ccs)
	}

	t := strings.Split(str, ",")
	for i := range t {
		if t[i] == "" {
			t[i] = "X"
		}
	}
	switch t[0] {
	case "GNTXT": // display text
		fmt.Printf("Text: %s\n", t[len(t)-1])

	case "GNGGA": // time, position, fix of receiver
		if len(t) < 11 {
			break
		}
		p := &c.gga
		p.Timestamp = t[1]
		p.Latitude = latlong(t[2], t[3][0])
		p.Longitude = latlong(t[4], t[5][0])
		p.Quality, _ = strconv.Atoi(t[6])
		p.Satellites, _ = strconv.Atoi(t[7])
		p.HDOP, _ = strconv.ParseFloat(t[7], 64)
		p.Altitude, _ = strconv.ParseFloat(t[8], 64)
		p.GeoidalSeparation, _ = strconv.ParseFloat(t[9], 64)
		p.DGNS, _ = strconv.Atoi(t[10])

	case "GNRMC": // time, date, position, course and speed
		if len(t) < 11 {
			break
		}
		p := &c.rmc
		p.Timestamp = t[1]
		p.Valid = t[2] == "A"
		p.Latitude = latlong(t[3], t[4][0])
		p.Longitude = latlong(t[5], t[6][0])
		p.Speed, _ = strconv.ParseFloat(t[7], 64)
		p.Course, _ = strconv.ParseFloat(t[8], 64)
		p.Date = t[9]
		p.Mode = rune(t[10][0])

	case "GNVTG": // course and speed relative to ground

	case "GNGLL": // position, time, fix status
		if len(t) < 6 {
			break
		}
		p := &c.gll
		p.Latitude = latlong(t[1], t[2][0])
		p.Longitude = latlong(t[3], t[4][0])
		p.Timestamp = t[5]
		p.Valid = t[6] == "A"

	case "GNGSA", "GPGSA", "BDGSA": // satellite ids

	case "GPGSV", "GLGSV": // elevation, azimuth, and carrier to noise (cnr)

	default:
		return fmt.Errorf("unsupported message")
	}

	return nil
}

func (c *GNS) dprintf(format string, args ...interface{}) {
	if c.Verbose {
		fmt.Printf(format, args...)
	}
}

func checksum(str string) byte {
	var v byte
	for i := 0; i < len(str); i++ {
		v ^= str[i]
	}
	return v
}

// https://journeynorth.org/tm/LongitudeIntro.html
func latlong(str string, dir byte) float64 {
	prec := 2
	if dir == 'W' || dir == 'E' {
		prec = 3
	}
	if len(str) < prec {
		return 0
	}

	deg, _ := strconv.ParseFloat(str[:prec], 64)
	min, _ := strconv.ParseFloat(str[prec:], 64)
	val := deg + min/60

	if dir == 'S' || dir == 'W' {
		val = -val
	}

	return val
}
