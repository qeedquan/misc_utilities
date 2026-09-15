// based on https://github.com/OneLoneCoder/synth
package main

import (
	"encoding/binary"
	"encoding/json"
	"flag"
	"fmt"
	"log"
	"math"
	"math/rand"
	"os"
	"runtime"
	"time"

	"github.com/qeedquan/go-media/math/ga"
	"github.com/qeedquan/go-media/sdl"
)

var flags struct {
	Randomize  bool
	Verbose    bool
	PrintScale float64

	Freq     int
	Channels int
	Spec     sdl.AudioSpec
}

func main() {
	runtime.LockOSThread()
	log.SetFlags(0)
	log.SetPrefix("olcsynth: ")
	rand.Seed(time.Now().UnixNano())
	parseflags()
	if flags.PrintScale != 0 {
		for i := 0; i <= 32; i++ {
			fmt.Printf("%d %.3f %d %.3f\n",
				-i, scale2freq(flags.PrintScale, -i),
				i, scale2freq(flags.PrintScale, i))
		}
		os.Exit(0)
	}

	var sheet *Sheet
	var err error
	switch {
	case flags.Randomize:
		sheet = randsheet()
	case flag.NArg() == 0:
		usage()
	default:
		sheet, err = loadsheet(flag.Arg(0))
		ck(err)
	}

	seq := newnoiser(sheet)
	if flags.Verbose {
		dumpsheet(sheet)
	}
	initsdl()
	sim(seq, sheet)
	fmt.Println("done")
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] <sheet>")
	flag.PrintDefaults()
	os.Exit(2)
}

func parseflags() {
	flags.Freq = 48000
	flags.Verbose = true
	flags.Channels = 2
	flag.Float64Var(&flags.PrintScale, "ps", flags.PrintScale, "print the scales")
	flag.BoolVar(&flags.Randomize, "r", flags.Randomize, "randomize sheet")
	flag.BoolVar(&flags.Verbose, "v", flags.Verbose, "be verbose")
	flag.Usage = usage
	flag.Parse()
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
	flags.Spec = spec
}

type Sheet struct {
	Duration   float64
	Repeat     bool
	Tempo      float64
	Beats      int
	Subbeats   int
	Instrument []struct {
		Name string
		Beat string
	}
}

func dumpsheet(sheet *Sheet) {
	b, err := json.MarshalIndent(sheet, "", "\t")
	ck(err)
	fmt.Printf("%s\n", b)
}

func loadsheet(name string) (*Sheet, error) {
	b, err := os.ReadFile(name)
	if err != nil {
		return nil, err
	}

	var s Sheet
	err = json.Unmarshal(b, &s)
	return &s, err
}

func randsheet() *Sheet {
	tab := []string{
		"Drum Snare", "Drum Kick", "Drum HiHat",
	}
	s := &Sheet{}
	s.Duration = rand.Float64() * 30
	s.Repeat = true
	s.Tempo = 60 + rand.Float64()*120
	s.Beats = 4 + rand.Intn(16)
	s.Subbeats = 4 + rand.Intn(16)
	for i := 0; i < 4; i++ {
		var p struct {
			Name string
			Beat string
		}

		beat := ""
		for i := 0; i < s.Beats*s.Subbeats; i++ {
			if rand.Float64() >= 0.5 {
				beat += "X"
			} else {
				beat += "."
			}
		}

		p.Name = tab[rand.Intn(len(tab))]
		p.Beat = beat
		s.Instrument = append(s.Instrument, p)
	}
	return s
}

func newnoiser(b *Sheet) *Sequencer {
	s := newsequencer(b.Tempo, b.Beats, b.Subbeats)
	for _, p := range b.Instrument {
		s.Channel = append(s.Channel, Channel{newinstrument(p.Name), p.Beat})
	}
	return s
}

