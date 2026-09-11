// https://en.wikipedia.org/wiki/Dual-tone_multi-frequency_signaling
// generates dtmf tones
package main

import (
	"encoding/binary"
	"flag"
	"fmt"
	"log"
	"math"
	"os"
	"runtime"
	"time"
	"unicode"

	"github.com/qeedquan/go-media/math/ga"
	"github.com/qeedquan/go-media/sdl"
)

var flags struct {
	Freq     int
	Channels int
	Warmup   time.Duration
	Duration float64
	Delay    float64
	Loop     bool
}

var (
	auspec sdl.AudioSpec
	audev  sdl.AudioDeviceID
	dtmf   DTMF
)

func main() {
	runtime.LockOSThread()
	parseflags()
	initsdl()
	initdtmf()

	audev.PauseAudio(0)
	time.Sleep(flags.Warmup)

	buf := make([]byte, auspec.Size)
	for !dtmf.Done() {
		if audev.GetQueuedAudioSize() < len(buf) {
			buflen := dtmf.Gen(buf[:])
			err := audev.QueueAudio(buf[:buflen])
			if err != nil {
				fmt.Println(err)
			}
		}

		if dtmf.Done() && flags.Loop {
			dtmf.Init(&auspec, toneprint)
		}
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] <keypad> ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func parseflags() {
	flags.Freq = 48000
	flags.Channels = 2
	flags.Duration = 0.3
	flags.Delay = 0.2
	flags.Warmup = 10 * time.Millisecond

	flag.BoolVar(&flags.Loop, "loop", flags.Loop, "loop")
	flag.Float64Var(&flags.Duration, "duration", flags.Duration, "duration of one tone")
	flag.Float64Var(&flags.Delay, "delay", flags.Delay, "delay to next tone")
	flag.DurationVar(&flags.Warmup, "warmup", flags.Warmup, "warmup delay")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}
}

func initsdl() {
	err := sdl.Init(sdl.INIT_AUDIO | sdl.INIT_TIMER)
	ck(err)

	spec := sdl.AudioSpec{
		Freq:     flags.Freq,
		Format:   sdl.AUDIO_S16LSB,
		Channels: uint8(flags.Channels),
		Samples:  4096,
	}
	spec, err = sdl.OpenAudio(spec)
	ck(err)

	auspec = spec
	audev = 1
}

func initdtmf() {
	dtmf.Init(&auspec, toneprint)
	for _, arg := range flag.Args() {
		for _, key := range arg {
			err := dtmf.AddTone(key, flags.Duration, flags.Delay)
			if err != nil {
				fmt.Fprintln(os.Stderr, err)
			}
		}
	}
}

func toneprint(_ int, t *Tone) {
	fmt.Printf("%c\n", t.key)
}

type Tone struct {
	key      rune
	freq     []float64
	duration float64
	delay    float64
}

type DTMF struct {
	spec   *sdl.AudioSpec
	tones  []Tone
	pos    int
	tick   float64
	once   bool
	nextcb func(int, *Tone)
}

func (p *DTMF) Init(spec *sdl.AudioSpec, nextcb func(int, *Tone)) {
	p.spec = spec
	p.pos = 0
	p.tick = 0
	p.once = false
	p.nextcb = nextcb
}

func (p *DTMF) AddTone(key rune, duration, delay float64) error {
	tab := [][]rune{
		{'1', '2', '3', 'A'},
		{'4', '5', '6', 'B'},
		{'7', '8', '9', 'C'},
		{'*', '0', '#', 'D'},
	}
	ftab := [][]float64{
		{697, 770, 852, 941},
		{1209, 1336, 1477, 1633},
	}

	key = unicode.ToUpper(key)
	for i := range tab {
		for j := range tab[i] {
			if key == tab[i][j] {
				p.tones = append(p.tones, Tone{
					key:      key,
					freq:     []float64{ftab[0][i], ftab[1][j]},
					duration: duration,
					delay:    delay,
				})
				return nil
			}
		}
	}
	return fmt.Errorf("unknown key %c", key)
}

func (p *DTMF) runcb() {
	if p.pos >= len(p.tones) || p.nextcb == nil {
		return
	}
	p.nextcb(p.pos, &p.tones[p.pos])
}

func (p *DTMF) Gen(buf []byte) int {
	if !p.once {
		p.runcb()
		p.once = true
	}

	dt := 1 / float64(p.spec.Freq)
	chans := int(p.spec.Channels)

	i := 0
loop:
	for {
		s := 0.0
		if p.pos < len(p.tones) {
			te := &p.tones[p.pos]
			switch {
			case p.tick < te.duration:
				for _, f := range te.freq {
					w := 2 * math.Pi * f
					s += math.Sin(w * p.tick)
				}
				fallthrough

			case p.tick < te.duration+te.delay:
				p.tick += dt

			default:
				p.pos++
				p.tick = 0
				p.runcb()
			}
		}
		s *= 0.5

		v := ga.LinearRemap(s, -1, 1, math.MinInt16, math.MaxInt16)

		for j := 0; j < chans; j++ {
			if i+2 >= len(buf) {
				break loop
			}
			binary.LittleEndian.PutUint16(buf[i:], uint16(v))
			i += 2
		}
	}

	return i
}

func (p *DTMF) Done() bool {
	return p.pos >= len(p.tones)
}
