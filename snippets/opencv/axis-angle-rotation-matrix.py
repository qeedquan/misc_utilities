#!/usr/bin/env python3

"""

Construct a rotation matrix out of a axis-angle vector representation.

The axis vector is normalized and multiplied by the angle as a scale factor:
v = normalize(axis of rotation)
t = angle of rotation (0 to 2pi)
R = rodrigues_rotation_formula(v*t)

"""

import cv2
import numpy as np
from math import pi
from scipy.spatial.transform import Rotation

def normalize(v):
    norm = np.linalg.norm(v)
    if norm == 0:
       return v
    return v / norm

dt = 1e-3
t = 0
while t <= 2*pi:
    v = np.random.rand(3, 1)
    r = np.transpose(normalize(v)) * t
    R1 = Rotation.from_rotvec(r).as_matrix()
    R2, _ = cv2.Rodrigues(r)
    t += dt
    assert(np.allclose(R1, R2))

