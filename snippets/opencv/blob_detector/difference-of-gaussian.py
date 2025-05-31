#!/usr/bin/env python3

"""

Approximation of LoG and is computationally more efficient.
The DoG algorithm computes the difference between two Gaussian-smoothed pictures.
It is handy for recognizing blobs of a given size range. We can vary the standard deviation of the Gaussian kernels to control the size of the detected blobs.

DoG = G(x, y, k*σ) - G(x, y, σ)

"""

from skimage import feature, color, io
import matplotlib.pyplot as plt

image = color.rgb2gray(io.imread("example-image.jpg"))
blobs_dog = feature.blob_dog(image, max_sigma=30, threshold=0.1)

# Compute radii
blobs_dog[:, 2] = blobs_dog[:, 2] * (2 ** 0.5)

# Display
fig, ax = plt.subplots()
ax.imshow(image, cmap='gray')
for y, x, r in blobs_dog:
    c = plt.Circle((x, y), r, color='lime', linewidth=2, fill=False)
    ax.add_patch(c)
plt.show()
