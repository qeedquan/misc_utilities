#!/usr/bin/env python

"""
https://specifications.freedesktop.org/mpris-spec/2.2/
https://amish.naidu.dev/blog/dbus/

Mainstream media players using this:
audacious
vlc
parole media player
"""

import dbus
import re

player_path = "org.mpris.MediaPlayer2.Player"

bus = dbus.SessionBus()

def test_control(player):
    # go to next song
    player.Next(dbus_interface=player_path)

    # go to previous song
    player.Previous(dbus_interface=player_path)

    # pause
    player.Pause(dbus_interface=player_path)

    # play/pause
    player.PlayPause(dbus_interface=player_path)

    # we can simplify the above by creating an interface
    interface = dbus.Interface(player, dbus_interface=player_path)

    # all method above applies
    interface.Next()

def get_properties(service):
    print(player.Get(player_path, 'Volume', dbus_interface='org.freedesktop.DBus.Properties'))

    # can create an interface object to make it simpler
    property_interface = dbus.Interface(player, dbus_interface='org.freedesktop.DBus.Properties')
    volume = property_interface.Get('org.mpris.MediaPlayer2.Player', 'Volume')
    print(volume)

    for property, value in property_interface.GetAll('org.mpris.MediaPlayer2.Player').items():
        print(property, ':', value)

    metadata = player.Get('org.mpris.MediaPlayer2.Player', 'Metadata', dbus_interface='org.freedesktop.DBus.Properties')
    for attr, value in metadata.items():
        print(attr, '\t', value)

for service in bus.list_names():
    if re.match('org.mpris.MediaPlayer2.', service):
        mpris_path = "/org/mpris/MediaPlayer2"

        # given an object, we can call an interface defined on the object
        player = bus.get_object(service, mpris_path)
        test_control(player)
        get_properties(player)
