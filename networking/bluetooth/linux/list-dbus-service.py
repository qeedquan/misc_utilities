#!/usr/bin/env python

import dbus
import json

bus = dbus.SystemBus()
manager = dbus.Interface(bus.get_object("org.bluez", "/"), "org.freedesktop.DBus.ObjectManager")

objects = manager.GetManagedObjects()
for path, intf in objects.iteritems():
    print("Path: " + path)
    print(json.dumps(intf, indent=1))
