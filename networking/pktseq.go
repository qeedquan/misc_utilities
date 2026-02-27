// send some packets see if we get it back and keep track of some stats
package main

import (
	"bytes"
	"crypto/rand"
	"encoding/binary"
	"flag"
	"fmt"
	"io"
	"log"
	"net"
	"os"
	"strings"
	"sync"
	"time"
)

var opt struct {
	network  string
	laddr    string
	raddr    []net.Addr
	interval time.Duration
	mtu      int
	sender   bool
}

func main() {
	parseopt()

	conn, err := net.ListenPacket(opt.network, opt.laddr)
	ck(err)
	defer conn.Close()
	log.Printf("local address: %v\n", conn.LocalAddr())

	pkt := newpkt(conn, opt.raddr, opt.interval, opt.mtu, opt.sender)
	pkt.Run()
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: pktseq [options] addr ...")
	flag.PrintDefaults()
	os.Exit(2)
}

func parseopt() {
	opt.sender = false

	flag.StringVar(&opt.network, "n", "udp", "use network protocol")
	flag.StringVar(&opt.laddr, "l", ":0", "listen on address")
	flag.BoolVar(&opt.sender, "s", opt.sender, "become a sender")
	flag.IntVar(&opt.mtu, "m", mtusize(), "mtu size")
	flag.DurationVar(&opt.interval, "i", 1*time.Second, "interval")

	flag.Usage = usage
	flag.Parse()
	if opt.mtu < 16 {
		log.Fatal("mtu size too small")
	}
	if opt.interval <= 0 {
		log.Fatal("invalid interval")
	}

	for _, addr := range flag.Args() {
		var raddr net.Addr
		var err error
		switch {
		case strings.HasPrefix(opt.network, "ip"):
			raddr, err = net.ResolveIPAddr(opt.network, addr)
		case strings.HasPrefix(opt.network, "udp"):
			raddr, err = net.ResolveUDPAddr(opt.network, addr)
		case strings.HasPrefix(opt.network, "unix"):
			raddr, err = net.ResolveUnixAddr(opt.network, addr)
		default:
			log.Fatal("unsupported network protocol", opt.network)
		}
		ck(err)
		opt.raddr = append(opt.raddr, raddr)
	}

	if opt.sender {
		if len(opt.raddr) == 0 {
			usage()
		}

		log.Printf("sender starting\n")
	} else {
		log.Printf("receiver starting\n")
	}
}

func mtusize() int {
	intf, err := net.Interfaces()
	if err != nil || len(intf) == 0 {
		return 128
	}

	mtu := intf[0].MTU
	for _, i := range intf[1:] {
		if mtu > i.MTU {
			mtu = i.MTU
		}
	}
	return mtu
}

func min(a, b int) int {
	if a < b {
		return a
	}
	return b
}

type Pkt struct {
	wg     sync.WaitGroup
	conn   net.PacketConn
	rbuf   []byte
	seq    []*Seq
	sender bool
	recv   uint64
}

type Seq struct {
	mu      sync.Mutex
	ticker  *time.Ticker
	wbuf    []byte
	cnt     uint64
	addr    net.Addr
	seen    map[uint64]uint64
	hist    map[uint64][]byte
	histlen uint64

	sent    uint64
	dups    uint64
	lostcnt uint64
	deadcnt uint64
	badcnt  uint64
	badbuf  uint64
	badsz   uint64
}

func newpkt(conn net.PacketConn, addr []net.Addr, interval time.Duration, mtu int, sender bool) *Pkt {
	p := &Pkt{
		conn:   conn,
		rbuf:   make([]byte, mtu),
		sender: sender,
	}
	for _, addr := range addr {
		p.seq = append(p.seq, newseq(addr, interval, mtu))
	}
	return p
}

func (p *Pkt) check(prefix string, err error) bool {
	if err != nil {
		log.Printf("%v: %v\n", prefix, err)
	}

	unrecoverable := err == io.EOF
	ne, ok := err.(net.Error)
	if ok {
		unrecoverable = !ne.Temporary()
	}
	return unrecoverable
}