func sim(s *Sequencer, b *Sheet) {
	audev := sdl.AudioDeviceID(1)
	audev.PauseAudio(0)

	var N []Note
	t0 := sdl.GetPerformanceCounter()
	t := 0.0
	for {
		if t >= b.Duration {
			if !b.Repeat {
				break
			}

			t = 0.0
			N = N[:0]
			fmt.Println("repeating")
		}

		t1 := sdl.GetPerformanceCounter()
		dt := float64(t1-t0) / float64(sdl.GetPerformanceFrequency())
		du := float64(flags.Spec.Samples) / float64(flags.Spec.Freq)
		if dt >= du && audev.GetQueuedAudioSize() < int(flags.Spec.Size) {
			N = step(s, b, t, dt, N)
			t0 = t1
			t += dt
		}
	}
}

func step(s *Sequencer, b *Sheet, t, dt float64, N []Note) []Note {
	var (
		buf    [0x80000]byte
		buflen int
	)
	audev := sdl.AudioDeviceID(1)
	f := 1 / float64(flags.Freq)

	for n := 0.0; n < dt; n += f {
		l := s.Update(f)
		for i := 0; i < l; i++ {
			s.Note[i].On = t + n
			N = append(N, s.Note[i])
		}

		M, v := makenoise(N, t+n)
		u := int16(ga.LinearRemap(v, -1, 1, math.MinInt16, math.MaxInt16))

		for i := 0; i < flags.Channels; i++ {
			if buflen >= len(buf) {
				audev.QueueAudio(buf[:buflen])
				buflen = 0
				fmt.Println("audio buffer overflowed", dt)
			}
			binary.LittleEndian.PutUint16(buf[buflen:], uint16(u))
			buflen += 2
		}

		N = M
	}
	audev.QueueAudio(buf[:buflen])

	return N
}

func makenoise(N []Note, t float64) (M []Note, val float64) {
	for i := range N {
		if N[i].Channel == nil {
			continue
		}

		snd, finished := N[i].Channel.Sample(t, N[i])
		val += snd
		if finished {
			N[i].Active = false
		}
	}

	for i := range N {
		if N[i].Active {
			M = append(M, N[i])
		}
	}
	return M, val * 0.2
}

const (
	OSC_SINE = iota
	OSC_SQUARE
	OSC_TRIANGLE
	OSC_SAW_ANA
	OSC_SAW_DIG
	OSC_NOISE
	OSC_MAX
)

func osc(t, hz float64, typ int, lfohz, lfoamp, custom float64) float64 {
	f := 2*math.Pi*hz*t + lfoamp*hz*math.Sin(2*math.Pi*lfohz*t)
	switch typ {
	case OSC_SINE:
		return math.Sin(f)
	case OSC_SQUARE:
		return ga.SignStrict(math.Sin(f))
	case OSC_TRIANGLE:
		return math.Asin(math.Sin(f)) * (2 / math.Pi)
	case OSC_SAW_ANA:
		v := 0.0
		for n := 1.0; n < custom; n++ {
			v += math.Sin(n*f) / n
		}
		return v * (2 / math.Pi)
	case OSC_SAW_DIG:
		return (2 / math.Pi) * (hz*math.Pi*math.Mod(t, 1/hz) - (math.Pi / 2))
	case OSC_NOISE:
		return 2*rand.Float64() - 1
	}
	return 0
}

type Envelope interface {
	Amplitude(t, ton, toff float64)
}

type ADSR struct {
	Attack  float64
	Decay   float64
	Sustain float64
	Release float64
	Start   float64
}

func (c *ADSR) Amplitude(t, ton, toff float64) float64 {
	val := 0.0
	lifetime := t - ton
	switch {
	case lifetime <= c.Attack:
		val = (lifetime / c.Attack) * c.Start
	case lifetime <= c.Attack+c.Decay:
		val = ((lifetime-c.Attack)/c.Decay)*(c.Sustain-c.Start) + c.Start
	default:
		val = c.Sustain
	}

	var amp float64
	if ton > toff {
		amp = val
	} else {
		amp = ((t-toff)/c.Release)*-val + val
	}

	return math.Max(amp, 0)
}

func scale2freq(basefreq float64, note int) float64 {
	twelthRootOfTwo := math.Pow(2, 1.0/12)
	return basefreq * math.Pow(twelthRootOfTwo, float64(note))
}

type Sampler interface {
	Sample(t float64, n Note) (float64, bool)
}

type Note struct {
	ID      int     // position in scale
	On      float64 // time note was activated
	Off     float64 // time note was deactivated
	Active  bool
	Channel Sampler
}

