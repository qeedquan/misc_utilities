// ported from https://github.com/ddunbar/PDSample
package main

import (
	"math"
	"math/rand"
	"sort"
	"time"

	"github.com/qeedquan/go-media/math/f64"
	"github.com/qeedquan/go-media/math/rng"
)

type Sampler struct {
	neighbors    []int
	grid         [][]int
	gridSize     int
	gridCellSize float64
	isTiled      bool
	radius       float64
	points       []f64.Vec2
	rng          rng.RNG
}

type Interface interface {
	Complete()
	Maximize()
	Points() []f64.Vec2
}

func NewSampler(radius float64, isTiled, usesGrid bool) *Sampler {
	const maxPointsPerCell = 9

	var (
		gridSize     int
		gridCellSize float64
		grid         [][]int
	)
	// grid size is chosen so that 4*radius search only
	// requires search adjacent cells, also determine
	// max points per cells
	if usesGrid {
		gridSize = int(math.Ceil(2 / (4 * radius)))
		if gridSize < 2 {
			gridSize = 2
		}
		gridCellSize = 2 / float64(gridSize)
		grid = make([][]int, gridSize*gridSize)
		for i := range grid {
			grid[i] = make([]int, maxPointsPerCell)
		}
		for i := range grid {
			for j := range grid[i] {
				grid[i][j] = -1
			}
		}
	}

	return &Sampler{
		grid:         grid,
		gridSize:     gridSize,
		gridCellSize: gridCellSize,
		isTiled:      isTiled,
		radius:       radius,
		rng:          rand.New(rand.NewSource(time.Now().UnixNano())),
	}
}

func (s *Sampler) SetRNG(rng rng.RNG) {
	s.rng = rng
}

func (s *Sampler) Points() []f64.Vec2 {
	return s.points
}

func (s *Sampler) pointInDomain(p f64.Vec2) bool {
	return -1 <= p.X && p.X <= 1 &&
		-1 <= p.Y && p.Y <= 1
}

func (s *Sampler) randomPoint() f64.Vec2 {
	return f64.Vec2{
		2*s.rng.Float64() - 1,
		2*s.rng.Float64() - 1,
	}
}

func (s *Sampler) getDistanceSquared(a, b f64.Vec2) float64 {
	v := s.getTiled(b.Sub(a))
	return v.X*v.X + v.Y*v.Y
}

func (s *Sampler) getTiled(v f64.Vec2) f64.Vec2 {
	x, y := v.X, v.Y
	if s.isTiled {
		if x < -1 {
			x += 2
		} else if x > 1 {
			x -= 2
		}

		if y < -1 {
			y += 2
		} else if y > 1 {
			y -= 2
		}
	}
	return f64.Vec2{x, y}
}

func (s *Sampler) getGridXY(v f64.Vec2) (gx, gy int) {
	gx = int(math.Floor(0.5 * (v.X + 1) * float64(s.gridSize)))
	gy = int(math.Floor(0.5 * (v.Y + 1) * float64(s.gridSize)))
	return
}

func (s *Sampler) addPoint(pt f64.Vec2) {
	s.points = append(s.points, pt)
	if s.grid != nil {
		gx, gy := s.getGridXY(pt)
		cell := s.grid[gy*s.gridSize+gx]
		for i := range cell {
			if cell[i] == -1 {
				cell[i] = len(s.points) - 1
				return
			}
		}
		panic("internal error: overflowed max points per cell")
	}
}

func (s *Sampler) findNeighbors(pt f64.Vec2, distance float64) int {
	if s.grid == nil {
		panic("internal error: sampler cannot search without grid")
	}

	distanceSquared := distance * distance
	N := int(math.Ceil(distance / s.gridCellSize))
	if N > s.gridSize>>1 {
		N = s.gridSize >> 1
	}

	s.neighbors = s.neighbors[:0]
	gx, gy := s.getGridXY(pt)
	for j := -N; j <= N; j++ {
		for i := -N; i <= N; i++ {
			cx := (gx + i + s.gridSize) % s.gridSize
			cy := (gy + j + s.gridSize) % s.gridSize
			cell := s.grid[cy*s.gridSize+cx]

			for k := range cell {
				if cell[k] == -1 {
					break
				}
				if s.getDistanceSquared(pt, s.points[cell[k]]) < distanceSquared {
					s.neighbors = append(s.neighbors, cell[k])
				}
			}
		}
	}
	return len(s.neighbors)
}

