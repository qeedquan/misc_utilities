#!/usr/bin/env python3

"""

https://www.visiondummy.com/2014/04/draw-error-ellipse-representing-covariance-matrix/

Given two sample sets X and Y generated from a gaussian distribution, if they are completely uncorrelated it will give you a circle if you were to plot X and Y as coordinates.
To make them correlated (rotated), apply a rotation matrix to the X, Y points but only keep one of the points depending on what the angle is.
This will give you a plot that makes the point looks like it's rotated

The eigenvalues/eigenvector of the covariance matrix of the data set will recover the two basis vectors that best fit this data

The axis aligned ellipse that can be drawn using the eigenvector of the covariance matrix has the form:

(x/a)^2 + (y/b)^2 = s

a=sigma_x (standard deviation in x)
b=sigma_y (standard deviation in y)
s=scale that represents the chosen confidence level

To choose the scale s to such that the scale of the resulting ellipse represents a chosen confidence level (e.g. a 95% confidence level corresponds to s=5.991)
use the chi2 inverse cdf since sum of gaussians squared forms a chi2 distribution

"""

import cv2
import numpy as np
import scipy as sc
from math import *

def random_points(n, w, h, t, dist):
    x = dist.rvs(size=n)
    y = dist.rvs(size=n)
    R = np.array(((np.cos(t), -np.sin(t)), (np.sin(t), np.cos(t))))

    for i in range(n):
        p = np.dot(R, (x[i], y[i]))
        if t < np.radians(90):
            x[i] = p[0]
        elif t < np.radians(180):
            y[i] = p[1]
        elif t < np.radians(270):
            y[i] = p[1]
        elif t < np.radians(360):
            x[i] = p[0]

    x0, x1 = min(x), max(x)
    y0, y1 = min(y), max(y)

    xm = sc.interpolate.interp1d([x0, x1], [w/4, 3*w/4])
    ym = sc.interpolate.interp1d([y0, y1], [h/4, 3*h/4])

    q = []
    p = []
    for i in range(n):
        px = int(xm(x[i]) + 0.5)
        py = int(ym(y[i]) + 0.5)
        q.append((x[i], y[i]))
        p.append((px, py))

    cov = np.cov(q, rowvar=False)

    return p, cov

"""

https://stats.stackexchange.com/questions/120179/generating-data-with-a-given-sample-covariance-matrix

There is an algorithm to generate data with the specified covariance matrix (with some restrictions):

1. Define the Sigma matrix to be a covariance matrix with the values we want
2. Generate a 2D normal distribution for the points X

3. Apply the following transformations:

inv() is matrix inverse
chol() is cholesky factorization

X = X - mean(X)
X = X * inv(chol(cov(X)));
X = X * chol(Sigma);

cov(X) should be the same as Sigma up to a sign (abs(cov(X)) = abs(Sigma))

The covariance matrix is closely related to the rotation matrix since it's eigenvectors represents the (X, Y) basis,
so we can use the rotation matrix to represent the covariance matrix we want.

One restriction is that not all angles of the rotation matrix works, since the cholesky factorization works on positive definite matrices.
We are restricted to [0, 45] and [316, 359] degrees.

The covariance matrix also has the property of being symmetric and positive semi-definite.

"""
def test_covariance_matrix_generation():
    n = 1000
    x = sc.stats.norm.rvs(size=n)
    y = sc.stats.norm.rvs(size=n)

    for t in range(360):
        t = np.radians(t)
        S = np.array(((np.cos(t), -np.sin(t)), (np.sin(t), np.cos(t))))

        try:
            X = np.stack((x, y), axis=1)
            X -= np.mean(X, axis=0)

            X = np.dot(X, np.transpose(np.linalg.inv(np.linalg.cholesky(np.cov(X, rowvar=False)))))
            X = np.dot(X, np.transpose(np.linalg.cholesky(S)))
            Y = np.cov(X, rowvar=False)
            assert(np.allclose(abs(Y), abs(S)))
        except:
            print("Failed to factorize at", np.degrees(t))

def main():
    N = 10000
    W = 1200
    H = 800

    font                   = cv2.FONT_HERSHEY_SIMPLEX
    bottomLeftCornerOfText = (32, 32)
    fontScale              = 1
    fontColor              = (0, 0, 0)
    thickness              = 1

    angle = 0
    points, cov = random_points(N, W, H, np.radians(angle), sc.stats.norm)
    img = np.empty((H, W, 3), dtype=np.uint8)
    print(cov, np.degrees(atan2(cov[1][0], cov[0][0])))
    while True:
        img.fill(200)
        for i in range(N):
            cv2.circle(img, points[i], 1, (255, 0, 0))
        img = cv2.putText(img, 'Angle: %d' % (angle), bottomLeftCornerOfText, font, fontScale, fontColor, thickness, cv2.LINE_AA) 
        cv2.imshow("Rotated Normal Distribution", img)

        key = cv2.waitKey(50)
        if key == 27:
            break

        xangle = angle
        if key == 83:
            angle = (angle + 10) % 360
        elif key == 81:
            angle = (angle - 10) % 360

        if angle != xangle:
            points, cov = random_points(N, W, H, np.radians(angle), sc.stats.norm)
            print(cov, np.degrees(atan2(cov[1][0], cov[0][0])))

    cv2.destroyAllWindows()

#test_covariance_matrix_generation()
main()
