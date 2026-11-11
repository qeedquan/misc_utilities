package main

import (
	"bytes"
	"encoding/binary"
	"flag"
	"fmt"
	"log"
	"net"
	"os"
	"os/signal"
	"syscall"
	"time"
	"unsafe"

	"golang.org/x/sys/unix"
)

var (
	intf    = BDADDRANY
	size    = 44
	ident   = uint8(200)
	delay   = 1
	count   = -1
	timeout = 10
	flood   = false
	reverse = false
	verify  = false
	verbose = false

	sentpkt uint64
	recvpkt uint64
)

func main() {
	parseFlags()

	lbd, rbd := parseAddr(intf, flag.Arg(0))
	conn, err := DialL2CAP("l2cap", &lbd, &rbd)
	ck(err)
	defer conn.Close()

	sigch := make(chan os.Signal)
	signal.Notify(sigch, syscall.SIGINT)
	go func() {
		select {
		case <-sigch:
			loss := uint64(0)
			if sentpkt != 0 {
				loss = uint64(float64(sentpkt-recvpkt) / (float64(sentpkt) / 100))
			}
			fmt.Printf("%d sent, %d received, %d%% loss\n", sentpkt, recvpkt, loss)
			os.Exit(0)
		}
	}()

	ping(conn)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] <bdaddr>")
	flag.PrintDefaults()
	os.Exit(2)
}

func parseFlags() {
	flag.StringVar(&intf, "i", intf, "The command is applied to device hciX (X = 0, 1, 2, ...). If not specified, use first bluetooth device")
	flag.IntVar(&size, "s", size, "The size of the data packets to be sent")
	flag.IntVar(&count, "c", count, "Send count number of packets then exit")
	flag.IntVar(&timeout, "t", timeout, "Wait timeout seconds for the response")
	flag.IntVar(&delay, "d", delay, "Wait delay seconds between pings")
	flag.BoolVar(&flood, "f", flood, "Kind of flood ping. Use with care! It reduces the delay between packets to 0")
	flag.BoolVar(&reverse, "r", reverse, "Reverse ping (gnip?). Send echo response instead of echo request")
	flag.BoolVar(&verify, "v", verify, "Verify response payload is identical to request payload. It is not required, but most stacks implement this behavior")
	flag.BoolVar(&verbose, "V", verbose, "Add verbose prints")

	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 {
		usage()
	}

	if flood {
		delay = 0
	}
}

func parseAddr(laddr, raddr string) (lbd, rbd BD) {
	hci, err := LookupHCIDevice(laddr)
	if err == nil {
		lbd = BD(hci.BDAddr)
		for i := 0; i < len(lbd)/2; i++ {
			lbd[i], lbd[len(lbd)-1-i] = lbd[len(lbd)-1-i], lbd[i]
		}
	} else {
		lbd, err = ParseBD(laddr)
	}
	if err != nil {
		log.Fatalf("Failed to parse local address: %v", err)
	}

	rbd, err = ParseBD(raddr)
	if err != nil {
		log.Fatalf("Failed to parse remote address: %v", err)
	}

	return
}

func dprintf(format string, args ...interface{}) {
	if verbose {
		fmt.Printf(format, args...)
	}
}

