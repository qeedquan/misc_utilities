package main

import (
	"errors"
	"fmt"
	"math/rand"
	"time"
)

func main() {
	rand.Seed(time.Now().UnixNano())

	opt := &Option{
		Devices:    100,
		Iterations: 1e4,
		Duration:   1 * time.Second,
		Speed:      2500,
	}
	rng := &RandChannel{}
	sim := &Sim1{}
	_, stats := runSims(sim, rng, opt)
	fmt.Println(stats)
}

var (
	ErrCollision   = errors.New("collision")
	ErrInvalidPort = errors.New("invalid port")
	ErrInvalidAddr = errors.New("invalid address")
	ErrTimeout     = errors.New("timeout")
)

type Option struct {
	Devices    int
	Iterations int
	Duration   time.Duration
	Speed      uint64
}

type Channel interface {
	Init(*Option)
	ReadReg(int, int, time.Duration) (int, time.Duration, error)
	WriteReg(int, int, int, time.Duration) (time.Duration, error)
}

type RandChannel struct {
	id   []int
	port []int
	mute []bool
	tau  time.Duration
}

func (c *RandChannel) Init(opt *Option) {
	c.id = make([]int, opt.Devices)
	c.port = make([]int, len(c.id))
	c.mute = make([]bool, len(c.id))
	for i := range c.id {
		c.id[i] = i + 1
		c.port[i] = i + 1
	}
	c.tau = (1 * time.Second) / time.Duration(opt.Speed)
}

func (c *RandChannel) lookup(tab []int, val int) int {
	for i := range tab {
		if tab[i] == val {
			return i
		}
	}
	return -1
}

func (c *RandChannel) randID(muteAllowed bool) int {
	var pool []int
	for i := range c.id {
		if muteAllowed || !c.mute[i] {
			pool = append(pool, c.id[i])
		}
	}
	if len(pool) == 0 {
		return -1
	}
	return pool[rand.Intn(len(pool))]
}

func (c *RandChannel) ReadReg(port, addr int, timeout time.Duration) (val int, elapsed time.Duration, err error) {
	if c.tau >= timeout {
		return -1, timeout, ErrTimeout
	}
	elapsed = c.tau

	val = -1
	switch addr {
	case 0:
		if port == 0 {
			if prob := 1 / float64(len(c.id)+1); rand.Float64() < prob {
				err = ErrCollision
			} else {
				val = c.randID(false)
			}
		} else if idx := c.lookup(c.port, port); idx >= 0 {
			val = c.id[idx]
		}

		if val < 0 && err == nil {
			err = ErrInvalidPort
		}

	default:
		err = ErrInvalidAddr
	}

	return
}

func (c *RandChannel) WriteReg(port, addr, val int, timeout time.Duration) (elapsed time.Duration, err error) {
	elapsed = c.tau
	if elapsed >= timeout {
		elapsed = timeout
		err = ErrTimeout
	}

	switch addr {
	case 0:
		if port == 0 {
			if val == 0 {
				for i := range c.mute {
					c.mute[i] = false
				}
			} else if idx := c.lookup(c.id, val&0xffff); idx >= 0 {
				c.port[idx] = (val >> 16) & 0xffff
				c.mute[idx] = true
			}
		}

	default:
		err = ErrInvalidAddr
	}

	return
}

type Sim interface {
	Run(Channel, *Option) map[string]int
}

type Sim1 struct {
	ptt   time.Duration
	att   time.Duration
	devs  map[int]int
	stats map[string]int
}

func (s *Sim1) Run(ch Channel, opt *Option) map[string]int {
	s.ptt = 3 * opt.Duration / 4
	s.att = 1 * opt.Duration / 4
	s.devs = make(map[int]int)
	s.stats = make(map[string]int)

	ch.WriteReg(0, 0, 0, 100*time.Millisecond)

	for t := time.Duration(0); t < s.ptt; {
		dev, dt, err := ch.ReadReg(0, 0, 1*time.Second)
		t += dt

		s.stats["probe_total"]++
		if err != nil {
			s.stats["probe_miss"]++
			continue
		}

		if _, found := s.devs[dev]; !found {
			s.devs[dev] = 0x10 + len(s.devs)
		}
	}

loop:
	for t := time.Duration(0); ; {
		for id, port := range s.devs {
			dt, _ := ch.WriteReg(0, 0, port<<16|id, 200*time.Millisecond)
			t += dt
			if t >= s.att {
				break loop
			}
		}
	}

	s.stats["assigned"] = 0
	for id, port := range s.devs {
		aid, _, _ := ch.ReadReg(port, 0, 1*time.Second)
		if id == aid {
			s.stats["assigned"]++
		}
	}

	if s.stats["assigned"] == len(s.devs) {
		s.stats["success"] = 1
	} else {
		s.stats["fail"] = 1
	}

	return s.stats
}

func runSims(sim Sim, ch Channel, opt *Option) (trials []map[string]int, stats map[string]int) {
	for i := 0; i < opt.Iterations; i++ {
		ch.Init(opt)
		trials = append(trials, sim.Run(ch, opt))
	}

	stats = make(map[string]int)
	stats["total_trials"] = len(trials)
	stats["total_probe_miss"] = sumMapInts(trials, "probe_miss")
	stats["total_probe_total"] = sumMapInts(trials, "probe_total")
	stats["total_fail"] = sumMapInts(trials, "fail")
	stats["total_success"] = sumMapInts(trials, "success")

	return
}

func sumMapInts(m []map[string]int, key string) int {
	s := 0
	for i := range m {
		s += m[i][key]
	}
	return s
}

func averageMapInts(m []map[string]int, key string) float64 {
	return float64(sumMapInts(m, key)) / float64(len(m))
}
