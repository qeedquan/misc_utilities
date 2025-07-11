#!/usr/bin/env python3

# Various slice identities

a = list(range(100))
n = len(a)
for i in range(n):
    j = (n - i) % n
    assert(a[:-i] == a[:j])
    assert(a[i:-i] == a[i:j])
    assert(a[-i:] == a[j:])

