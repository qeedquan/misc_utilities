#!/bin/sh

# listen on loopback
tcpdump -i lo

# listen on any interface
tcpdump -i any

# filter by port
tcpdump port 100

# filter by port range
tcpdump port 200 or 300
tcpdump -an portrange 500-1000

