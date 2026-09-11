package main

import (
	"bytes"
	"crypto/rand"
	"encoding/binary"
	"flag"
	"fmt"
	"log"
	"net"
	"os"
	"time"
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("dhcp-discover: ")

	var opt option
	flag.DurationVar(&opt.timeout, "t", 3*time.Second, "set timeout")
	flag.BoolVar(&opt.randmac, "r", false, "use random mac address")

	flag.Usage = usage
	flag.Parse()

	err := discover(opt)
	if err != nil {
		log.Fatal(err)
	}
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: dhcp-discover [options]")
	flag.PrintDefaults()
	os.Exit(2)
}

type option struct {
	timeout time.Duration
	randmac bool
}

type result struct {
	laddr net.Addr
	raddr net.Addr
	found bool
}

type message struct {
	Op     uint8
	Htype  uint8
	Hlen   uint8
	Hops   uint8
	Xid    uint32
	Secs   uint16
	Flags  uint16
	Ciaddr uint32
	Yiaddr uint32
	Siaddr uint32
	Giaddr uint32
	Chaddr [16]byte
	Magic  uint32
	Dtype  uint8
	Cid    [16]byte
	Mark   uint8
}

func discover(opt option) error {
	err := sendpacket(opt)
	if err != nil {
		return err
	}

	var lns []*net.UDPConn
	ln, err := net.ListenUDP("udp4", &net.UDPAddr{
		IP:   net.IPv4zero,
		Port: 68,
	})
	if err != nil {
		return err
	}
	lns = append(lns, ln)

	ch := make(chan result)
	for _, ln := range lns {
		go fetch(opt, ln, ch)
	}

	var res []result
	for range lns {
		r := <-ch
		if !r.found {
			continue
		}
		res = append(res, r)
	}
	if len(res) == 0 {
		return fmt.Errorf("no response received")
	}

	for i, r := range res {
		fmt.Printf("Response %v of %v", i+1, len(res))
		fmt.Printf("Local address %v\n", r.laddr)
		fmt.Printf("Remote address %v\n", r.raddr)
	}

	return nil
}

func rand4() uint32 {
	var p [4]byte
	rand.Read(p[:])
	return uint32(p[0]) | uint32(p[1])<<8 |
		uint32(p[2])<<16 | uint32(p[3])<<24
}

func genmac() (mac [6]byte, err error) {
	_, err = rand.Read(mac[:])
	return
}

func sendpacket(opt option) error {
	conn, err := net.DialUDP("udp4",
		&net.UDPAddr{
			IP:   net.IPv4zero,
			Port: 68,
		},
		&net.UDPAddr{
			IP:   net.IPv4bcast,
			Port: 67,
		},
	)
	if err != nil {
		return err
	}
	defer conn.Close()

	mac := [6]byte{0xde, 0xad, 0xc0, 0xde, 0xca, 0xfe}
	if opt.randmac {
		var err error
		mac, err = genmac()
		if err != nil {
			return err
		}
	}

	msg := message{
		Op:    1,
		Htype: 1,
		Hlen:  6,
		Xid:   rand4(),
		Secs:  0,
		Flags: 0x8000,
		Magic: 0x63825363,
		Dtype: 1,
		Mark:  0xff,
	}
	copy(msg.Chaddr[:], mac[:])
	copy(msg.Cid[:], mac[:])

	buf := new(bytes.Buffer)
	binary.Write(buf, binary.BigEndian, msg)
	_, err = conn.Write(buf.Bytes())

	return err
}

func fetch(opt option, ln *net.UDPConn, ch chan result) {
	r := result{
		laddr: ln.LocalAddr(),
		found: true,
	}

	defer func() {
		ln.Close()
		ch <- r
	}()

	ln.SetReadDeadline(time.Now().Add(opt.timeout))

	buf := make([]byte, 4096)
	_, raddr, err := ln.ReadFromUDP(buf)
	if err != nil {
		r.found = false
		return
	}
	r.raddr = raddr

	return
}