func (p *Pkt) Run() {
	p.wg.Add(1)
	go p.read()

	if p.sender {
		for _, s := range p.seq {
			p.wg.Add(1)
			go p.write(s)
		}
	}

	p.wg.Wait()
}

func (p *Pkt) read() {
	defer p.wg.Done()

	for {
		n, addr, err := p.conn.ReadFrom(p.rbuf)
		p.recv += uint64(n)
		if p.check("", err) {
			break
		}
		if err != nil {
			continue
		}

		if p.sender {
			for _, s := range p.seq {
				if s.matchaddr(addr) {
					log.Printf("%v: received %v bytes total %v bytes\n", s.addr, n, p.recv)
					s.mu.Lock()
					s.checkbuf(p.rbuf[:n])
					s.mu.Unlock()
					break
				}
			}
		} else {
			log.Printf("write %v %v %v...\n", p.conn.LocalAddr(), p.recv, p.rbuf[:min(n, 16)])
			p.conn.WriteTo(p.rbuf[:n], addr)
		}
	}
	log.Printf("reader died\n")
}

func (p *Pkt) write(s *Seq) {
	defer p.wg.Done()

	for {
		<-s.ticker.C
		s.mu.Lock()
		s.dumpstats()
		s.cleanhist()
		s.fillbuf()
		s.mu.Unlock()

		log.Printf("%v: writing seq %v buf %v...\n", s.addr, s.cnt-1, s.wbuf[8:16])
		_, err := p.conn.WriteTo(s.wbuf, s.addr)
		if p.check(fmt.Sprintf("%v", s.addr), err) {
			break
		}
	}
	log.Printf("%v: writer died\n", s.addr)
}

func newseq(addr net.Addr, interval time.Duration, mtu int) *Seq {
	return &Seq{
		ticker:  time.NewTicker(interval),
		seen:    make(map[uint64]uint64),
		hist:    make(map[uint64][]byte),
		wbuf:    make([]byte, mtu),
		addr:    addr,
		histlen: 50,
	}
}

func (s *Seq) matchaddr(a net.Addr) bool {
	return s.addr.Network() == a.Network() && s.addr.String() == a.String()
}

func (s *Seq) checkbuf(b []byte) {
	if len(b) != len(s.wbuf) {
		s.badsz++
		return
	}

	cnt := binary.LittleEndian.Uint64(b)
	switch {
	case cnt > s.cnt: // sequence number newer than what we sent
		s.badcnt++
	case s.cnt-cnt >= s.histlen: // sequence number too old
		s.deadcnt++
	default:
		if s.seen[cnt] != 0 {
			// duplicates
			s.dups++
		}

		// validate integrity of data
		if bytes.Compare(s.hist[cnt], b) != 0 {
			s.badbuf++
		}
		s.seen[cnt]++
	}
}

func (s *Seq) fillbuf() {
	binary.LittleEndian.PutUint64(s.wbuf[0:], s.cnt)
	rand.Read(s.wbuf[8:])

	s.hist[s.cnt], s.cnt = append([]byte{}, s.wbuf...), s.cnt+1
	s.sent += uint64(len(s.wbuf))
}

func (s *Seq) cleanhist() {
	if uint64(len(s.hist)) < s.histlen {
		return
	}

	n := 0
	for k, _ := range s.hist {
		if s.cnt > s.histlen && s.cnt-s.histlen > k {
			if s.seen[k] == 0 {
				s.lostcnt++
			}
			delete(s.hist, k)
			n++
		}
	}
	log.Printf("%v: histsize %d reclaimed %d\n", s.addr, len(s.hist), n)
}

func (s *Seq) dumpstats() {
	b := new(bytes.Buffer)
	fmt.Fprintf(b, "\n")
	fmt.Fprintf(b, "%v sent %v bytes dups %v lostcnt %v deadcnt %v badsz %v badcnt %v badbuf %v\n",
		s.addr, s.sent, s.dups, s.lostcnt, s.deadcnt, s.badsz, s.badcnt, s.badbuf)
	log.Printf("%s\n", b.String())
}
