#!/usr/bin/python

import resource
import sys

cache = {}

def ackermann(x, y):
    if cache.has_key(x) and cache[x].has_key(y):
        return cache[x][y]

    if x == 0:
        z = y+1
    elif y == 0:
        z = ackermann(x-1, 1)
    else:
        z = ackermann(x-1, ackermann(x, y-1))

    cache[x] = {y: z}
    return z

def main():
    resource.setrlimit(resource.RLIMIT_STACK, [0x100000000, resource.RLIM_INFINITY])
    sys.setrecursionlimit(0x1000000)

    if len(sys.argv) != 3:
        for x in xrange(1, 3):
            for y in xrange(1, 1001):
                print("({:d}, {:d}) -> {:d}".format(x, y, ackermann(x, y)))
        exit(0)

    x = int(sys.argv[1])
    y = int(sys.argv[2])
    print("({:d}, {:d}) -> {:d}".format(x, y, ackermann(x, y)))

main()
