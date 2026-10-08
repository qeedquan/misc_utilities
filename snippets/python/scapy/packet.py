#!/usr/bin/env python3

from scapy.all import *

a = IP()
print(a)
hexdump(a)

a = Ether()/IP()/TCP()
print(a)
hexdump(a)

p = PacketList([a, a, a, a])
p.nsummary()
