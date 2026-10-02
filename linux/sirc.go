// based on suckless sic irc client

package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"net"
	"os"
	"os/user"
	"strings"
	"time"
	"unicode/utf8"
)

func main() {
	c := NewClient()

	flag.Usage = usage
	flag.StringVar(&c.Host, "h", c.Host, "host")
	flag.StringVar(&c.Port, "p", c.Port, "port")
	flag.StringVar(&c.Nick, "n", c.Nick, "nick")
	flag.StringVar(&c.Pass, "k", c.Pass, "password")
	flag.Parse()

	c.Chat()
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options]")
	flag.PrintDefaults()
	os.Exit(1)
}

type Client struct {
	net.Conn
	Host   string
	Port   string
	Nick   string
	Pass   string
	Part   string
	Chan   string
	Stdin  io.Reader
	Stdout io.Writer
	CP     rune
	quit   chan bool
	err    chan error
}

func NewClient() *Client {
	nick := "luser"
	user, err := user.Current()
	if err == nil {
		nick = user.Username
	}

	return &Client{
		Host:   "irc.irchighway.net",
		Port:   "6667",
		Nick:   nick,
		Stdin:  os.Stdin,
		Stdout: os.Stdout,
		Part:   "sirc - 250 LOC are too much!",
		CP:     ':',
		quit:   make(chan bool),
		err:    make(chan error),
	}
}

func (c *Client) Chat() (err error) {
	c.Conn, err = net.Dial("tcp", fmt.Sprintf("%s:%s", c.Host, c.Port))
	if err != nil {
		return
	}

	if c.Pass != "" {
		c.sout("PASS %s", c.Pass)
	}
	c.sout("NICK %s", c.Nick)
	c.sout("USER %s localhost %s :%s", c.Nick, c.Host, c.Nick)

	go c.input()
	go c.listen()

	defer close(c.quit)
	return <-c.err
}

func (c *Client) input() {
	r := bufio.NewScanner(c.Stdin)
	for {
		select {
		case <-c.quit:
			return

		default:
			if !r.Scan() {
				c.err <- r.Err()
				return
			}
			c.pin(r.Text())
		}
	}
}

func (c *Client) listen() {
	r := bufio.NewScanner(c)
	res := time.Now()
	for {
		select {
		case <-c.quit:
			c.Close()
			return

		default:
			c.SetReadDeadline(time.Now().Add(120 * time.Second))
			if !r.Scan() {
				if ne, ok := r.Err().(net.Error); ok {
					if ne.Temporary() && ne.Timeout() && time.Since(res) < 300*time.Second {
						r = bufio.NewScanner(c)
						c.sout("PING %s", c.Host)
						continue
					}
				}
				c.err <- r.Err()
				return
			}
			c.psrv(r.Text())
			res = time.Now()
		}
	}
}

func (c *Client) pin(s string) {
	s = strings.TrimSpace(s)
	if !strings.HasPrefix(s, string(c.CP)) {
		c.privmsg(c.Chan, s)
		return
	}

	s = s[utf8.RuneLen(c.CP):]
	s = strings.Replace(s, "\t", " ", -1)
	s = strings.Replace(s, "\r", " ", -1)
	s = strings.Replace(s, "\n", " ", -1)
	l := strings.Split(s, " ")
	r, l := strings.ToLower(l[0]), l[1:]
	switch r {
	case "n", "nick":
		c.Nick = l[0]
		c.sout("NICK %s", l[0])

	case "j", "join":
		c.sout("JOIN %s", l[0])
		c.Chan = l[0]

	case "l", "leave", "part":
		channel := l[0]
		part := c.Part
		if len(l) > 1 {
			part = strings.Join(l[1:], " ")
		}
		c.sout("PART %s :%s", channel, part)

	case "m", "msg":
		channel := l[0]
		msg := ""
		if len(l) > 1 {
			msg = strings.Join(l[1:], " ")
		}
		c.privmsg(channel, msg)

	case "s", "switch":
		c.Chan = l[0]

	case "q", "quit":
		c.err <- io.EOF

	default:
		c.sout("%s", s)
	}
}

func (c *Client) psrv(s string) {
	if len(s) < 5 {
		return
	}

	var nick, usr, host string
	if s[0] == ':' {
		var src string

		if i := strings.Index(s, " "); i >= 0 {
			src = s[1:i]
		} else {
			return
		}
		i := strings.Index(src, "!")
		j := strings.Index(src, "@")
		if i >= 0 && j >= 0 && i < j {
			nick = src[:i]
			usr = src[i+1 : j]
			host = src[j+1 : len(src)]
		}
	}

	split := strings.SplitN(s, " :", 2)
	cmd := strings.ToUpper(split[0])
	par := ""
	txt := ""
	if len(split) > 1 {
		par = split[1]
		txt = split[1]
	}
	switch cmd {
	case "PONG":
		return
	case "PRIVMSG":
		c.pout(par, "<%s> %s", usr, txt)
	case "PING":
		c.sout("PONG %s", txt)
	default:
		c.pout(host, ">< %s (%s): %s", cmd, nick, txt)
		if cmd == "NICK" && usr == c.Nick {
			c.Nick = txt
		}
	}
}

func (c *Client) sout(format string, args ...interface{}) {
	fmt.Fprintf(c, format+"\r\n", args...)
}

func (c *Client) pout(channel, format string, args ...interface{}) {
	tim := time.Now().Local().Format("2006-Jan-2 15:04:05")
	str := fmt.Sprintf(format, args...)
	fmt.Fprintf(c.Stdout, "%-12s: %s %s\n", channel, tim, str)
}

func (c *Client) privmsg(channel, msg string) {
	if channel == "" {
		c.pout(channel, "No channel to send to")
		return
	}
	c.pout(channel, "<%s> %s", c.Nick, msg)
	c.sout("PRIVMSG %s :%s", channel, msg)
}