func (s *Sampler) findClosestNeighbor(pt f64.Vec2, distance float64) float64 {
	if s.grid == nil {
		panic("internal error: sampler cannot search without grid")
	}

	closestSquared := distance * distance
	N := int(math.Ceil(distance / s.gridCellSize))
	if N > s.gridSize>>1 {
		N = s.gridSize >> 1
	}

	gx, gy := s.getGridXY(pt)
	for j := -N; j <= N; j++ {
		for i := -N; i <= N; i++ {
			cx := (gx + i + s.gridSize) % s.gridSize
			cy := (gy + j + s.gridSize) % s.gridSize
			cell := s.grid[cy*s.gridSize+cx]

			for k := range cell {
				if cell[k] == -1 {
					break
				}
				d := s.getDistanceSquared(pt, s.points[cell[k]])
				if d < closestSquared {
					closestSquared = d
				}
			}
		}
	}
	return math.Sqrt(closestSquared)
}

func (s *Sampler) findNeighborRanges(index int, rl *RangeList) {
	if s.grid == nil {
		panic("internal error, sampler cannot search without grid")
	}

	candidate := s.points[index]
	rangeSquared := 4 * 4 * s.radius * s.radius
	N := int(math.Ceil(4 * s.radius / s.gridCellSize))
	if N > s.gridSize>>1 {
		N = s.gridSize >> 1
	}

	gx, gy := s.getGridXY(candidate)

	xSide, ySide := 0, 0
	if (candidate.X - (-1 + float64(gx)*s.gridCellSize)) > s.gridCellSize*.5 {
		xSide = 1
	}
	if (candidate.Y - (-1 + float64(gx)*s.gridCellSize)) > s.gridCellSize*.5 {
		ySide = 1
	}
	iy := 1
	for j := -N; j <= N; j++ {
		ix := 1
		if j == 0 {
			iy = ySide
		} else if j == 1 {
			iy = 0
		}
		for i := -N; i <= N; i++ {
			if i == 0 {
				ix = xSide
			} else if i == 1 {
				ix = 0
			}

			// offset to closest cell point
			dx := candidate.X - (-1 + float64(gx+i+ix)*s.gridCellSize)
			dy := candidate.Y - (-1 + float64(gy+j+iy)*s.gridCellSize)
			if dx*dx+dy*dy < rangeSquared {
				cx := (gx + i + s.gridSize) % s.gridSize
				cy := (gy + j + s.gridSize) % s.gridSize
				cell := s.grid[cy*s.gridSize+cx]

				for k := range cell {
					if cell[k] == -1 {
						break
					}
					if cell[k] != index {
						pt := s.points[cell[k]]
						v := s.getTiled(pt.Sub(candidate))
						distSquared := v.X*v.X + v.Y*v.Y
						if distSquared < rangeSquared {
							dist := math.Sqrt(distSquared)
							angle := math.Atan2(v.Y, v.X)
							theta := math.Acos(.25 * dist / s.radius)

							rl.Subtract(angle-theta, angle+theta)
						}
					}
				}
			}
		}
	}
}

func (s *Sampler) Maximize() {
	rl := NewRangeList(0, 0)
	N := len(s.points)

	for i := 0; i < N; i++ {
		candidate := &s.points[i]
		rl.Reset(0, 2*math.Pi)
		s.findNeighborRanges(i, rl)
		for rl.NumRanges != 0 {
			re := rl.Ranges[rand.Intn(rl.NumRanges)]
			angle := re.Min + (re.Max-re.Min)*s.rng.Float64()
			pt := s.getTiled(f64.Vec2{
				candidate.X + math.Cos(angle)*2*s.radius,
				candidate.Y + math.Sin(angle)*2*s.radius,
			})
			s.addPoint(pt)
			rl.Subtract(angle-math.Pi/3, angle+math.Pi/3)
		}
	}
}

