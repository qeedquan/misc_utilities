package main

import "math"

type RangeEntry struct {
	Min, Max float64
}

type RangeList struct {
	Ranges        []RangeEntry
	NumRanges     int
	smallestRange float64
}

func NewRangeList(min, max float64) *RangeList {
	r := &RangeList{
		Ranges:        make([]RangeEntry, 8),
		smallestRange: 0.000001,
	}
	r.Reset(min, max)
	return r
}

func (r *RangeList) Reset(min, max float64) {
	r.NumRanges = 1
	r.Ranges[0] = RangeEntry{min, max}
}

func (r *RangeList) Subtract(a, b float64) {
	const twoPi = 2 * math.Pi
	switch {
	case a > twoPi:
		r.Subtract(a-twoPi, b-twoPi)
	case b < 0:
		r.Subtract(a+twoPi, b+twoPi)
	case a < 0:
		r.Subtract(0, b)
		r.Subtract(a+twoPi, twoPi)
	case b > twoPi:
		r.Subtract(a, twoPi)
		r.Subtract(0, b-twoPi)
	case r.NumRanges == 0:
	default:
		var pos int
		if a < r.Ranges[0].Min {
			pos = -1
		} else {
			lo := 0
			mid := 0
			hi := r.NumRanges

			for lo < hi-1 {
				mid = (lo + hi) / 2
				if r.Ranges[mid].Min < a {
					lo = mid
				} else {
					hi = mid
				}
			}
			pos = lo
		}

		if pos == -1 {
			pos = 0
		} else if a < r.Ranges[pos].Max {
			c := r.Ranges[pos].Min
			d := r.Ranges[pos].Max
			if a-c < r.smallestRange {
				if b < d {
					r.Ranges[pos].Min = b
				} else {
					r.DeleteRange(pos)
				}
			}
		} else {
			if pos < r.NumRanges-1 && b > r.Ranges[pos+1].Min {
				pos++
			} else {
				return
			}
		}

		for pos < r.NumRanges && b > r.Ranges[pos].Min {
			if r.Ranges[pos].Max < r.smallestRange {
				r.DeleteRange(pos)
			} else {
				r.Ranges[pos].Min = b
			}
		}
	}
}

func (r *RangeList) DeleteRange(pos int) {
	if pos < r.NumRanges-1 {
		copy(r.Ranges[pos:], r.Ranges[pos+1:])
	}
	r.NumRanges--
}
