#!/usr/bin/env python3

import itertools

def iterate(n):
    for x in itertools.chain(range(1, n), range(n, 0, -1)):
        print(x)

iterate(5)

