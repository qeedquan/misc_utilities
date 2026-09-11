#!/bin/sh

# Manual creation of a bridge device for tap and ethernet
# Run this script and specify QEMU to use TAP as the device

# The bridge device IP will be assigned some random IP on the network with DHCP
# The guest OS in QEMU will get another IP using DHCP, the host can interact with the guest using the guest OS assigned IP
# It is important to note that the guest OS IP is different from the bridge IP

brctl addbr br0
ip addr flush dev enp6s0
brctl addif br0 enp6s0
tunctl -t tap0 -u `whoami`
brctl addif br0 tap0
ifconfig enp6s0 up
ifconfig tap0 up
ifconfig br0 up
dhclient -v br0


