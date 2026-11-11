// an echo server, writes back to client what client sent it
package main

import (
	"bufio"
	"flag"
	"io"
	"log"
	"net"
	"os"
	"strings"
	"sync"
	"sync/atomic"
	"time"
)

var options struct {
	Discard    bool
	MaxConns   int
	MaxRetries int
	Net        string
	Addr       string
	Log        string
	Timeout    time.Duration
}

var (
	logger Logger
)

func main() {
	flag.BoolVar(&options.Discard, "discard", false, "discard reads (no writes to client)")
	flag.IntVar(&options.MaxConns, "con", 10, "max number of connections, use 0 for no limit")
	flag.IntVar(&options.MaxRetries, "retries", 5, "max number of error retries to client before closing client")
	flag.StringVar(&options.Net, "net", "tcp", "network service to use [tcp, tcp4, tcp6, unix, unixpacket, ip, ip4, ip6, udp, udp4, udp6, unixgram]")
	flag.StringVar(&options.Addr, "addr", ":12321", "binding address")
	flag.StringVar(&options.Log, "log", "", "log file")
	flag.DurationVar(&options.Timeout, "timeout", 5*time.Second, "connection timeout")
	flag.Parse()

	if options.MaxConns < 0 {
		logger.Fatal("invalid max number of connections, must be a positive number")
	}
	if options.MaxRetries < 0 {
		logger.Fatal("invalid max number of retries, must be a positive number")
	}

	err := logger.Init(options.Log)
	if err != nil {
		logger.Warnln(err)
	}

	ticker := time.Tick(5 * time.Second)
	go func() {
		for {
			select {
			case <-ticker:
				logger.Flush()
			}
		}
	}()

	if isPacketNetwork(options.Net) {
		packetListen()
	} else {
		streamListen()
	}
}

func isPacketNetwork(network string) bool {
	switch {
	case strings.HasPrefix(network, "ip"):
		return true
	case strings.HasPrefix(network, "udp"):
		return true
	case strings.HasPrefix(network, "unixgram"):
		return true
	}
	return false
}

func mtuSize() int {
	intf, err := net.Interfaces()
	if err != nil || len(intf) == 0 {
		return 8192
	}
	mtu := intf[0].MTU
	for _, i := range intf[1:] {
		if i.MTU > mtu {
			mtu = i.MTU
		}
	}
	return mtu
}

func packetListen() {
	logger.Printf("packet listener on options %v!%v\n", options.Net, options.Addr)

	conn, err := net.ListenPacket(options.Net, options.Addr)
	if err != nil {
		logger.Fatal(err)
	}

	buf := make([]byte, mtuSize())
	for {
		n, addr, err := conn.ReadFrom(buf)
		if err != nil {
			logger.Warnln(err)
		}
		go packetServe(conn, addr, buf[:n])
	}
}

func packetServe(conn net.PacketConn, addr net.Addr, buf []byte) {
	logger.Printf("got packet connection from %v\n", addr)
	conn.SetDeadline(time.Now().Add(options.Timeout))
	_, err := conn.WriteTo(buf, addr)
	if err != nil {
		logger.Warnf("failed to write to %v: %v\n", addr, err)
	}
}

func streamListen() {
	ln, err := net.Listen(options.Net, options.Addr)
	if err != nil {
		logger.Fatal(err)
	}

	logger.Printf("stream listener on options %v!%v\n", options.Net, options.Addr)

	ln = NewLimitedListener(ln, options.MaxConns)
	numConns := uint64(0)
	for {
		conn, err := ln.Accept()
		if err != nil {
			logger.Warnln(err)
			continue
		}
		atomic.AddUint64(&numConns, 1)
		go streamServe(conn, &numConns)
	}
}

func streamServe(conn net.Conn, numConns *uint64) {
	logger.Printf("serving %v (%v connections total)", conn.RemoteAddr(), atomic.LoadUint64(numConns))

	conn.SetDeadline(time.Now().Add(options.Timeout))
	retries := 0
	for {
		output := io.Discard
		if !options.Discard {
			output = conn
		}
		_, err := io.Copy(output, conn)
		if err != nil {
			ne, ok := err.(net.Error)
			if ok && ne.Temporary() {
				conn.SetDeadline(time.Now().Add(options.Timeout))
				if options.MaxRetries > 0 {
					if retries++; retries > options.MaxRetries {
						break
					}
				}

				continue
			}
			logger.Println(err)
			break
		}
	}

	logger.Printf("closing connection to %v (%v connections total)",
		conn.RemoteAddr(), atomic.AddUint64(numConns, ^uint64(0)))
	conn.Close()
}

type Logger struct {
	mu     sync.Mutex
	stderr *log.Logger
	stdout *log.Logger
	w      *bufio.Writer
	f      *os.File
}

func (lg *Logger) Init(name string) error {
	lg.stderr = log.New(os.Stderr, "", log.LstdFlags)
	lg.stdout = log.New(os.Stdout, "", log.LstdFlags)

	if name == "" {
		return nil
	}

	f, err := os.OpenFile(name, os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		return err
	}

	lg.w = bufio.NewWriter(f)
	lg.stderr.SetOutput(io.MultiWriter(os.Stderr, lg.w))
	lg.stdout.SetOutput(io.MultiWriter(os.Stdout, lg.w))
	lg.f = f

	return nil
}

func (lg *Logger) Warnf(format string, args ...interface{}) {
	lg.mu.Lock()
	lg.stderr.Printf(format, args...)
	lg.mu.Unlock()
}

func (lg *Logger) Warnln(args ...interface{}) {
	lg.mu.Lock()
	lg.stderr.Println(args...)
	lg.mu.Unlock()
}

func (lg *Logger) Fatal(args ...interface{}) {
	lg.mu.Lock()
	lg.stderr.Fatal(args...)
	lg.mu.Unlock()
}

func (lg *Logger) Println(args ...interface{}) {
	lg.mu.Lock()
	lg.stdout.Println(args...)
	lg.mu.Unlock()
}

func (lg *Logger) Printf(format string, args ...interface{}) {
	lg.mu.Lock()
	lg.stdout.Printf(format, args...)
	lg.mu.Unlock()
}

func (lg *Logger) Flush() {
	if lg.w != nil {
		lg.mu.Lock()
		lg.w.Flush()
		lg.mu.Unlock()
	}
}

type Semaphore chan struct{}

func NewSemaphore(n int) Semaphore {
	sem := make(chan struct{}, n)
	for i := 0; i < n; i++ {
		sem <- struct{}{}
	}
	return Semaphore(sem)
}

func (s Semaphore) Acquire() { <-s }
func (s Semaphore) Release() { s <- struct{}{} }

type LimitedListener struct {
	net.Listener
	sem Semaphore
}

func NewLimitedListener(l net.Listener, max int) net.Listener {
	if max <= 0 {
		return l
	}

	return &LimitedListener{
		Listener: l,
		sem:      NewSemaphore(max),
	}
}

func (ln *LimitedListener) Accept() (net.Conn, error) {
	ln.sem.Acquire()
	conn, err := ln.Listener.Accept()
	if err != nil {
		ln.sem.Release()
	}
	return &LimitedConn{Conn: conn, sem: ln.sem}, err
}

type LimitedConn struct {
	net.Conn
	sem Semaphore
}

func (c *LimitedConn) Close() error {
	c.sem.Release()
	return c.Conn.Close()
}