type Instrument struct {
	Flags       uint
	Volume      float64
	Env         ADSR
	BaseFreq    float64
	MaxLifeTime float64
	Name        string
	Osc         []Oscillator
}

type Channel struct {
	Instrument Sampler
	Beat       string
}

type Sequencer struct {
	Channel []Channel
	Note    []Note

	beats       int
	subbeats    int
	tempo       float64
	beattime    float64
	accumulate  float64
	currentbeat int
	totalbeats  int
}

type Oscillator struct {
	Type      int
	Shift     int
	Flip      bool
	Amp       float64
	LFOAmp    float64
	LFOFreq   float64
	Harmonics float64
}

func newsequencer(tempo float64, beats, subbeats int) *Sequencer {
	if tempo == 0 {
		tempo = 0.01
	}
	if subbeats == 0 {
		subbeats = 1
	}

	return &Sequencer{
		beats:      beats,
		subbeats:   subbeats,
		tempo:      tempo,
		beattime:   (60.0 / tempo) / float64(subbeats),
		totalbeats: subbeats * beats,
	}
}

func (c *Sequencer) Update(dt float64) int {
	c.Note = c.Note[:0]

	c.accumulate += dt
	for c.accumulate >= c.beattime {
		c.accumulate -= c.beattime

		c.currentbeat++
		if c.currentbeat >= c.totalbeats {
			c.currentbeat = 0
		}

		for _, v := range c.Channel {
			if len(v.Beat) == 0 {
				break
			}

			if v.Beat[c.currentbeat] == 'X' {
				c.Note = append(c.Note, Note{
					Channel: v.Instrument,
					Active:  true,
					ID:      0,
				})
			}
		}
	}

	return len(c.Note)
}

