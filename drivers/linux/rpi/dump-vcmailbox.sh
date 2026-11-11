#!/bin/sh

# https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface

# tag reqlen rsplen [data...]
# even if reqlen is empty, it must match rsplen padded with 0 data
# because the buffer used for request is the same as response

echo "Firmware Revision"
vcmailbox 0x00000001 4 4 0

echo "Board Model"
vcmailbox 0x00010001 4 4 0

echo "Board Revison"
vcmailbox 0x00010002 4 4 0

echo "Board MAC Address"
vcmailbox 0x00010003 8 8 0 0

echo "Board Serial"
vcmailbox 0x00010004 8 8 0 0

echo "ARM Memory"
vcmailbox 0x00010005 8 8 0 0

echo "VC Memory"
vcmailbox 0x00010006 8 8 0 0

echo "Command Line Config"
vcmailbox 0x00050001 32 32 0 0 0 0 0 0 0 0

echo "DMA Channels"
vcmailbox 0x00060001 4 4 0

echo "Physical Display"
vcmailbox 0x00040003 8 8 0 0

echo "Virtual Display"
vcmailbox 0x00040004 8 8 0 0

echo "Depth"
vcmailbox 0x00040005 4 4 0

echo "Max Voltage"
vcmailbox 0x00030005 8 8 1 0
vcmailbox 0x00030005 8 8 2 0
vcmailbox 0x00030005 8 8 3 0
vcmailbox 0x00030005 8 8 4 0

echo "Clock State"
vcmailbox 0x00030001 8 8 3 0 

echo "Clock Rate"
vcmailbox 0x00030002 8 8 1 0
vcmailbox 0x00030002 8 8 2 0
vcmailbox 0x00030002 8 8 3 0
vcmailbox 0x00030002 8 8 4 0
