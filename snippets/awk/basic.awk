#!/bin/sh

# insert text above of a line that match
awk '/search_text/{print "insert_text"}1' file
