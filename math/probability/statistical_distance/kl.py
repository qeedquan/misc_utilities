"""

https://en.wikipedia.org/wiki/Kullback%E2%80%93Leibler_divergence
https://www.youtube.com/watch?v=SxGYPqCgJWM

D_KL(P || Q) - Measures how much the probability distribution Q differs from P

It is not symmetrical (the operation is not commutative), that is

D_KL(P || Q) != D_KL(Q || P)

For a symmetrical measure, use
https://en.wikipedia.org/wiki/Jensen%E2%80%93Shannon_divergence

The output value can be seen as a number of bits measuring entropy.

That is: If I have two coins, one fair and one weighted, KL divergence tells me how much information
do I need to distinguish the weighted coin from the fair coin.

If the two distribution are exactly the same, then the KL divergence is 0
The range of the KL divergence output ranges from 0 to oo (0 means equal, higher number means more difference).

If we are attempting to approximate an unknown probability distribution,
then the target probability distribution from data is P and Q is our approximation of the distribution.
In this case, the KL divergence summarizes the number of additional bits required to represent an event from the random variable.
The better our approximation, the less additional information is required.

"""

import numpy as np

def kl_divergence(p, q):
    div = 0
    for i in range(len(p)):
        div += p[i] * np.log2(p[i]/q[i])
    return div

events = ['red', 'green', 'blue']
p = [0.10, 0.40, 0.50]
q = [0.80, 0.15, 0.05]

print(kl_divergence(p, q))
print(kl_divergence(q, p))

print(kl_divergence(p, p))
print(kl_divergence(q, q))

