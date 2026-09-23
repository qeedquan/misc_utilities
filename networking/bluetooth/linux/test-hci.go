package main

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"log"
	"reflect"

	"github.com/davecgh/go-spew/spew"
	"github.com/qeedquan/go-media/bluetooth/bthci"
	"golang.org/x/sys/unix"
)

func main() {
	addr := fmt.Sprintf("%d:%d", bthci.DEV_NONE, unix.HCI_CHANNEL_CONTROL)
	conn, err := bthci.Dial("hci", addr)
	if err != nil {
		log.Fatal(err)
	}
	defer conn.Close()

	btio(conn, 0x1, bthci.DEV_NONE, nil, &Version{})
	btio(conn, 0x4, 0x0, nil, &Controller{})
	btio(conn, 0x6, 0x1, &Discoverable{1, 10}, new(uint32))
	btev(conn)
}

func btev(conn *bthci.Conn) {
	fmt.Printf("btev()\n")

	fd := int(conn.Fd())
	var buf [4096]byte
	for {
		n, err := unix.Read(fd, buf[:])
		if err != nil {
			fmt.Printf("read: %v\n", err)
			return
		}
		fmt.Printf("% x\n", buf[:n])
	}
}

func btio(conn *bthci.Conn, code, index uint16, param, result interface{}) {
	fmt.Printf("btio(code = %#x, index = %#x)\n", code, index)

	var data []byte
	var length uint16
	if param != nil {
		rw := new(bytes.Buffer)
		rt := reflect.TypeOf(param)
		length = uint16(rt.Size())
		binary.Write(rw, binary.LittleEndian, param)
		data = rw.Bytes()
	}

	fd := int(conn.Fd())
	buf := make([]byte, 6+length)
	binary.LittleEndian.PutUint16(buf[0:], code)
	binary.LittleEndian.PutUint16(buf[2:], index)
	binary.LittleEndian.PutUint16(buf[4:], length)
	copy(buf[6:], data)

	_, err := unix.Write(fd, buf)
	if err != nil {
		fmt.Printf("write: %v\n", err)
		return
	}

	resp := make([]byte, 1024)
	_, err = unix.Read(fd, resp)
	if err != nil {
		fmt.Printf("read: %v\n", err)
		return
	}
	err = binary.Read(bytes.NewBuffer(resp), binary.LittleEndian, result)
	if err != nil {
		fmt.Printf("bread: %v\n", err)
		return
	}

	spew.Dump(result)
	switch v := result.(type) {
	case *uint32:
		fmt.Printf("%#x\n", *v)
	}
	fmt.Println()
}

type Version struct {
	Header   [6]byte
	Version  uint8
	Revision uint16
}

type Controller struct {
	Header       [9]byte
	Addr         [6]byte
	Version      byte
	Manufacturer [2]byte
	Supported    [4]byte
	Current      [4]byte
	Class        [3]byte
	Name         [249]byte
	ShortName    [11]byte
}

type Discoverable struct {
	Type    uint8
	Timeout uint16
}
