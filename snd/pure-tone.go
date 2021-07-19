// https://en.wikipedia.org/wiki/Pure_tone
// https://en.wikipedia.org/wiki/Semitone
// https://en.wikipedia.org/wiki/Twelfth_root_of_two
package main

import (
	"encoding/binary"
	"flag"
	"fmt"
	"log"
	"math"
	"runtime"
	"time"

	"github.com/qeedquan/go-media/math/f64"
	"github.com/qeedquan/go-media/sdl"
)

type Option struct {
	Channels uint8
	Freq     int
	Base     float64
	Samples  uint16
	Size     uint32
	Mode     int
}

type Pure struct {
	Base     float64
	Semitone float64
	Step     float64
	Freq     float64
}

type Timer struct {
	Tick float64
	Last time.Time
}

var opt = Option{
	Channels: 2,
	Freq:     48000,
	Base:     440,
	Samples:  4096,
	Mode:     3,
}

var (
	audev sdl.AudioDeviceID
	timer Timer
	pure  Pure
)

func main() {
	runtime.LockOSThread()
	flag.Float64Var(&opt.Base, "b", opt.Base, "base frequency")
	flag.IntVar(&opt.Freq, "s", opt.Freq, "sampling rate")
	flag.IntVar(&opt.Mode, "m", opt.Mode, "mode")
	flag.Parse()
	initsdl()
	initsynth()

	fmt.Printf("mode %d %f hz\n", opt.Mode, pure.Freq)
	timer.Last = time.Now()
	buf := make([]byte, opt.Freq*32)
	d0 := time.Duration(0)
	d1 := time.Duration(0)
	t0 := time.Now()

	// test several modes of playback
	// case 0: this is like case 3 but we use elapsed time instead of fixed timestep to generate the buffer, it leads to distortions
	// case 1: the queue size is never stable, the buffer we have to fill differs everytime
	// case 2: use the audio thread callback to handle this for us, this works well because the C thread is the one that request the data
	// case 3: we push based on ticks, but we do use real time to fill in the audio, instead we do best effort where we assume
	//         that the timer is fast enough to meet the buffer fill, then we just increment a fixed interval to fill the buffer
	//         this case works the best for the push api, however the C audio thread buffer is still increasing too quickly due to the unequal
	//         timeslices, we can fix this by allowing some time for the audio buffer drain by checking the size
	//         by adding the audio buffer size waiting, it leads us to the same kind of time beats that case 2 have instead of the fixed frequency
	//         of when we fill the buffer
	for {
		switch opt.Mode {
		case 0:
			du, err := time.ParseDuration(fmt.Sprintf("%fms", 1/(float64(opt.Samples)/float64(opt.Freq))))
			ck(err)
			t1 := time.Now()
			dt := t1.Sub(t0)
			if dt >= du && audev.GetQueuedAudioSize() < int(opt.Size/2) {
				d1 += dt
				pt0 := float64(d0.Milliseconds()) / 1e3
				pt1 := float64(d1.Milliseconds()) / 1e3

				buflen := pure.Fill(buf, pt0, pt1, (pt1-pt0)/float64(opt.Samples))
				fmt.Println(pt0, pt1, d0, d1, d1-d0, buflen, audev.GetQueuedAudioSize())
				err := audev.QueueAudio(buf[:buflen])
				if err != nil {
					fmt.Println(err)
				}

				d0 = d1
				t0 = t1
			}

		case 1:
			if audev.GetQueuedAudioSize() < int(opt.Size) {
				t1 := time.Now()
				dt := t1.Sub(t0)
				d0 = d1
				d1 += dt
				t0 = t1

				pt0 := float64(d0.Milliseconds()) / 1e3
				pt1 := float64(d1.Milliseconds()) / 1e3

				buflen := pure.Fill(buf, pt0, pt1, 1/float64(opt.Freq))
				fmt.Println(pt0, pt1, d0, d1, d1-d0, buflen, audev.GetQueuedAudioSize())
				err := audev.QueueAudio(buf[:buflen])
				if err != nil {
					fmt.Println(err)
				}
			}

		case 2:

		case 3:
			du, err := time.ParseDuration(fmt.Sprintf("%fms", 1/(float64(opt.Samples)/float64(opt.Freq))))
			ck(err)
			t1 := time.Now()
			dt := t1.Sub(t0)
			buflen := 0
			if dt >= du && audev.GetQueuedAudioSize() < int(opt.Size) {
				audiofill(buf[:opt.Size])
				buflen = int(opt.Size)
				err := audev.QueueAudio(buf[:buflen])
				if err != nil {
					fmt.Println(err)
				}

				t0 = t1
			}

		default:
			log.Fatal("unknown mode ", opt.Mode)
		}
	}
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func initsdl() {
	err := sdl.Init(sdl.INIT_AUDIO)
	ck(err)

	spec := sdl.AudioSpec{
		Freq:     opt.Freq,
		Format:   sdl.AUDIO_S16LSB,
		Channels: opt.Channels,
		Samples:  opt.Samples,
	}
	if opt.Mode == 2 {
		spec.Callback = audiofill
	}
	spec, err = sdl.OpenAudio(spec)
	ck(err)
	fmt.Printf("%#v\n", spec)

	audev = sdl.AudioDeviceID(1)

	opt.Samples = spec.Samples
	opt.Size = spec.Size
}

func initsynth() {
	pure = Pure{
		Base:     opt.Base,
		Semitone: math.Pow(2, 1.0/12),
		Step:     0,
	}
	pure.Freq = pure.calcfreq()
	audev.PauseAudio(0)
}

func audiofill(b []byte) {
	now := time.Now()
	fmt.Println(now.Sub(timer.Last), len(b), audev.GetQueuedAudioSize())
	timer.Last = now

	for i := 0; i < len(b); {
		s := pure.Sample(timer.Tick) * 0.5
		v := int16(f64.LinearRemap(s, -1, 1, math.MinInt16, math.MaxInt16))

		for j := uint8(0); j < opt.Channels; j++ {
			if i > len(b) {
				break
			}

			binary.LittleEndian.PutUint16(b[i:], uint16(v))
			i += 2
		}

		timer.Tick += 1.0 / float64(opt.Freq)
	}
}

func (p *Pure) calcfreq() float64 {
	return p.Base * math.Pow(p.Semitone, p.Step)
}

func (p *Pure) Sample(t float64) float64 {
	return math.Sin(p.Freq * 2.0 * math.Pi * t)
}

func (p *Pure) Fill(b []byte, t0, t1, dt float64) int {
	i := 0
	ti := math.Floor(t0)
	x := math.Ceil((t0 - ti) / dt)
	for t := ti + x*dt; t < t1; t += dt {
		if i >= len(b) {
			fmt.Println("overflowed sound buffer")
			break
		}

		s := p.Sample(t) * 0.5
		v := int16(f64.LinearRemap(s, -1, 1, math.MinInt16, math.MaxInt16))
		for j := uint8(0); j < opt.Channels; j++ {
			binary.LittleEndian.PutUint16(b[i:], uint16(v))
			i += 2
		}
	}
	return i
}