func ping(c *L2CAPConn) {
	laddr := c.LocalAddr()
	raddr := c.RemoteAddr()

	wb := make([]byte, L2CAP_CMD_HDR_SIZE+size)
	rb := make([]byte, len(wb))

	for i := 0; i < size; i++ {
		wb[L2CAP_CMD_HDR_SIZE+i] = byte(i%40) + 'A'
	}

	fmt.Printf("Ping: %s from %s (data size %d) ...\n", raddr, laddr, size)
	id := ident
	for count != 0 {
		wb[0] = L2CAP_ECHO_REQ
		wb[1] = id
		binary.LittleEndian.PutUint16(wb[2:], uint16(size))
		dprintf("write: %v\n", wb)
		c.Write(wb)

		lost := false
		start := time.Now()
		for {
			n, err := c.Poll(time.Duration(timeout) * time.Second)
			if err == syscall.EINTR || err == syscall.EAGAIN {
				dprintf("poll: %v\n", err)
				continue
			}

			if err != nil {
				log.Fatalf("Poll Failed: %v", err)
			}
			if n == 0 {
				lost = true
				break
			}

			n, err = c.Read(rb)
			if err != nil {
				log.Fatalf("Recv failed: %v", err)
			}
			dprintf("read: %v\n", rb)
			if n == 0 {
				log.Fatal("Disconnected")
			}

			if rb[1] != id {
				continue
			}
			if !reverse && rb[0] == L2CAP_ECHO_RSP {
				break
			}
			if rb[1] == L2CAP_COMMAND_REJ {
				log.Fatal("Peer doesn't support Echo packets")
			}
		}
		sentpkt++

		if !lost {
			recvpkt++

			rcvlen := binary.LittleEndian.Uint16(rb[2:])
			if verify {
				if rcvlen != uint16(size) {
					log.Fatalf("Received %d bytes expected %d", rcvlen, size)
				}

				if bytes.Compare(wb[L2CAP_CMD_HDR_SIZE:], rb[L2CAP_CMD_HDR_SIZE:]) != 0 {
					log.Fatalf("Response payload different\n")
				}
			}

			fmt.Printf("%d bytes from %s id %d time %v\n",
				rcvlen, raddr,
				id-ident, time.Since(start))

			if delay > 0 {
				time.Sleep(time.Duration(delay) * time.Second)
			}
		} else {
			fmt.Printf("no response from %s: id %d\n", laddr, id-ident)
		}

		if id++; id > 254 {
			id = ident
		}

		if count > 0 {
			count--
		}
	}
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func ior(t, nr, size uintptr) uintptr {
	return (2 << 30) | (t << 8) | nr | (size << 16)
}

func iow(t, nr, size uintptr) uintptr {
	return (1 << 30) | (t << 8) | nr | (size << 16)
}

var (
	HCIGETDEVINFO = ior('H', 211, 4)
)

type HCIDeviceInfo struct {
	DevID uint16
	Name  [8]byte

	BDAddr [6]uint8

	Flags uint32
	Type  uint8

	Features [8]uint8

	PktType    uint32
	LinkPolicy uint32
	LinkMode   uint32

	AclMtu  uint16
	AclPkts uint16
	ScoMtu  uint16
	ScoPkts uint16

	Stat HCIDeviceStats
}

type HCIDeviceStats struct {
	ErrRx  uint32
	ErrTx  uint32
	CmdTx  uint32
	EvtRx  uint32
	AclTx  uint32
	AclRx  uint32
	ScoTx  uint32
	ScoRx  uint32
	ByteRx uint32
	ByteTx uint32
}

func LookupHCIDevice(s string) (di HCIDeviceInfo, err error) {
	var d int
	n, _ := fmt.Sscanf(s, "hci%d", &d)
	if n != 1 {
		err = fmt.Errorf("Invalid hci device: %s", s)
		return
	}

	dd, err := unix.Socket(unix.AF_BLUETOOTH, unix.SOCK_RAW|unix.SOCK_CLOEXEC, unix.BTPROTO_HCI)
	if err != nil {
		return
	}
	defer unix.Close(dd)

	_, _, errno := unix.Syscall(unix.SYS_IOCTL, uintptr(dd), uintptr(HCIGETDEVINFO), uintptr(unsafe.Pointer(&di)))
	if errno != 0 {
		err = errno
	}

	return
}

const (
	BDADDRANY = "00:00:00:00:00:00"
)

type BD [6]uint8

func ParseBD(s string) (BD, error) {
	var b BD
	n, _ := fmt.Sscanf(s, "%02x:%02x:%02x:%02x:%02x:%02x",
		&b[0], &b[1], &b[2], &b[3], &b[4], &b[5])
	if n != 6 {
		return b, fmt.Errorf("Invalid BD address")
	}
	return b, nil
}

func (b BD) String() string {
	return fmt.Sprintf("%02X:%02X:%02X:%02X:%02X:%02X",
		b[0], b[1], b[2], b[3], b[4], b[5])
}

type BDAddr struct {
	BD       BD
	Protocol string
}

func (bd BDAddr) Network() string {
	return bd.Protocol
}

func (bd BDAddr) String() string {
	return bd.BD.String()
}

const (
	L2CAP_CMD_HDR_SIZE = 4

	L2CAP_COMMAND_REJ = 0x01
	L2CAP_ECHO_REQ    = 0x08
	L2CAP_ECHO_RSP    = 0x09
	L2CAP_INFO_REQ    = 0x0a
	L2CAP_INFO_RSP    = 0x0b
)

type L2CAPCMDHDR struct {
	Code  uint8
	Ident uint8
	Len   uint16
}

type L2CAPConn struct {
	fd    int
	laddr BDAddr
	raddr BDAddr
}

func DialL2CAP(network string, laddr, raddr *BD) (*L2CAPConn, error) {
	if laddr == nil {
		laddr = &BD{}
	}
	if raddr == nil {
		return nil, &net.OpError{Op: "dial", Net: network, Err: fmt.Errorf("missing network address")}
	}

	fd, err := unix.Socket(unix.AF_BLUETOOTH, unix.SOCK_RAW, unix.BTPROTO_L2CAP)
	if err != nil {
		return nil, &net.OpError{Op: "socket", Net: network, Err: err}
	}

	err = unix.Bind(fd, &unix.SockaddrL2{Addr: *laddr})
	if err != nil {
		return nil, &net.OpError{Op: "bind", Net: network, Err: err}
	}

	err = unix.Connect(fd, &unix.SockaddrL2{Addr: *raddr})
	if err != nil {
		return nil, &net.OpError{Op: "connect", Net: network, Err: err}
	}

	sa, err := unix.Getsockname(fd)
	if err != nil {
		return nil, &net.OpError{Op: "getsockname", Net: network, Err: err}
	}
	*laddr = sa.(*unix.SockaddrL2).Addr

	return &L2CAPConn{fd: fd, laddr: BDAddr{*laddr, network}, raddr: BDAddr{*raddr, network}}, nil
}

func (c *L2CAPConn) LocalAddr() net.Addr {
	return c.laddr
}

func (c *L2CAPConn) RemoteAddr() net.Addr {
	return c.raddr
}

func (c *L2CAPConn) Read(b []byte) (n int, err error) {
	return unix.Read(c.fd, b)
}

func (c *L2CAPConn) Write(b []byte) (n int, err error) {
	return unix.Write(c.fd, b)
}

func (c *L2CAPConn) Close() error {
	return unix.Close(c.fd)
}

func (c *L2CAPConn) Poll(dt time.Duration) (n int, err error) {
	fds := []unix.PollFd{
		{Fd: int32(c.fd), Events: unix.POLLIN},
	}
	return unix.Poll(fds, int(dt.Milliseconds()))
}
