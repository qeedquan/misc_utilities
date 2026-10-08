// ported from OpenBSD whois

package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"net"
	"net/textproto"
	"os"
	"strconv"
	"strings"
	"time"
	"unicode/utf8"
)

func main() {
	var host, country, port string
	var quick bool
	var flags int
	var timeout time.Duration

	for _, p := range hosts {
		h := &hostFlag{p.name, &host}
		flag.Var(h, p.flag, fmt.Sprint("use host ", p.name))
	}
	flag.StringVar(&host, "h", "", "use hostname")
	flag.StringVar(&country, "c", "", "use country")
	flag.StringVar(&port, "p", "whois", "use port")
	flag.BoolVar(&quick, "Q", false, "perform quick whois")
	flag.DurationVar(&timeout, "t", 0, "dial timeout")

	flag.Usage = usage
	flag.Parse()
	if flag.NArg() < 1 || (country != "" && host != "") {
		usage()
	}

	if quick {
		flags |= QUICK
	}
	if host == "" && country == "" && flags&QUICK == 0 {
		flags |= RECURSE
	}

	status := 0
	for _, name := range flag.Args() {
		xhost := host
		if xhost == "" {
			xhost = chooseServer(name, country)
		}
		status |= whois(name, xhost, port, flags, timeout)
	}
	os.Exit(status)
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] name ...")
	flag.PrintDefaults()
	os.Exit(1)
}

func whois(query, server, port string, flags int, timeout time.Duration) int {
	fmt.Println(query, server, port)
	addr := fmt.Sprintf("%s:%s", server, port)
	conn, err := net.DialTimeout("tcp", addr, timeout)
	if err != nil {
		fmt.Fprintln(os.Stderr, "whois:", err)
		return 1
	}
	defer conn.Close()

	var format string
	switch server {
	case "whois.denic.de", "de" + QNICHOST_TAIL:
		format = "-T dn,ace -C ISO-8859-1 %s\r\n"

	case "whois.dk-hostmaster.dk", "dk" + QNICHOST_TAIL:
		format = "--show-handles %s\r\n"

	default:
		format = "%s\r\n"
	}
	fmt.Fprintf(conn, format, query)

	var nhost, line string
	r := textproto.NewReader(bufio.NewReader(conn))
	for {
		line, err = r.ReadLine()
		if err != nil {
			if err != io.EOF {
				fmt.Fprintln(os.Stderr, "whois:", err)
			}
			break
		}

		line = strings.TrimSpace(line)
		fmt.Println(line)

		if nhost != "" || flags&RECURSE == 0 {
			continue
		}

		i := strings.Index(line, "Whois Server:")
		if i > 0 {
			line = strings.TrimSpace(line)
		} else if server == ANICHOST {
			ipWhois := []string{LNICHOST, RNICHOST, PNICHOST, BNICHOST, AFNICHOST}
			line = strings.ToLower(line)

			for _, ip := range ipWhois {
				if strings.Index(line, ip) >= 0 {
					nhost = ip
					break
				}
			}
		}
	}

	status := 0
	if err != nil {
		status = 1
	}

	if nhost != "" {
		status |= whois(query, nhost, port, 0, timeout)
	}

	return 0
}

// if no country is specified, determine the top level domain from the query
// if the tld is a number, query ARIN, otherwise, use TLD.whois-server.net
// if the domain does not contain '.', check to see if it is a NSI handle
// (starts eith '!') or a CORE handle (COCO[0-9]+ or COHO-[0-9]+) or an
// ASN (starts with AS). fall back to NICHOST for the non-handle case

func chooseServer(name, country string) string {
	var qhead string

	uname := strings.ToUpper(name)
	i := strings.LastIndex(uname, ".")
	if country != "" {
		qhead = country
	} else if i < 0 {
		switch {
		case strings.HasPrefix(uname, "!"):
			return INICHOST

		case strings.HasPrefix(uname, "COCO-"),
			strings.HasPrefix(uname, "COHO-"):
			_, err := strconv.ParseInt(uname[5:], 10, 64)
			if err == nil {
				return CNICHOST
			}

		case strings.HasPrefix(uname, "AS"):
			_, err := strconv.ParseInt(uname[2:], 10, 64)
			if err == nil {
				return MNICHOST
			}

		default:
			return NICHOST
		}
	} else {
		qhead = name[i+1:]
		if len(qhead) > 0 && isDigit(qhead[0]) {
			return ANICHOST
		}
	}

	// post-2003 ("new") gTLDs are all supposed to have "whois.nic.domain"
	// (per registry agreement), some older gTLDs also support this...
	server := "whois.nic." + qhead
	useQNIC := false

	// most ccTLDs don't do this, but QNICHOST/whois-servers mostly works
	// and is required for most <=2003 TLDs/gTLDs
	switch strings.ToLower(qhead) {
	case "org", "com", "net", "pro", "info", "aero", "jobs", "mobi", "museum":
		useQNIC = true

	default:
		if utf8.RuneCountInString(qhead) == 2 {
			useQNIC = true
		} else {
			_, err := net.LookupHost(server)
			if err != nil {
				useQNIC = true
			}
		}
	}

	// for others, if whois.nic.TLD doesn't exist, try whois-servers
	if useQNIC {
		server = qhead + QNICHOST_TAIL
	}

	return server
}

func isDigit(c byte) bool {
	return '0' <= c && c <= '9'
}

const (
	RECURSE = 1 << iota
	QUICK
)

const (
	NICHOST       = "whois.crsnic.net"
	INICHOST      = "whois.networksolutions.com"
	CNICHOST      = "whois.corenic.net"
	DNICHOST      = "whois.nic.mil"
	GNICHOST      = "whois.nic.gov"
	ANICHOST      = "whois.arin.net"
	RNICHOST      = "whois.ripe.net"
	PNICHOST      = "whois.apnic.net"
	RUNICHOST     = "whois.ripn.net"
	MNICHOST      = "whois.ra.net"
	LNICHOST      = "whois.lacnic.net"
	AFNICHOST     = "whois.afrinic.net"
	BNICHOST      = "whois.registro.br"
	PDBHOST       = "whois.peeringdb.com"
	IANAHOST      = "whois.iana.org"
	QNICHOST_TAIL = ".whois-servers.net"
)

type hostFlag struct {
	name string
	bind *string
}

func (hostFlag) String() string   { return "" }
func (hostFlag) IsBoolFlag() bool { return true }

func (h *hostFlag) Set(string) error {
	*h.bind = h.name
	return nil
}

var hosts = []struct {
	flag string
	name string
}{
	{"a", ANICHOST},
	{"A", PNICHOST},
	{"D", NICHOST},
	{"g", GNICHOST},
	{"i", INICHOST},
	{"I", IANAHOST},
	{"l", LNICHOST},
	{"m", MNICHOST},
	{"P", PDBHOST},
	{"r", RNICHOST},
	{"R", RUNICHOST},
}
