#!/usr/bin/env python

# packages:
# python-dbus
import dbus

def print_sep(sep=5):
    print("-" * sep)

# dump system services
print("System\n")
for service in dbus.SystemBus().list_names():
    print(service)

print_sep()

# dump session services
print("Session\n")
for service in dbus.SessionBus().list_names():
    print(service)

print_sep()

# make a session bus
bus = dbus.SessionBus()
print(bus.get_unique_name())

# same way as printing session bus above
print_sep()
print("Session (Again)\n")
for service in bus.list_names():
    print(service)
