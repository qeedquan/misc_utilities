#!/usr/bin/env python3

"""

Given a point p=(x, y, z), the point projection function the following transformations:

Rotate the point by rvec and translate it by tvec (extrinsic matrix)
Corresponds to multiplying by the modelview matrix (gluLookAt/etc)
p' = R*p + t

Multiply it by the intrinsic matrix
Corresponds to multiplying the projection matrix (gluPerspective/glOrtho/etc)
p' = K*p'

Perform perspective division
p' = (p'[x]/p'[z], p'[y]/p'[z])

Assume no distortions here

"""

import numpy as np
import cv2

def random_point(s=1):
    p = np.zeros((3, 1), np.float32)
    p[0] = np.random.rand()*s
    p[1] = np.random.rand()*s
    p[2] = np.random.rand()*s
    return p

def camera_matrix():
    fx = np.random.rand()
    fy = np.random.rand()
    cx = np.random.rand()*1000
    cy = np.random.rand()*2000
    K = np.zeros((3, 3), np.float32)
    K[0][0] = fx
    K[0][2] = cx
    K[1][1] = fy
    K[1][2] = cy
    K[2][2] = 1
    return K

def perspective_divide(p):
    return np.array([p[0]/p[2], p[1]/p[2]])

for i in range(1000):
    point = random_point()
    rvec = random_point(2) 
    tvec = random_point(100)
    K = camera_matrix()

    projected_point, _ = cv2.projectPoints(point, rvec, tvec, K, None)

    R, _ = cv2.Rodrigues(rvec)

    calculated_point = point
    calculated_point = np.dot(R, calculated_point)
    calculated_point += tvec
    calculated_point = np.dot(K, calculated_point)
    calculated_point = perspective_divide(calculated_point)

    print(calculated_point)
    print(projected_point)
    print()

