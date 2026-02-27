// Use resource hiearchy to solve the dining philosopher's problem
package main

import (
	"flag"
	"fmt"
	"math/rand"
	"os"
	"strconv"
	"sync"
	"time"
)

// struct that represents a philosopher
type Philosopher struct {
	// their id
	Id int

	// chopstick, lower id to higher id
	Chopsticks [2]*sync.Mutex
}

// philosophers
var (
	philosophers    []Philosopher
	numphilosophers = 5

	maxsleeptime = flag.Duration("d", 1*time.Second, "sleep time before next tick")
)

func main() {
	flag.Parse()
	if flag.NArg() >= 1 {
		n, err := strconv.Atoi(flag.Arg(0))
		if err != nil || n <= 0 {
			usage()
		}
		numphilosophers = n
	}

	// make new philosophers
	philosophers = NewPhilosophers(numphilosophers)

	// do the loop
	for i := range philosophers {
		p := &philosophers[i]
		go p.Run()
	}

	// sleep forever
	select {}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] num_philosophers")
	flag.PrintDefaults()
	os.Exit(2)
}

// make a new set of philosophers
func NewPhilosophers(n int) []Philosopher {
	p := make([]Philosopher, n)
	l := len(p)

	// these represents the chopstick mutexes
	// that the philosophers will share
	// since it is a round table, they are linked
	// together, link lower ids to higher first
	ch1 := new(sync.Mutex)
	for i := 0; i < l; i++ {
		ch2 := new(sync.Mutex)

		p[i].Id = i + 1
		p[i].Chopsticks[0] = ch1
		p[i].Chopsticks[1] = ch2
		ch1 = ch2
	}

	// link the last one with the first one, and reverse
	// the id, since first should be smaller than the second to
	// last id
	if l > 1 {
		p[l-1].Chopsticks[1] = p[l-1].Chopsticks[0]
		p[l-1].Chopsticks[0] = p[0].Chopsticks[0]
	}

	return p
}

// sleep for a random amount of time
func randsleep(max time.Duration) {
	d := (time.Duration(rand.Int()) * time.Millisecond) % max
	if d < 0 {
		d += max
	}
	time.Sleep(d)
}

// the run function
func (p *Philosopher) Run() {
	for {
		// wait for a while before eating
		randsleep(*maxsleeptime)
		fmt.Printf("Philosopher %v is hungry.\n", p.Id)

		// lower to higher id lock
		p.Chopsticks[0].Lock()
		p.Chopsticks[1].Lock()

		// got 2 chopsticks, he can eat now
		fmt.Printf("Philosopher %v has started to eat.\n", p.Id)
		randsleep(*maxsleeptime)
		fmt.Printf("Philosopher %v has stopped to eating and is now thinking.\n", p.Id)

		// higher to lower id unlock
		p.Chopsticks[1].Unlock()
		p.Chopsticks[0].Unlock()
	}
}
