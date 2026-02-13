// UDP sockets created in Go does not use SO_REUSEADDR, so we can't make multiple clients listen on for broadcast messages
// we can make one if we use the syscall package though, and then create a network connection out of that.
// This way, we can get broadcast if we ran several instances of this. To see it fail to bind
// if there is more than one listener, comment out the setsockopt portion with SO_REUSEADDR

// To generate UDP broadcasts, do this:
// while true; do echo hello world | socat - UDP-DATAGRAM:255.255.255.255:1234,broadcast; sleep 1; done

package main

import (
	"fmt"
	"log"
	"net"
	"os"
	"syscall"
)

func main() {
	s, err := syscall.Socket(syscall.AF_INET, syscall.SOCK_DGRAM, 0)
	ck(err)

	err = syscall.SetsockoptInt(s, syscall.SOL_SOCKET, syscall.SO_BROADCAST, 1)
	ck(err)

	err = syscall.SetsockoptInt(s, syscall.SOL_SOCKET, syscall.SO_REUSEADDR, 1)
	ck(err)

	err = syscall.Bind(s, &syscall.SockaddrInet4{
		Port: 1234,
		Addr: [4]byte{255, 255, 255, 255},
	})
	ck(err)

	f := os.NewFile(uintptr(s), "socket")
	c, err := net.FileConn(f)
	ck(err)

	for {
		var buf [8192]byte
		n, err := c.Read(buf[:])
		ck(err)
		fmt.Printf("%q\n", buf[:n])
	}
}

func ck(err error) {
	if err != nil {
		log.Fatal(err)
	}
}
