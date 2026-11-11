// +build unix

// ported from toybox
package main

import (
	"flag"
	"fmt"
	"io"
	"log/syslog"
	"os"
	"os/user"
	"strconv"
	"strings"
)

var (
	status int
)

func main() {
	var prio, tag string
	flag.StringVar(&prio, "p", "user.notice", "set priority")
	user, err := user.Current()
	if err == nil {
		tag = user.Username
	}
	flag.StringVar(&tag, "t", tag, "use tag to prefix every message")
	flag.Usage = usage
	flag.Parse()

	value, err := lookup(prio)
	ck(err)

	logger, err := syslog.New(value, tag)
	ck(err)

	var buf []byte
	if flag.NArg() < 1 {
		buf, err = io.ReadAll(os.Stdin)
		ck(err)
	} else {
		buf = []byte(strings.Join(flag.Args(), " "))
	}

	_, err = logger.Write(buf)
	ek(err)
	ek(logger.Close())
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: logger [options] message")
	flag.PrintDefaults()
	os.Exit(2)
}

func ck(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "logger:", err)
		os.Exit(1)
	}
}

func ek(err error) {
	if err != nil {
		fmt.Fprintln(os.Stderr, "logger:", err)
		status |= 1
	}
}

func lookup(str string) (syslog.Priority, error) {
	n, err := strconv.ParseInt(str, 64, 0)
	if err == nil {
		return syslog.Priority(n), nil
	}

	facility, priority := "user", "notice"
	if i := strings.IndexRune(str, '.'); i > 0 {
		facility, priority = str[:i], str[i+1:]
	} else {
		priority = str
	}
	facility = strings.ToLower(facility)
	priority = strings.ToLower(priority)

	var p syslog.Priority
	switch facility {
	case "kern":
		p |= syslog.LOG_KERN
	case "user":
		p |= syslog.LOG_USER
	case "mail":
		p |= syslog.LOG_MAIL
	case "daemon":
		p |= syslog.LOG_DAEMON
	case "auth":
		p |= syslog.LOG_AUTH
	case "syslog":
		p |= syslog.LOG_SYSLOG
	case "lpr":
		p |= syslog.LOG_LPR
	case "news":
		p |= syslog.LOG_NEWS
	case "uucp":
		p |= syslog.LOG_UUCP
	case "cron":
		p |= syslog.LOG_CRON
	case "authpriv":
		p |= syslog.LOG_AUTHPRIV
	case "ftp":
		p |= syslog.LOG_FTP
	case "local0":
		p |= syslog.LOG_LOCAL0
	case "local1":
		p |= syslog.LOG_LOCAL1
	case "local2":
		p |= syslog.LOG_LOCAL2
	case "local3":
		p |= syslog.LOG_LOCAL3
	case "local4":
		p |= syslog.LOG_LOCAL4
	case "local5":
		p |= syslog.LOG_LOCAL5
	case "local6":
		p |= syslog.LOG_LOCAL6
	case "local7":
		p |= syslog.LOG_LOCAL7
	default:
		return 0, fmt.Errorf("invalid facility %q", facility)
	}

	switch priority {
	case "emerg":
		p |= syslog.LOG_EMERG
	case "alert":
		p |= syslog.LOG_ALERT
	case "crit":
		p |= syslog.LOG_CRIT
	case "err":
		p |= syslog.LOG_ERR
	case "warning":
		p |= syslog.LOG_WARNING
	case "notice":
		p |= syslog.LOG_NOTICE
	case "info":
		p |= syslog.LOG_INFO
	case "debug":
		p |= syslog.LOG_DEBUG
	default:
		return 0, fmt.Errorf("invalid priority %q", priority)
	}

	return p, nil
}
