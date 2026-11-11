import numpy as np

def cov(x, y, xm, ym, n):
    p = 0.0
    for i in range(len(x)):
        p += (x[i]-xm)*(y[i]-ym)
    return p/(n-1)

X = np.array([4, 4.2, 3.9, 4.3, 4.1])
Y = np.array([2, 2.1, 2.0, 2.1, 2.2])
Z = np.array([0.6, 0.59, 0.58, 0.62, 0.63])

xm = np.average(X)
ym = np.average(Y)
zm = np.average(Z)
n = 5

xx = cov(X, X, xm, ym, n)
xy = cov(X, Y, xm, ym, n)
xz = cov(X, Z, xm, zm, n)
yx = cov(Y, X, ym, xm, n)
yy = cov(Y, Y, ym, ym, n)
yz = cov(Y, Z, ym, zm, n)
zx = cov(Z, X, zm, xm, n)
zy = cov(Z, Y, zm, ym, n)
zz = cov(Z, Z, zm, zm, n)

print("Mean {:.5f} {:.5f} {:.5f}".format(xm, ym, zm))
print("{:.5f} {:.5f} {:.5f}".format(xx, xy, xz))
print("{:.5f} {:.5f} {:.5f}".format(yx, yy, yz))
print("{:.5f} {:.5f} {:.5f}".format(zx, zy, zz))
