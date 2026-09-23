# https://www.electronics-tutorials.ws/opamp/opamp_4.html
def opamp_inv_sum(Vl, Rl, Rf):
    Al = []
    for i in xrange(len(Vl)):
        Al.append(Rf / Rl[i])

    s = 0
    for i in xrange(len(Vl)):
        s -= Al[i] * Vl[i]

    return s

Vl1 = [2*1e-3, 5*1e-3, 14*1e-3, 53*1e-3, 45*1e-3, 67*1e-3]
Rl1 = [1*1e3, 2*1e3, 3*1e3, 15*1e3, 35*1e3, 73*1e3]
Rf1 = 10*1e3

print(opamp_inv_sum(Vl1, Rl1, Rf1))

