/*

Given a set of lock operations, simulate them to see if they deadlock.
This program relies on the Go deadlock detector to do the hard work.

*/

package main

import (
	"encoding/csv"
	"flag"
	"fmt"
	"log"
	"os"
	"runtime"
	"strings"
	"sync"
)

type Op struct {
	cmd int
	id  string
}

type Lock struct {
	*sync.Mutex
	active bool
}

type Lockstore struct {
	store map[string]*Lock
}

type Sim struct {
	iterations int
	lockstore  *Lockstore
	wg         *sync.WaitGroup
	verbose    bool
}

var flags struct {
	iterations int
	verbose    bool
}

func main() {
	log.SetFlags(0)
	log.SetPrefix("lock-orderings: ")

	parseflags()

	ops, err := getops(flag.Arg(0))
	check(err)

	c := Sim{
		iterations: flags.iterations,
		lockstore:  newlockstore(ops),
		wg:         new(sync.WaitGroup),
		verbose:    flags.verbose,
	}
	c.Run(ops)
}

func check(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] lockops.csv")
	flag.PrintDefaults()
	os.Exit(2)
}

func parseflags() {
	flag.IntVar(&flags.iterations, "iterations", 1e6, "number of iterations")
	flag.BoolVar(&flags.verbose, "verbose", true, "be verbose")

	flag.Usage = usage
	flag.Parse()
	if flag.NArg() != 1 {
		usage()
	}
}

func getops(name string) ([][]Op, error) {
	f, err := os.Open(name)
	if err != nil {
		return nil, err
	}
	defer f.Close()

	csvr := csv.NewReader(f)
	records, err := csvr.ReadAll()
	if err != nil {
		return nil, err
	}

	if len(records) == 0 || len(records[0]) == 0 {
		return nil, nil
	}

	rows, cols := len(records), len(records[0])
	ops := make([][]Op, cols)
	for i := range ops {
		ops[i] = make([]Op, rows)
	}

	for r := 0; r < rows; r++ {
		for c := 0; c < cols; c++ {
			val := strings.TrimSpace(records[r][c])
			if val == "" {
				continue
			}

			ops[c][r] = Op{
				cmd: int(val[0]),
				id:  val[1:],
			}
		}
	}

	return ops, nil
}

func newlockstore(ops [][]Op) *Lockstore {
	l := &Lockstore{
		store: make(map[string]*Lock),
	}
	for i := range ops {
		for _, op := range ops[i] {
			l.store[op.id] = &Lock{
				Mutex:  new(sync.Mutex),
				active: false,
			}
		}
	}
	return l
}

func (l *Lockstore) Reset() {
	for _, m := range l.store {
		if m.active {
			m.Unlock()
			m.active = false
		}
	}
}

func (l *Lockstore) Lock(id string) {
	m := l.store[id]
	m.Lock()
	m.active = true
}

func (l *Lockstore) Unlock(id string) {
	m := l.store[id]
	m.active = false
	m.Unlock()
}

func (c *Sim) Run(ops [][]Op) {
	l := c.lockstore
	for n := 0; n < c.iterations; n++ {
		c.print("Iteration %d\n", n+1)

		l.Reset()
		c.wg.Add(len(ops))
		for i := range ops {
			go c.exec(i+1, ops[i])
		}
		c.wg.Wait()

		c.print("\n")
	}
}

func (c *Sim) exec(thread int, ops []Op) {
	l := c.lockstore
	for _, op := range ops {
		c.print("Thread %d: %c%s\n", thread, op.cmd, op.id)
		switch op.cmd {
		case '+':
			l.Lock(op.id)
		case '-':
			l.Unlock(op.id)
		}
		runtime.Gosched()
	}
	c.wg.Done()
}

func (c *Sim) print(format string, args ...any) {
	if c.verbose {
		fmt.Printf(format, args...)
	}
}
