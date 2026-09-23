/*

Generates the Daisy Bell instrumental
Ported from https://github.com/ayoungblood/daisy-bell

https://en.wikipedia.org/wiki/Daisy_Bell
https://www.historyofinformation.com/detail.php?entryid=4445

*/

package main

import (
	"encoding/binary"
	"fmt"
	"log"
	"math"
	"os"
)

type WAV struct {
	ChunkID       [4]uint8
	ChunkSize     uint32
	Format        [4]uint8
	SubChunk1ID   [4]uint8
	SubChunk1Size uint32
	AudioFormat   uint16
	NumChannels   uint16
	SampleRate    uint32
	ByteRate      uint32
	BlockAlign    uint16
	BitsPerSample uint16
	SubChunk2ID   [4]uint8
	SubChunk2Size uint32
}

func main() {
	signal := daisybell()
	err := writewav("daisy_bell.wav", signal)
	check(err)
}

func check(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func daisybell() []int16 {
	// calculate length of audio in seconds
	seconds := 0.0
	for i := range DAISY_T {
		seconds += DAISY_T[i] * QUARTER
	}

	// calculate the sample rate from seconds
	samples := int(math.Ceil(seconds) * SAMPLERATE)
	signal := make([]int16, samples)
	fmt.Printf("Length: %.2fs; Samples: %d\n", seconds, samples)

	// generate the song from time and pitch intervals
	index := 0
	for i := range DAISY_T {
		note := make([]int16, int(math.Ceil(DAISY_T[i]*QUARTER*SAMPLERATE))+1)
		freq := math.Pow(2, (DAISY_P[i]-69)/12.0) * A440
		for j := range note {
			t := float64(j) / SAMPLERATE
			note[j] = int16(math.Floor(32768 * math.Sin(2*math.Pi*freq*t)))
			note[j] = int16(float64(note[j]) * math.Exp(-2.0/(DAISY_T[i]*QUARTER)*t))
		}

		copy(signal[index:], note)
		index += len(note) - 1
	}
	return signal
}

func writewav(name string, signal []int16) error {
	const (
		chans = 1
		bps   = 16
	)
	samples := uint32(len(signal))
	header := WAV{
		ChunkID:       [4]uint8{'R', 'I', 'F', 'F'},
		Format:        [4]uint8{'W', 'A', 'V', 'E'},
		SubChunk1ID:   [4]uint8{'f', 'm', 't', ' '},
		SubChunk2ID:   [4]uint8{'d', 'a', 't', 'a'},
		AudioFormat:   1,
		NumChannels:   chans,
		BitsPerSample: bps,
		BlockAlign:    chans * bps / 8,
		SubChunk2Size: samples * chans * bps / 8,
		SubChunk1Size: 16,
		SampleRate:    SAMPLERATE,
		ByteRate:      SAMPLERATE * chans * bps / 8,
	}
	header.ChunkSize = 4 + (8 + header.SubChunk1Size) + (8 + header.SubChunk2Size)

	fd, err := os.Create(name)
	if err != nil {
		return err
	}
	defer fd.Close()

	binary.Write(fd, binary.LittleEndian, &header)
	binary.Write(fd, binary.LittleEndian, signal)
	return nil
}

const (
	SAMPLERATE = 44100 // Sample rate in hertz
	QUARTER    = 0.180 // Quarter note length in seconds
	A440       = 440   // Frequency constant
)

var DAISY_P = []float64{
	74, 71, 67, 62, 64, 66, 67, 64, 67, 62,
	69, 74, 71, 67, 64, 66, 67, 69, 71, 69,
	71, 72, 71, 69, 74, 71, 69, 67, 69, 71, 67, 64, 67, 64, 62,
	62, 67, 71, 69, 67, 71, 69, 71, 72, 74, 71, 67, 69, 62, 67,
}

var DAISY_T = []float64{
	3, 3, 3, 3, 1, 1, 1, 2, 1, 6,
	3, 3, 3, 3, 1, 1, 1, 2, 1, 6,
	1, 1, 1, 1, 2, 1, 1, 4, 1, 2, 1, 2, 1, 1, 5,
	1, 2, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 5,
}
