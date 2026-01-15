#!/bin/sh

# dump all session services
echo "Session Services"
dbus-send --session           \
  --dest=org.freedesktop.DBus \
  --type=method_call          \
  --print-reply               \
  /org/freedesktop/DBus       \
  org.freedesktop.DBus.ListNames

# dump all system services
echo "System Services"
dbus-send --system            \
  --dest=org.freedesktop.DBus \
  --type=method_call          \
  --print-reply               \
  /org/freedesktop/DBus       \
  org.freedesktop.DBus.ListNames

# gdbus is from glib2, bluez uses it officially
gdbus call --system --dest org.freedesktop.DBus      \
           --object-path /org/freedesktop/DBus       \
           --method org.freedesktop.DBus.ListNames 
