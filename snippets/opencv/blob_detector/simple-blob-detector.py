#!/usr/bin/env python3

"""

OpenCV wraps blob detection in the SimpleBlobDetector class.
It detects blobs by analyzing connected regions and filtering based on shape, size, and intensity characteristics.

"""

import cv2
import numpy as np

# Load image
image = cv2.imread("example-image.jpg", cv2.IMREAD_GRAYSCALE)

# Setup SimpleBlobDetector parameters
params = cv2.SimpleBlobDetector_Params()

# Thresholds for binarization
params.minThreshold = 10
params.maxThreshold = 200

# Filter by Area
params.filterByArea = True
params.minArea = 10

# Filter by Circularity
params.filterByCircularity = True
params.minCircularity = 0.1

# Filter by Convexity
params.filterByConvexity = True
params.minConvexity = 0.87

# Filter by Inertia
params.filterByInertia = True
params.minInertiaRatio = 0.01

# Create a detector with the parameters
detector = cv2.SimpleBlobDetector_create(params)

# Detect blobs
keypoints = detector.detect(image)

# Draw blobs as red circles
output = cv2.drawKeypoints(image, keypoints, np.array([]), (0, 0, 255),
                           cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)

# Show the output
cv2.imshow("Blobs Detected", output)
cv2.waitKey(0)
cv2.destroyAllWindows()
