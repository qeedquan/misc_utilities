#!/usr/bin/env python

import bluetooth
from bluetooth.ble import DiscoveryService

duration = 10

print("Performing bluetooth classic query...")
nearby_devices = bluetooth.discover_devices(duration=duration, lookup_names=True, flush_cache=True, lookup_class=False)
print("Found {} devices".format(len(nearby_devices)))

for addr, name in nearby_devices:
    try:
        print("   {} - {}".format(addr, name))
    except UnicodeEncodeError:
        print("   {} - {}".format(addr, name.encode("utf-8", "replace")))


print("Performing bluetooth low energy query...")
service = DiscoveryService()
devices = service.discover(duration)
for address, name in devices.items():
    print("Name: {}, address: {}".format(name, address))
