// ported from suckless tftp
package main

import (
	"flag"
	"fmt"
	"io"
	"log"
	"net"
	"os"
	"strings"
	"time"
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("tftp: ")

	var host, port string
	var mode rune
	var timeout time.Duration
	var retries int

	for _, p := range modes {
		m := &modeFlag{&mode, p.flag}
		flag.Var(m, string(p.flag), p.usage)
	}
	flag.StringVar(&host, "h", "", "host")
	flag.StringVar(&port, "p", "tftp", "port")
	flag.DurationVar(&timeout, "t", 5*time.Second, "receive timeout")
	flag.IntVar(&retries, "r", 5, "number of retries")
	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	c, err := NewClient(host, port, os.Stdin, os.Stdout, timeout, retries)
	ck(err)
	defer c.Close()

	name := flag.Arg(0)
	if mode == 'c' {
		err = c.Put(name)
	} else {
		err = c.Get(name)
	}
	ck(err)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] file")
	flag.PrintDefaults()
	os.Exit(1)
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

type modeFlag struct {
	bind *rune
	mode rune
}

func (modeFlag) String() string   { return "" }
func (modeFlag) IsBoolFlag() bool { return true }
func (m *modeFlag) Set(string) error {
	*m.bind = m.mode
	return nil
}

var modes = []struct {
	flag  rune
	usage string
}{
	{'c', "put file"},
	{'x', "get file"},
}

const (
	RRQ = 1 + iota
	WWQ
	DATA
	ACK
	ERR
)

const (
	BLKSIZE = 512
	HDRSIZE = 4
	PKTSIZE = BLKSIZE + HDRSIZE
)

type tftpError struct {
	Code int
	Msg  string
}

func (e tftpError) String() string {
	errs := []string{
		"Undefined",
		"File not found",
		"Access violation",
		"Disk full or allocation exceeded",
		"Illegal TFTP operation",
		"Unknown transfer ID",
		"File already exists",
		"No such user",
	}

	if e.Msg != "" {
		return fmt.Sprintf("(%d) %s", e.Code, e.Msg)
	}

	if 0 <= e.Code && e.Code < len(errs) {
		return errs[e.Code]
	}

	return "Unknown error"
}

type Client struct {
	*net.UDPConn
	to      net.Addr
	stdin   io.Reader
	stdout  io.Writer
	timeout time.Duration
	retries int
	trie    int
}

func NewClient(host, port string, stdin io.Reader, stdout io.Writer, timeout time.Duration, retries int) (*Client, error) {
	laddr, err := net.ResolveUDPAddr("udp", ":0")
	if err != nil {
		return nil, err
	}

	raddr, err := net.ResolveUDPAddr("udp", fmt.Sprintf("%s:%s", host, port))
	if err != nil {
		return nil, err
	}

	conn, err := net.ListenUDP("udp", laddr)
	if err != nil {
		return nil, err
	}

	return &Client{
		UDPConn: conn,
		to:      raddr,
		stdin:   stdin,
		stdout:  stdout,
		timeout: timeout,
		retries: retries,
	}, nil
}

func (c *Client) Put(name string) (err error) {
	defer func() {
		if e, _ := recover().(error); e != nil {
			err = e
		}
	}()

	var inb, outb, blkno int
	inbuf := make([]byte, PKTSIZE)
	outbuf := make([]byte, PKTSIZE)
	state := WWQ
	done := false
	nextblkno := 1
	for {
		switch state {
		case WWQ:
			outb = c.packreq(outbuf, WWQ, name, "octet")
			c.writepkt(outbuf[:outb])
			inb := c.readpkt(inbuf)
			if inb > 0 {
				op := c.unpackop(inbuf)
				if op != ACK && op != ERR {
					errf("bad opcode: %d\n", op)
				}
				state = op
			}

		case DATA:
			if blkno == nextblkno {
				nextblkno++
				c.packdata(outbuf, nextblkno)
				outb, _ = c.stdin.Read(outbuf[HDRSIZE:])
				if outb < BLKSIZE {
					done = true
				}
			}
			c.writepkt(outbuf[:HDRSIZE+outb])
			inb = c.readpkt(inbuf)
			if inb > 0 {
				op := c.unpackop(inbuf)
				if op != ACK && op != ERR {
					errf("bad opcode: %d", op)
				}
				state = op
			}

		case ACK:
			if inb < HDRSIZE {
				errf("truncated packet")
			}
			blkno = c.unpackblkno(inbuf)
			if blkno == nextblkno {
				if done {
					return
				}
				state = DATA
			}

		case ERR:
			errf("error: %s", c.unpackerrc(inbuf))
		}
	}
}