type DartThrowing struct {
	*Sampler
	minMaxThrows  int
	maxThrowsMult int
}

func NewDartThrowing(radius float64, isTiled bool, minMaxThrows, maxThrowsMult int) *DartThrowing {
	return &DartThrowing{
		Sampler:       NewSampler(radius, isTiled, true),
		minMaxThrows:  minMaxThrows,
		maxThrowsMult: maxThrowsMult,
	}
}

func (d *DartThrowing) Complete() {
	for {
		N := len(d.points) * d.maxThrowsMult
		if N < d.minMaxThrows {
			N = d.minMaxThrows
		}

		i := 0
		for ; i < N; i++ {
			pt := d.randomPoint()
			d.findNeighbors(pt, 2*d.radius)
			if len(d.neighbors) == 0 {
				d.addPoint(pt)
				break
			}
		}

		if i == N {
			break
		}
	}
}

type BestCandidate struct {
	*Sampler
	multiplier int
	n          int
}

func NewBestCandidate(radius float64, isTiled bool, multiplier int) *BestCandidate {
	return &BestCandidate{
		Sampler:    NewSampler(radius, isTiled, true),
		multiplier: multiplier,
		n:          int(0.7 / (radius * radius)),
	}
}

func (b *BestCandidate) Complete() {
	for i := 0; i < b.n; i++ {
		best := f64.Vec2{}
		bestDistance := 0.0
		count := 1 + len(b.points)*b.multiplier

		for j := 0; j < count; j++ {
			pt := b.randomPoint()
			closest := 2.0

			closest = b.findClosestNeighbor(pt, 4*b.radius)
			if j == 0 || closest > bestDistance {
				bestDistance = closest
				best = pt
			}
		}

		b.addPoint(best)
	}
}

type BoundarySampler struct {
	*Sampler
}

func NewBoundarySampler(radius float64, isTiled bool) *BoundarySampler {
	return &BoundarySampler{
		Sampler: NewSampler(radius, isTiled, true),
	}
}

func (b *BoundarySampler) Complete() {
	var candidates []int
	b.addPoint(b.randomPoint())
	candidates = append(candidates, len(b.points)-1)
	rl := NewRangeList(0, 0)

	for len(candidates) > 0 {
		c := rand.Int() % len(candidates)
		index := candidates[c]
		candidate := b.points[index]
		candidates[c] = candidates[len(candidates)-1]
		candidates = candidates[:len(candidates)-1]

		rl.Reset(0, 2*math.Pi)
		b.findNeighborRanges(index, rl)
		for rl.NumRanges != 0 {
			re := rl.Ranges[rand.Int()%rl.NumRanges]
			angle := re.Min + (re.Max-re.Min)*b.rng.Float64()
			pt := b.getTiled(f64.Vec2{
				candidate.X + math.Cos(angle)*2*b.radius,
				candidate.Y + math.Sin(angle)*2*b.radius,
			})
			b.addPoint(pt)
			candidates = append(candidates, len(b.points)-1)

			rl.Subtract(angle-math.Pi/3, angle+math.Pi/3)
		}
	}
}

type PureSampler struct {
	*Sampler
}

func NewPureSampler(radius float64) *PureSampler {
	return &PureSampler{
		Sampler: NewSampler(radius, true, true),
	}
}

func (p *PureSampler) Complete() {
	pt := p.randomPoint()
	rgn := NewScallopedRegion(pt, p.radius*2, p.radius*4, SCALLOPED_REGION_MIN_AREA)
	regions := make(map[int]*ScallopedRegion)
	var regionsPDF WDPDF

	p.addPoint(pt)
	regions[len(p.points)-1] = rgn
	regionsPDF.Insert(len(p.points)-1, rgn.Area)

	for len(regions) > 0 {
		idx := regionsPDF.Choose(p.rng.Float64())

		pt := p.getTiled(regions[idx].Sample())
		rgn := NewScallopedRegion(pt, p.radius*2, p.radius*4, SCALLOPED_REGION_MIN_AREA)

		p.findNeighbors(pt, p.radius*8)
		for _, nIdx := range p.neighbors {
			n := p.points[nIdx]

			rgn.SubtractDisk(pt.Add(p.getTiled(n.Sub(pt))), p.radius*4)

			nRgn, found := regions[nIdx]
			if found {
				nRgn.SubtractDisk(n.Add(p.getTiled(pt.Sub(n))), p.radius*2)

				if nRgn.IsEmpty() {
					delete(regions, nIdx)
					regionsPDF.Remove(nIdx)
				} else {
					regionsPDF.Update(nIdx, nRgn.Area)
				}
			}
		}

		p.addPoint(pt)

		if !rgn.IsEmpty() {
			regions[len(p.points)-1] = rgn
			regionsPDF.Insert(len(p.points)-1, rgn.Area)
		}
	}
}

