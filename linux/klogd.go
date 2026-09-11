// +build linux

// ported from toybox
package main

import (
	"bytes"
	"flag"
	"fmt"
	"log/syslog"
	"os"
	"os/signal"
	"syscall"
	"unicode/utf8"
)

var (
	level  = flag.Int("C", 0, "print to console messages more urgent than prio N (1-8)")
	kern   = flag.Bool("K", false, "enable kernel ring buffer as log source")
	logger *syslog.Writer
	fd     *os.File
)

func main() {
	flag.Usage = usage
	flag.Parse()
	flag.Visit(func(f *flag.Flag) {
		switch f.Name {
		case "C":
			setLogLevel(*level)
		}
	})

	var err error
	logger, err = syslog.New(0, "")
	ck(err)

	if *kern {
		logger.Notice("KLOGD: started with Kernel ring buffer as log source\n")
		kctl(1, 0)
	} else {
		fd, err = os.Open("/proc/kmsg")
		ck(err)
		logger.Notice("KLOGD: started with /proc/kmsg as log source\n")
	}

	logger.Close()
	logger, err = syslog.New(syslog.LOG_KERN, "Kernel")
	ck(err)

	sigCh := make(chan os.Signal)
	var sigs []os.Signal
	for i := syscall.Signal(-1); i != syscall.SIGCHLD; i++ {
		sigs = append(sigs, i)
	}
	signal.Notify(sigCh, sigs...)

	readCh := make(chan string)
	go reader(readCh)

loop:
	for {
		select {
		case <-sigCh:
			break loop
		case str := <-readCh:
			log(str)
		}
	}

	logger.Notice("KLOGD: Daemon exiting......\n")
	if *kern {
		kctl(7, 0)
		kctl(0, 0)
	} else {
		setLogLevel(7)
		fd.Close()
	}
	os.Exit(1)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: klogd [-C level] [-K]")
	flag.PrintDefaults()
	os.Exit(3)
}

func setLogLevel(n int) {
	if *kern {
		kctl(8, n)
	} else {
		f, err := os.OpenFile("/proc/sys/kernel/printk", os.O_WRONLY, 0644)
		ck(err)
		fmt.Fprintf(f, "%d\n", n)
		f.Close()
	}
}

func ck(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "klogd:", err)
		if logger != nil {
			logger.Crit("klogd: " + err.Error() + "\n")
		}
		os.Exit(2)
	}
}

func kctl(typ, length int) int {
	n, _, e := syscall.Syscall(syscall.SYS_SYSLOG, uintptr(typ), uintptr(0), uintptr(length))
	if e != 0 {
		ck(syscall.Errno(e))
	}
	return int(n)
}

func log(str string) {
	prio := syslog.LOG_INFO
	n, _ := fmt.Sscanf(str, "<%d>", &prio)
	if n == 1 {
		str = str[3:]
	}

	switch prio {
	case syslog.LOG_EMERG:
		logger.Emerg(str)
	case syslog.LOG_ALERT:
		logger.Alert(str)
	case syslog.LOG_CRIT:
		logger.Crit(str)
	case syslog.LOG_ERR:
		logger.Err(str)
	case syslog.LOG_WARNING:
		logger.Warning(str)
	case syslog.LOG_NOTICE:
		logger.Notice(str)
	case syslog.LOG_DEBUG:
		logger.Debug(str)
	case syslog.LOG_INFO:
		fallthrough
	default:
		logger.Info(str)
	}
}

func reader(ch chan string) {
	var buf bytes.Buffer
	var msg [16348]byte
	for {
		var n int
		var err error
		if *kern {
			n, err = syscall.Klogctl(1, msg[:])
		} else {
			n, err = fd.Read(msg[:])
		}
		ck(err)

		for i := 0; i < n; {
			r, size := utf8.DecodeRune(msg[i:])
			i += size

			buf.WriteRune(r)
			if r == '\n' || buf.Len() >= len(msg) {
				ch <- buf.String()
				buf.Reset()
			}
		}
	}
}
