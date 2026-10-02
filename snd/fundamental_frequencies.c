/*

https://www.songstuff.com/recording/article/music-fundamental-frequencies/

Notes are divided into 12 frequencies (7 if we exclude the sharp notes)
A higher octave represents a doubling in frequency

*/

#include <stdio.h>
#include <math.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

void
table()
{
	static const char *notes[] = {
		"C",
		"C#/Db",
		"D",
		"D#/E",
		"E",
		"F",
		"F#/G",
		"G",
		"G#Ab",
		"A",
		"A#B",
		"B",
	};

	static const int pitchs = 8;

	double basefreq;
	double freq;
	int pitch;
	size_t note;

	basefreq = 16.35160;
	for (pitch = 0; pitch <= pitchs; pitch++) {
		for (note = 0; note < nelem(notes); note++) {
			freq = basefreq * pow(2, note * 1.0 / nelem(notes));
			printf("%8s%d: %f\n", notes[note], pitch, freq);
		}
		basefreq *= 2;
		printf("\n");
	}
}

int
main()
{
	table();
	return 0;
}