type LinearPureSampler struct {
	*Sampler
}

func NewLinearPureSampler(radius float64) *LinearPureSampler {
	return &LinearPureSampler{
		Sampler: NewSampler(radius, true, true),
	}
}

func (l *LinearPureSampler) Complete() {
	var candidates []int

	l.addPoint(l.randomPoint())
	candidates = append(candidates, len(l.points)-1)

	for len(candidates) > 0 {
		c := rand.Int() % len(candidates)
		index := candidates[c]
		candidate := l.points[index]
		candidates[c] = candidates[len(candidates)-1]
		candidates = candidates[:len(candidates)-1]

		sr := NewScallopedRegion(candidate, l.radius*2, l.radius*4, SCALLOPED_REGION_MIN_AREA)
		l.findNeighbors(candidate, l.radius*8)

		for _, nIdx := range l.neighbors {
			n := &l.points[nIdx]
			nClose := candidate.Add(l.getTiled(n.Sub(candidate)))

			if nIdx < index {
				sr.SubtractDisk(nClose, l.radius*4)
			} else {
				sr.SubtractDisk(nClose, l.radius*2)
			}
		}

		for !sr.IsEmpty() {
			p := sr.Sample()
			pt := l.getTiled(p)

			l.addPoint(pt)
			candidates = append(candidates, len(l.points)-1)

			sr.SubtractDisk(p, l.radius*2)
		}
	}
}

type PenroseQuasiSampler struct {
	*QuasiSampler
	val int
}

func NewPenroseQuasiSampler(val int) *PenroseQuasiSampler {
	p := &PenroseQuasiSampler{
		val: val,
	}
	p.QuasiSampler = NewQuasiSampler(100, 100, p.GetImportanceAt)
	return p
}

func (p *PenroseQuasiSampler) GetImportanceAt(pt f64.Vec2) int {
	return p.val
}

type PenroseSampler struct {
	*Sampler
}

func NewPenroseSampler(radius float64) *PenroseSampler {
	return &PenroseSampler{
		Sampler: NewSampler(radius, false, false),
	}
}

func (p *PenroseSampler) Complete() {
	s := NewPenroseQuasiSampler(int(9.1 / (p.radius * p.radius)))
	pts := s.GetSamplingPoints()

	for i := range pts {
		pt := f64.Vec2{
			pts[i].X/50 - 1,
			pts[i].Y/50 - 1,
		}
		if p.pointInDomain(pt) {
			p.addPoint(pt)
		}
	}
}

type UniformSampler struct {
	*Sampler
}

func NewUniformSampler(radius float64) *UniformSampler {
	return &UniformSampler{
		Sampler: NewSampler(radius, false, true),
	}
}

func (u *UniformSampler) Complete() {
	N := int(.75 / (u.radius * u.radius))
	for i := 0; i < N; i++ {
		u.addPoint(u.randomPoint())
	}
}

func OutsideRadius(p []f64.Vec2, r float64) bool {
	w := true
	sort.Slice(p, func(i, j int) bool {
		a := p[i].Len()
		b := p[j].Len()
		if math.Abs(a-b) < r {
			w = false
		}
		return a < b
	})
	return w
}

func PairDistance(p []f64.Vec2) [][]float64 {
	d := make([][]float64, len(p))
	for i := range d {
		d[i] = make([]float64, len(p))
	}
	for i := range d {
		for j := range d {
			d[i][j] = p[i].Distance(p[j])
		}
	}
	return d
}
