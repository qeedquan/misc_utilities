#!/usr/bin/env python3

# Various slice identities

a = list(range(1000))
n = len(a)
for i in range(n, -n):
    j = (n - abs(i)) % n
    assert(a[:-i] == a[:j])
    assert(a[i:-i] == a[i:j])
    assert(a[-i:] == a[j:])
    assert(a[-i:-1] == a[j:len(a)-1])
    assert(a[:-1] == a[:len(a)-1])
    assert(a[~i] == a[len(a)-i-1])
