package main

import (
	"fmt"
	"log"
	"os/exec"
	"syscall"
	"time"

	"github.com/qeedquan/go-media/posix"
)

func main() {
	log.SetPrefix("")
	log.SetFlags(0)

	m, s, err := posix.Openpty(nil, nil)
	if err != nil {
		log.Fatal(err)
	}
	cmd := exec.Command("sh")
	cmd.Stdin = s
	cmd.Stdout = s
	cmd.Stderr = s
	cmd.SysProcAttr = &syscall.SysProcAttr{
		Setsid:  true,
		Setctty: true,
		Ctty:    int(s.Fd()),
	}
	err = cmd.Start()
	if err != nil {
		log.Fatal(err)
	}

	for {
		var buf [8192]byte
		n, err := m.Read(buf[:])
		if err != nil {
			break
		}

		fmt.Printf("%q\n", buf[:n])
		m.Write([]byte("date\n"))
		time.Sleep(1 * time.Second)
	}
}
