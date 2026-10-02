"""

https://en.wikipedia.org/wiki/Hofstadter_sequence

"""

import sys

def memoize(f):
    memo = {}
    def helper(x):
        if x not in memo:            
            memo[x] = f(x)
        return memo[x]
    return helper

def FF(n):
    if n <= 0:
        return 0
    r = 1
    s = 2
    m = {}
    for i in range(n-1):
        m[r] = True
        m[r+s] = True
        
        r += s
        while True:
            s += 1
            if s not in m:
                break

    return (r, s)

def G(n):
    if n <= 0:
        return 0
    return n - G(G(n - 1))

def H(n):
    if n <= 0:
        return 0
    return n - H(H(H(n -1)))

def F(n):
    if n <= 0:
        return 1
    return n - M(F(n - 1))

def M(n):
    if n <= 0:
        return 0
    return n - F(M(n - 1))

def FM(n):
    return (F(n), M(n))

def Q(n):
    if n <= 0:
        return 0
    if n <= 2:
        return 1
    return Q(n - Q(n - 1)) + Q(n - Q(n - 2))

def HC(n):
    if n <= 0:
        return 0
    if n <= 2:
        return 1
    return HC(HC(n-1)) + HC(n - HC(n-1))

def iter(name, f, n):
    print("{} Sequence".format(name))
    for i in range(n):
        print(i, f(i))
    print()

G = memoize(G)
H = memoize(H)
F = memoize(F)
M = memoize(M)
Q = memoize(Q)
HC = memoize(HC)

if len(sys.argv) < 2:
    print("usage: order")
    sys.exit()

N = int(sys.argv[1]) + 1

iter("Figure-Figure", FF, N)
iter("G", G, N)
iter("H", H, N)
iter("Female and Male", FM, N)
iter("Q", Q, N)
iter("Hofstader-Conway", HC, N)