func newinstrument(name string) *Instrument {
	const DEFAULT_BASE_FREQ = 8

	var in *Instrument
	switch name {
	case "Bell":
		in = &Instrument{
			Env: ADSR{
				Attack:  0.01,
				Decay:   1.0,
				Sustain: 0.0,
				Release: 1.0,
			},
			BaseFreq:    DEFAULT_BASE_FREQ,
			MaxLifeTime: 3.0,
			Volume:      1.0,
			Name:        name,
			Osc: []Oscillator{
				{
					Type:    OSC_SINE,
					Amp:     1.0,
					Shift:   12,
					LFOFreq: 5.0,
					LFOAmp:  0.001,
				},
				{
					Type:  OSC_SINE,
					Amp:   0.50,
					Shift: 24,
				},
				{
					Type:  OSC_SINE,
					Amp:   0.25,
					Shift: 36,
				},
			},
		}

	case "8-Bit Bell":
		in = &Instrument{
			Env: ADSR{
				Attack:  0.01,
				Decay:   1.0,
				Sustain: 0.0,
				Release: 1.0,
			},
			BaseFreq:    DEFAULT_BASE_FREQ,
			MaxLifeTime: 3.0,
			Volume:      1.0,
			Name:        name,
			Osc: []Oscillator{
				{
					Type: OSC_SQUARE,
					Amp:  1.0,
				},
				{
					Type:  OSC_SINE,
					Amp:   0.50,
					Shift: 12,
				},
				{
					Type:  OSC_SINE,
					Amp:   0.25,
					Shift: 24,
				},
			},
		}

	case "Harmonica":
		in = &Instrument{
			Env: ADSR{
				Attack:  0.0,
				Decay:   1.0,
				Sustain: 0.95,
				Release: 0.1,
			},
			BaseFreq:    DEFAULT_BASE_FREQ,
			MaxLifeTime: -1.0,
			Volume:      0.3,
			Name:        name,
			Osc: []Oscillator{
				{
					Type:      OSC_SAW_ANA,
					Amp:       1.00,
					Flip:      true,
					Shift:     -12,
					LFOFreq:   5.0,
					LFOAmp:    0.001,
					Harmonics: 100,
				},
				{
					Type:    OSC_SQUARE,
					Amp:     1.00,
					LFOFreq: 5.0,
					LFOAmp:  0.001,
				},
				{
					Type:  OSC_SQUARE,
					Amp:   0.50,
					Shift: 12,
				},
				{
					Type:  OSC_NOISE,
					Amp:   0.05,
					Shift: 24,
				},
			},
		}

	case "Drum Kick":
		in = &Instrument{
			Env: ADSR{
				Attack:  0.01,
				Decay:   0.15,
				Sustain: 0.0,
				Release: 0.0,
			},
			BaseFreq:    DEFAULT_BASE_FREQ,
			MaxLifeTime: 1.5,
			Volume:      1.0,
			Name:        name,
			Osc: []Oscillator{
				{
					Type:    OSC_SINE,
					Amp:     0.99,
					Shift:   -36,
					LFOFreq: 1.0,
					LFOAmp:  1.0,
				},
				{
					Type: OSC_NOISE,
					Amp:  0.01,
				},
			},
		}

	case "Drum Snare":
		in = &Instrument{
			Env: ADSR{
				Attack:  0.0,
				Decay:   0.2,
				Sustain: 0.0,
				Release: 0.0,
			},
			BaseFreq:    DEFAULT_BASE_FREQ,
			MaxLifeTime: 1.0,
			Volume:      1.0,
			Name:        name,
			Osc: []Oscillator{
				{
					Type:    OSC_SINE,
					Amp:     0.5,
					Shift:   -24,
					LFOFreq: 0.5,
					LFOAmp:  1.0,
				},
				{
					Type: OSC_NOISE,
					Amp:  0.5,
				},
			},
		}

	case "Drum HiHat":
		in = &Instrument{
			Env: ADSR{
				Attack:  0.01,
				Decay:   0.05,
				Sustain: 0.0,
				Release: 0.0,
			},
			BaseFreq:    DEFAULT_BASE_FREQ,
			MaxLifeTime: 1.0,
			Volume:      0.5,
			Name:        name,
			Osc: []Oscillator{
				{
					Type:    OSC_SQUARE,
					Amp:     0.1,
					Shift:   -12,
					LFOFreq: 1.5,
					LFOAmp:  1.0,
				},
				{
					Type: OSC_NOISE,
					Amp:  0.9,
				},
			},
		}

	case "Pure":
		in = &Instrument{
			Env: ADSR{
				Attack:  0.0,
				Decay:   0.0,
				Sustain: 1.0,
				Release: 0.0,
			},
			BaseFreq:    220,
			MaxLifeTime: 1.0,
			Volume:      0.5,
			Name:        name,
			Osc: []Oscillator{
				{
					Type: OSC_SINE,
					Amp:  1.0,
				},
			},
		}

	case "Random":
		in = &Instrument{
			Env: ADSR{
				Attack:  rand.Float64(),
				Decay:   rand.Float64(),
				Sustain: rand.Float64(),
				Release: rand.Float64(),
			},
			BaseFreq:    DEFAULT_BASE_FREQ,
			MaxLifeTime: rand.Float64(),
			Volume:      0.5,
			Name:        name,
		}

		n := rand.Intn(8)
		for i := 0; i < n; i++ {
			in.Osc = append(in.Osc, Oscillator{
				Type:      rand.Intn(OSC_MAX),
				Amp:       rand.Float64(),
				Shift:     rand.Intn(32),
				LFOFreq:   rand.Float64(),
				LFOAmp:    rand.Float64(),
				Harmonics: rand.Float64() * 50,
			})
		}

	default:
		return nil
	}

	in.Env.Start = 1

	return in
}

func (c *Instrument) Sample(t float64, n Note) (val float64, finished bool) {
	amp := c.Env.Amplitude(t, n.On, n.Off)
	if c.MaxLifeTime > 0 && t-n.On >= c.MaxLifeTime {
		finished = true
	} else if c.MaxLifeTime < 0 && amp <= 0 {
		finished = true
	}

	snd := 0.0
	for _, o := range c.Osc {
		nt := t - n.On
		if o.Flip {
			nt = -nt
		}
		snd += o.Amp * osc(nt, scale2freq(c.BaseFreq, n.ID+o.Shift), o.Type, o.LFOFreq, o.LFOAmp, o.Harmonics)
	}

	val = snd * amp * c.Volume
	return
}