func (c *Client) Get(name string) (err error) {
	defer func() {
		if e, _ := recover().(error); e != nil {
			err = e
		}
	}()

	var n, blkno int
	buf := make([]byte, PKTSIZE)
	state := RRQ
	nextblkno := 1
	done := false
	for {
		switch state {
		case RRQ:
			n = c.packreq(buf, RRQ, name, "octet")
			c.writepkt(buf[:n])
			n = c.readpkt(buf)
			if n > 0 {
				op := c.unpackop(buf)
				if op != DATA && op != ERR {
					errf("bad opcode: %d", op)
				}
				state = op
			}

		case DATA:
			n -= HDRSIZE
			if n < 0 {
				errf("truncated packet")
			}

			blkno = c.unpackblkno(buf)
			if blkno == nextblkno {
				nextblkno++
				c.stdout.Write(buf[HDRSIZE : HDRSIZE+n])
			}
			if n < BLKSIZE {
				done = true
			}
			state = ACK

		case ACK:
			n = c.packack(buf, blkno)
			c.writepkt(buf[:n])
			if done {
				return
			}

			n = c.readpkt(buf)
			if n > 0 {
				op := c.unpackop(buf)
				if op != DATA && op != ERR {
					errf("bad opcode: %d", op)
				}
				state = op
			}

		case ERR:
			errf("error: %s", c.unpackerrc(buf))
		}
	}
}

func (c *Client) packreq(p []byte, op int, path, mode string) int {
	if len(path)+1 > 256 {
		errf("filename too long")
	}

	i := 0
	p[i], i = byte(op>>8), i+1
	p[i], i = byte(op&0xff), i+1
	copy(p[i:], path)
	i += len(path)
	p[i], i = 0, i+1
	copy(p[i:], mode)
	i += len(mode)
	p[i], i = 0, i+1
	return i
}

func (c *Client) packack(p []byte, blkno int) int {
	p[0] = ACK >> 8
	p[1] = ACK & 0xff
	p[2] = byte(blkno >> 8)
	p[3] = byte(blkno & 0xff)
	return 4
}

func (c *Client) packdata(p []byte, blkno int) {
	p[0] = DATA >> 8
	p[1] = DATA & 0xff
	p[2] = byte(blkno >> 8)
	p[3] = byte(blkno & 0xff)
}

func (c *Client) unpackop(p []byte) int {
	return int(p[0])<<8 | int(p[1])&0xff
}

func (c *Client) unpackblkno(p []byte) int {
	return int(p[2])<<8 | int(p[3])&0xff
}

func (c *Client) unpackerrc(p []byte) tftpError {
	return tftpError{
		Code: int(p[2])<<8 | int(p[3])&0xff,
		Msg:  strings.TrimRight(string(p[4:]), "\x00"),
	}
}

func (c *Client) writepkt(p []byte) int {
	n, err := c.WriteTo(p, c.to)
	if err != nil {
		panic(err)
	}
	if n != len(p) {
		panic(io.ErrShortWrite)
	}
	return n
}

func (c *Client) readpkt(p []byte) int {
	c.SetReadDeadline(time.Now().Add(c.timeout))
	n, addr, err := c.ReadFrom(p)
	if err != nil {
		if ne, _ := err.(net.Error); ne != nil && ne.Timeout() {
			if c.trie++; c.trie >= c.retries {
				errf("transfer timed out")
			}
		}
	} else {
		c.to = addr
		c.trie = 0
	}
	return n
}

func errf(format string, args ...interface{}) {
	panic(fmt.Errorf(format, args...))
}
