// daytime client
// http://tf.nist.gov/tf-cgi/servers.cgi# has some servers to try

package main

import (
	"flag"
	"fmt"
	"io"
	"net"
	"os"
	"time"
)

var options struct {
	Retries int
	Network string
	Timeout time.Duration
}

func main() {
	flag.StringVar(&options.Network, "net", "tcp", "network type")
	flag.IntVar(&options.Retries, "retries", 5, "max number of retries, use 0 for infinite retries")
	flag.DurationVar(&options.Timeout, "timeout", 2*time.Second, "connection timeout")
	flag.Usage = usage
	flag.Parse()

	var err error
	switch n := flag.NArg(); {
	case n < 1:
		usage()

	case n == 1:
		address := fmt.Sprintf("%s:13", flag.Arg(0))
		err = daytime(options.Network, address, options.Timeout, options.Retries)

	default:
		for i := 1; i < n; i++ {
			address := fmt.Sprintf("%s:%s", flag.Arg(0), flag.Arg(i))
			xerr := daytime(options.Network, address, options.Timeout, options.Retries)
			if xerr != nil {
				err = xerr
			}
		}
	}

	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
}

func daytime(network, address string, timeout time.Duration, retries int) error {
	conn, err := net.DialTimeout(network, address, timeout)
	if err != nil {
		return err
	}

	conn.SetDeadline(time.Now().Add(timeout))
	for {
		n, err := io.Copy(os.Stdout, conn)
		if n == 0 || err == io.EOF {
			break
		}

		if err != nil {
			ne, ok := err.(net.Error)
			if ok && ne.Temporary() {
				fmt.Println(ne)
				conn.SetDeadline(time.Now().Add(timeout))
				if retries > 0 {
					if retries--; retries <= 0 {
						break
					}
					continue
				}
			}
			return err
		}
	}

	return nil
}

func usage() {
	fmt.Fprintln(os.Stderr, "usage: [options] <address> [ports] ...")
	flag.PrintDefaults()
	os.Exit(1)
}
