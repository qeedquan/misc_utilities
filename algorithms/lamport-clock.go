/*

https://en.wikipedia.org/wiki/Lamport_timestamp
https://martinfowler.com/articles/patterns-of-distributed-systems/lamport-clock.html
https://sookocheff.com/post/time/lamport-clock/
https://lamport.azurewebsites.net/pubs/time-clocks.pdf
https://en.wikipedia.org/wiki/Vector_clock

Lamport clock uses logical timestamps as a version for a value to allow ordering of values across servers
Vector clocks are one generalization of lamport clocks (uses N logical clocks instead of 1)

Algorithm for sending can be expressed as:

# event happens
time = time + 1;
send(message, time);

And the algorithm for receiving a message as:

(message, incoming_time) = receive();
time = max(incoming_time, time) + 1;

This gives one possible total ordering of events for servers that talks to each other for a particular sequence of events.

Since these clocks uses logical timestamps to order events, there is a limitation to note:
Given a set of computers with parallel running sequences of events using multiple lamport clocks to order it,
the lamport clocks cannot discern ordering relative to each other.

Example:
(A, B, C) are 3 servers running 2 sequence of events in parallel A and B.
Use lamport clocks denoted LCA and LCB to order the events.

The timestamps in LCA cannot be used to deduce any event that happens in LCB and vice versa.
Also, the timestamps in LCA cannot be used to compare against the timestamps in LCB.
More succinctly, a total ordering cannot be constructed using LCA and LCB that gives
the total ordering of one lamport clock LCAB that tracks the order events for sequence A and B together.

*/

package main

import (
	"fmt"
	"math/rand"
	"time"
)

func main() {
	nthread := 5
	chans := make([]chan [2]int, nthread)
	for id := range nthread {
		chans[id] = make(chan [2]int)
		go worker(chans, id)
	}
	select {}
}

func worker(chans []chan [2]int, id int) {
	clock := LamportClock{}
	duration := time.Duration(rand.Intn(500)+1) * time.Millisecond
	ticker := time.NewTicker(duration)
	for {
		select {
		case msg := <-chans[id]:
			curtime := clock.Receive(msg[1])
			fmt.Println(id, "|", msg[0], curtime)

		case <-ticker.C:
			duration = time.Duration(rand.Intn(500)+1) * time.Millisecond
			ticker.Reset(duration)

			tid := rand.Intn(len(chans))
			if tid == id {
				tid = (tid + 1) % len(chans)
			}
			curtime := clock.Send()
			chans[tid] <- [2]int{id, curtime}
		}
	}
}

type LamportClock struct {
	time int
}

func (c *LamportClock) Send() int {
	c.time++
	return c.time
}

func (c *LamportClock) Receive(time int) int {
	c.time = max(c.time, time) + 1
	return c.time
}
